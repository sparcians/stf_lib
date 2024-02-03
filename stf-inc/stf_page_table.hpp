#ifndef __STF_PAGE_TABLE_HPP__
#define __STF_PAGE_TABLE_HPP__

#include <iostream>
#include <numeric>
#include <functional>
#include <unordered_map>

#include "boost_wrappers/flat_map.hpp"
#include "boost_wrappers/small_vector.hpp"
#include "format_utils.hpp"
#include "stf_item.hpp"
#include "stf_record_types.hpp"
#include "stf_satp_decoder.hpp"
#include "util.hpp"

#define stf_translation_assert(cond, orig_va, index) \
    do { if(STF_EXPECT_FALSE(!(cond))) { throw STFTranslationException(orig_va, index); } } while(0)

namespace stf {
    /**
     * \class STFTranslationException
     * \brief Exception thrown when address translation fails
     */
    class STFTranslationException : public std::exception {
        private:
            const uint64_t addr_;
            const uint64_t index_;

        public:
            /**
             * Constructor
             */
            explicit STFTranslationException(const uint64_t addr, const uint64_t index) :
                addr_(addr),
                index_(index)
            {
            }

            /**
             * Generates the exception message
             */
            const char* what() const noexcept override {
                static constexpr char MSG_TEMPLATE_SEGMENT_1[] = "Address translation failure for VA 0x";
                static constexpr char MSG_TEMPLATE_SEGMENT_2[] = " at index 0x";
                static constexpr char DIGITS[] = "0123456789abcdef";

                // Translate the address to hex the hard way so that we don't risk
                // throwing any exceptions here
                static constexpr int HEX_LEN = format_utils::numHexDigits<uint64_t>();

                static thread_local char msg[sizeof(MSG_TEMPLATE_SEGMENT_1) + HEX_LEN + sizeof(MSG_TEMPLATE_SEGMENT_2) + HEX_LEN - 1];

                strcpy(msg, MSG_TEMPLATE_SEGMENT_1);

                static constexpr int FIRST_HEX_START = sizeof(MSG_TEMPLATE_SEGMENT_1) - 1;
                for(size_t i = FIRST_HEX_START, j = 4*(HEX_LEN - 1); i < FIRST_HEX_START + HEX_LEN; ++i, j -= 4) {
                    msg[i] = DIGITS[(addr_ >> j) & 0xf];
                }

                strcpy(msg + FIRST_HEX_START + HEX_LEN, MSG_TEMPLATE_SEGMENT_2);

                static constexpr int SECOND_HEX_START = FIRST_HEX_START + HEX_LEN + sizeof(MSG_TEMPLATE_SEGMENT_2) - 1;
                for(size_t i = SECOND_HEX_START, j = 4*(HEX_LEN - 1); i < sizeof(msg) - 1; ++i, j -= 4) {
                    msg[i] = DIGITS[(index_ >> j) & 0xf];
                }

                msg[sizeof(msg) - 1] = '\0';

                return msg;
            }

            /**
             * Gets the virtual address that generated the exception
             */
            uint64_t getAddress() const {
                return addr_;
            }

            /**
             * Gets the instruction index that generated the exception
             */
            uint64_t getIndex() const {
                return index_;
            }
    };

    /**
     * \class STFPageTable
     * \brief Tracks page table entries by the instruction index they become
     * valid, providing the correct address translation at any point in a trace
     */
    class STFPageTable {
        private:

            /**
             * \class STFPageTableImplBase
             * \brief Non-templatized page table interface class
             */
            class STFPageTableImplBase {
                protected:
                    // If the trace doesn't start with a mode change event, assume it is in machine mode
                    boost::container::flat_map<uint64_t, EXECUTION_MODE> modes_{{0, EXECUTION_MODE::MACHINE_MODE}}; /**< Maps instruction indices to the correct execution mode */

                public:
                    virtual inline ~STFPageTableImplBase() = default;
                    virtual uint64_t translate(const uint64_t orig_va, const uint64_t index) const = 0;
                    virtual void update(const PageTableWalkRecord& rec) = 0;
                    virtual void updateSatp(const InstRegRecord& reg_rec, const uint64_t index) = 0;

                    /**
                     * Updates the execution mode at the given instruction index
                     */
                    __attribute__((always_inline))
                    inline void updateMode(const EXECUTION_MODE mode, const uint64_t index) {
                        // Don't use emplace() because we need to overwrite the default index 0 entry if
                        // the trace specifies something different
                        modes_[index] = mode;
                    }

                    virtual void clear() = 0;
            };

            /**
             * \class STFPageTableImpl
             * \brief Provides page table implementation depending on the INST_IEM present in a trace
             */
            template<INST_IEM iem>
            class STFPageTableImpl final : public STFPageTableImplBase {
                private:
                    /**
                     * \typedef SmallVectorMap
                     * \brief boost::flat_map that stores its entries in a boost::small_vector for faster allocations
                     */
                    template<typename Key, typename Val, size_t N>
                    using SmallVectorMap = boost::container::flat_map<Key,
                                                                      Val,
                                                                      std::less<Key>,
                                                                      boost::container::small_vector<std::pair<Key, Val>, N>>;

                    /**
                     * \class SatpEntry
                     * \brief Wrapper class that points to the correct page table implementation for a particular
                     * SATP register value
                     */
                    class SatpEntry {
                        private:
                            /**
                             * \class PageTableMapBase
                             * \brief Non-templatized interface class for PageTableMap
                             */
                            class PageTableMapBase {
                                public:
                                    virtual ~PageTableMapBase() = default;
                                    virtual void update(const PageTableWalkRecord& pte) = 0;
                                    virtual uint64_t translate(const uint64_t orig_va,
                                                               const uint64_t index) const = 0;
                            };

                            /**
                             * \class PageTableMap
                             * \brief Tracks PTEs and implements address translation for a particular
                             * virtual addressing mode
                             */
                            template<VAMode virt_addr_type, typename DummyType = void>
                            class PageTableMap final : public PageTableMapBase {
                                private:
                                    // The NO_TRANSLATION case is handled by a template specialization
                                    static_assert(virt_addr_type != VAMode::NO_TRANSLATION);

                                    template<size_t N>
                                    static inline constexpr uint64_t mask_() {
                                        return byte_utils::bitMask<uint64_t, N>();
                                    }

                                    using va_props = VAProperties<virt_addr_type>;

                                    /**
                                     * \class PageTableEntry
                                     * \brief Holds decoded PTE information
                                     */
                                    class PageTableEntry {
                                        private:
                                            static inline constexpr uint64_t getAddr_(const uint64_t pte_value) {
                                                constexpr uint64_t PHYS_ADDR_MASK = mask_<va_props::PHYS_ADDR_SIZE>();
                                                constexpr size_t PTE_TO_PHYS_PAGE_SHIFT = 10;

                                                return ((pte_value >> PTE_TO_PHYS_PAGE_SHIFT) << PAGE_OFFSET_SIZE) & PHYS_ADDR_MASK;
                                            }

                                            static inline constexpr bool getLeaf_(const uint64_t pte_value) {
                                                constexpr uint64_t LEAF_MASK = 0xA;
                                                return !!(pte_value & LEAF_MASK);
                                            }

                                            uint64_t phys_addr_; /**< PTE base address */
                                            bool is_leaf_; /**< If true, this is a leaf PTE */

                                        public:
                                            /**
                                             * Constructs a PageTableEntry
                                             */
                                            explicit PageTableEntry(const uint64_t pte_value) :
                                                phys_addr_(getAddr_(pte_value)),
                                                is_leaf_(getLeaf_(pte_value))
                                            {
                                            }

                                            /**
                                             * Gets whether this is a leaf PTE
                                             */
                                            __attribute__((always_inline))
                                            inline bool isLeaf() const {
                                                return is_leaf_;
                                            }

                                            /**
                                             * Gets the base address for this PTE
                                             */
                                            __attribute__((always_inline))
                                            inline uint64_t getAddress() const {
                                                return phys_addr_;
                                            }

                                            /**
                                             * Equality operator
                                             */
                                            __attribute__((always_inline))
                                            inline bool operator==(const PageTableEntry& rhs) const {
                                                return (phys_addr_ == rhs.phys_addr_) && (is_leaf_ == rhs.is_leaf_);
                                            }

                                            /**
                                             * Inequality operator
                                             */
                                            __attribute__((always_inline))
                                            inline bool operator!=(const PageTableEntry& rhs) const {
                                                return !(*this == rhs);
                                            }
                                    };

                                    const uint64_t satp_base_addr_; /**< Base address extracted from the SATP */

                                    // 2-level map
                                    // * Level 1: indexed by page table entry physical address
                                    // * Level 2: indexed by instruction index. Preallocates 1 entry under the assumption
                                    //            that page reallocation is rare.
                                    std::unordered_map<uint64_t, SmallVectorMap<uint64_t, PageTableEntry, 1>> phys_pages_;

                                public:
                                    /**
                                     * Constructs a PageTableMap
                                     */
                                    explicit PageTableMap(const uint64_t satp_base_addr) :
                                        satp_base_addr_(satp_base_addr)
                                    {
                                    }

                                    /**
                                     * Updates the page table with a new page table walk record
                                     */
                                    void update(const PageTableWalkRecord& rec) final {
                                        const uint64_t index = rec.getFirstAccessIndex();

                                        for(const auto& pte: rec.getPTEs()) {
                                            PageTableEntry new_entry(pte.getPTE());

                                            if(const auto [pte_it, inserted] = phys_pages_.try_emplace(pte.getPA()); inserted) {
                                                // We've never seen a PTE at this address before, so go ahead and add
                                                // the new entry
                                                pte_it->second.emplace(std::piecewise_construct,
                                                                       std::forward_as_tuple(index),
                                                                       std::forward_as_tuple(std::move(new_entry)));
                                            }
                                            else {
                                                // This PTE already exists
                                                auto& pte_versions = pte_it->second;
                                                auto it = pte_versions.upper_bound(index);
                                                stf_assert(it != pte_versions.begin(),
                                                           "Hit an existing PTE but could not find any actual entries");
                                                const auto it_backup = it--;
                                                // Don't do anything if the content of the PTE hasn't changed
                                                if(it->second != new_entry) {
                                                    stf_assert(it->first != index,
                                                               "PTE has two different values at the same instruction index:\nIndex: " << index
                                                               << "\nCurrent Entry:\n    Leaf: " << it->second.isLeaf()
                                                               << "\n    Address: " << std::hex << it->second.getAddress() << std::dec
                                                               << "\nNew Entry:\n    Leaf: " << new_entry.isLeaf()
                                                               << "\n    Address: " << std::hex << new_entry.getAddress());
                                                    pte_versions.emplace_hint(it_backup,
                                                                              std::piecewise_construct,
                                                                              std::forward_as_tuple(index),
                                                                              std::forward_as_tuple(std::move(new_entry)));
                                                }
                                            }
                                        }
                                    }

                                    uint64_t translate(const uint64_t orig_va, const uint64_t index) const final {
                                        static constexpr uint64_t VIRT_ADDR_MASK = mask_<va_props::VIRT_ADDR_SIZE>();
                                        static constexpr unsigned int NUM_VPNS = (va_props::VIRT_ADDR_SIZE - PAGE_OFFSET_SIZE) / va_props::VPN_SIZE;
                                        static constexpr uint64_t VPN_MASK = mask_<va_props::VPN_SIZE>();

                                        const uint64_t vpn = (orig_va & VIRT_ADDR_MASK) >> PAGE_OFFSET_SIZE;

                                        bool is_leaf = false;

                                        uint64_t pte_base_addr = satp_base_addr_;

                                        int vpn_shift_amount;

                                        // Implements the address translation algorithm from the RISC-V Privileged Architecture spec
                                        for(vpn_shift_amount = va_props::VPN_SIZE * (NUM_VPNS - 1); vpn_shift_amount >= 0; vpn_shift_amount -= va_props::VPN_SIZE) {
                                            const uint64_t next_pte_addr = pte_base_addr + (((vpn >> vpn_shift_amount) & VPN_MASK) << va_props::PTE_SHIFT);
                                            const auto next_pte_it = phys_pages_.find(next_pte_addr);

                                            // We expect the trace to contain all of the walk information
                                            stf_translation_assert(next_pte_it != phys_pages_.end(), orig_va, index);

                                            const auto& pte_versions = next_pte_it->second;
                                            const auto it = pte_versions.upper_bound(index);

                                            // Same reasoning as above
                                            stf_translation_assert(it != pte_versions.begin(), orig_va, index);

                                            const auto& pte = std::prev(it)->second;

                                            is_leaf = pte.isLeaf();
                                            pte_base_addr = pte.getAddress();

                                            if(STF_EXPECT_FALSE(is_leaf)) {
                                                // We hit a leaf entry, so we're done
                                                break;
                                            }
                                        }

                                        // The hardware would issue a page fault if this check failed, but the trace
                                        // should have the complete walk information
                                        stf_translation_assert(is_leaf || vpn_shift_amount >= 0, orig_va, index);

                                        // Accounts for hugepages (vpn_shift_amount != 0 case)
                                        const uint64_t va_offset_mask = byte_utils::bitMask<uint64_t>(static_cast<unsigned int>(vpn_shift_amount) + PAGE_OFFSET_SIZE);
                                        return pte_base_addr | (orig_va & va_offset_mask);
                                    }
                            };

                            // Specialization for no-translation case
                            template<typename Dummy>
                            class PageTableMap<VAMode::NO_TRANSLATION, Dummy> : public PageTableMapBase {
                                public:
                                    void update(const PageTableWalkRecord&) final {
                                        // There's nothing to update if translation isn't enabled
                                        stf_throw("SATP says translation not enabled, but we're doing translation!");
                                    }

                                    uint64_t translate(const uint64_t orig_va,
                                                       const uint64_t) const final {
                                        // VA == PA
                                        return orig_va;
                                    }
                            };

                            /**
                             * Constructs a page table configured for the given SATP register value
                             */
                            static inline std::unique_ptr<PageTableMapBase> constructPageTable_(const Satp& satp_value);

                            std::unique_ptr<PageTableMapBase> page_table_ptr_; /**< Holds the page table for this SATP config */

                        public:
                            /**
                             * \typedef PageTable
                             * \brief Convenience typedef for outer classes
                             */
                            using PageTable = PageTableMapBase;

                            /**
                             * Constructs a SatpEntry
                             */
                            explicit SatpEntry(const uint64_t satp_value) :
                                page_table_ptr_(constructPageTable_(SatpDecoder<iem>::decode(satp_value)))
                            {
                            }

                            /**
                             * Gets a raw pointer to the underlying page table
                             */
                            __attribute__((always_inline))
                            inline PageTable* get() {
                                return page_table_ptr_.get();
                            }
                    };

                    // The value of the SATP register can change during execution, but the page table a given value maps to
                    // will remain constant. This means we can allocate the SatpEntry once and then use pointers to pick the
                    // correct entry for a given instruction index.

                    // Preallocates 2 entries since there's usually one no-translation case and one translation-enabled case
                    // Points directly to the underlying page table object to save some indirection
                    SmallVectorMap<uint64_t, typename SatpEntry::PageTable*, 2> indexed_satp_entries_; /**< Maps instruction indices to the correct SATP config */

                    std::unordered_map<uint64_t, SatpEntry> satp_entries_; /**< Maps SATP register values to their respective page tables */

                public:
                    /**
                     * Translates the given VA using the page table setup that was valid at the given instruction index
                     */
                    uint64_t translate(const uint64_t orig_va, const uint64_t index) const final {
                        auto mode_it = modes_.upper_bound(index);

                        stf_assert(mode_it != modes_.begin(), "Failed to find execution mode for index " << index);

                        // If we're in machine mode, translation is disabled.
                        if(std::prev(mode_it)->second == EXECUTION_MODE::MACHINE_MODE) {
                            return orig_va;
                        }

                        const auto satp_it = indexed_satp_entries_.upper_bound(index);

                        stf_translation_assert(satp_it != indexed_satp_entries_.begin(), orig_va, index);

                        return std::prev(satp_it)->second->translate(orig_va, index);
                    }

                    /**
                     * Updates the page table with a new page table walk record
                     */
                    void update(const PageTableWalkRecord& rec) final {
                        const uint64_t index = rec.getFirstAccessIndex();

                        const auto satp_it = indexed_satp_entries_.upper_bound(index);

                        stf_assert(satp_it != indexed_satp_entries_.begin(),
                                   "No SATP values known, but we're doing translation!");

                        std::prev(satp_it)->second->update(rec);
                    }

                    /**
                     * Updates the SATP register value at the given instruction index
                     */
                    void updateSatp(const InstRegRecord& reg_rec, const uint64_t index) final {
                        const uint64_t satp_data = reg_rec.getScalarData();
                        const auto it = satp_entries_.try_emplace(satp_data, satp_data).first;
                        indexed_satp_entries_.try_emplace(index, it->second.get());
                    }

                    /**
                     * Clears all of the page tables
                     */
                    void clear() final {
                        modes_.clear();
                        indexed_satp_entries_.clear();
                        satp_entries_.clear();
                    }
            };

            std::unique_ptr<STFPageTableImplBase> ptr_; /**< Points to the underlying page table implementation */

        public:
            /**
             * Default constructor
             */
            STFPageTable() = default;

            /**
             * Constructs an STFPageTable preconfigured for the given IEM
             */
            explicit STFPageTable(const INST_IEM iem) {
                reset(iem);
            }

            /**
             * Resets the page table to the given INST_IEM
             */
            inline void reset(const INST_IEM iem) {
                switch(iem) {
                    case INST_IEM::STF_INST_IEM_RV32:
                        ptr_ = std::make_unique<STFPageTableImpl<INST_IEM::STF_INST_IEM_RV32>>();
                        break;
                    case INST_IEM::STF_INST_IEM_RV64:
                        ptr_ = std::make_unique<STFPageTableImpl<INST_IEM::STF_INST_IEM_RV64>>();
                        break;
                    case INST_IEM::STF_INST_IEM_INVALID:
                    case INST_IEM::__RESERVED_END:
                        stf_throw("Invalid INST_IEM specified: " << iem);
                }
            }

            /**
             * Translates the given virtual address. If no index is given, the most recent known PTEs are used.
             */
            __attribute__((always_inline))
            inline uint64_t translate(const uint64_t orig_va,
                                      const uint64_t index = std::numeric_limits<uint64_t>::max()) const {
                return ptr_ ? ptr_->translate(orig_va, index) : 0;
            }

            /**
             * Updates the page table with a new page table walk record
             */
            __attribute__((always_inline))
            inline void update(const PageTableWalkRecord& rec) {
                ptr_->update(rec);
            }

            /**
             * Updates the SATP register value at the given instruction index
             */
            __attribute__((always_inline))
            inline void updateSatp(const InstRegRecord& reg_rec, const uint64_t index) {
                ptr_->updateSatp(reg_rec, index);
            }

            /**
             * Updates the execution mode at the given instruction index
             */
            __attribute__((always_inline))
            inline void updateMode(const EXECUTION_MODE mode, const uint64_t index) {
                ptr_->updateMode(mode, index);
            }

            /**
             * Clears the page table without changing the IEM
             */
            inline void clear() {
                ptr_->clear();
            }
    };

    // Page table factory method for RV32 IEM
    template<>
    inline std::unique_ptr<STFPageTable::STFPageTableImpl<INST_IEM::STF_INST_IEM_RV32>::SatpEntry::PageTableMapBase>
    STFPageTable::STFPageTableImpl<INST_IEM::STF_INST_IEM_RV32>::SatpEntry::constructPageTable_(const Satp& satp) {
        switch(satp.getVAMode()) {
            case VAMode::SV32:
                return std::make_unique<PageTableMap<VAMode::SV32>>(satp.getBaseAddr());
            case VAMode::NO_TRANSLATION:
                return std::make_unique<PageTableMap<VAMode::NO_TRANSLATION>>();
            default:
                stf_throw("The only supported translation modes in RV32 are SV32 and no translation");
        }
    }

    // Page table factory method for RV64 IEM
    template<>
    inline std::unique_ptr<STFPageTable::STFPageTableImpl<INST_IEM::STF_INST_IEM_RV64>::SatpEntry::PageTableMapBase>
    STFPageTable::STFPageTableImpl<INST_IEM::STF_INST_IEM_RV64>::SatpEntry::constructPageTable_(const Satp& satp) {
        switch(satp.getVAMode()) {
            case VAMode::SV32:
                stf_throw("SV32 not supported in RV64 mode");
            case VAMode::SV39:
                return std::make_unique<PageTableMap<VAMode::SV39>>(satp.getBaseAddr());
            case VAMode::SV48:
                return std::make_unique<PageTableMap<VAMode::SV48>>(satp.getBaseAddr());
            case VAMode::SV57:
                return std::make_unique<PageTableMap<VAMode::SV57>>(satp.getBaseAddr());
            case VAMode::NO_TRANSLATION:
                return std::make_unique<PageTableMap<VAMode::NO_TRANSLATION>>();
        }
        __builtin_unreachable();
    }
} // end namespace stf

#undef stf_translation_assert

#endif
