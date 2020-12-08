#ifndef __STF_RECORD_TYPES_HPP__
#define __STF_RECORD_TYPES_HPP__

#include <algorithm>
#include <array>
#include <locale>
#include <memory>
#include <string>
#include <vector>
#include "format_utils.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"
#include "stf_record_factory.hpp"
#include "stf_record_interfaces.hpp"
#include "stf_serializable_container.hpp"
#include "stf_enums.hpp"
#include "stf_reg_def.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class STFIdentifierRecord
     *
     * Indicates the beginning of an STF file
     *
     */
    class STFIdentifierRecord : public TypeAwareSTFRecord<STFIdentifierRecord> {
        private:
            static constexpr std::string_view IDENT_STR_ = "STF";
            std::array<char, 3> str_;

        public:
            STFIdentifierRecord() :
                STFIdentifierRecord({'S', 'T', 'F'})
            {
            }

            /**
             * Constructs an STFIdentifierRecord
             * \param str id string
             */
            explicit STFIdentifierRecord(const std::array<char, 3>& str) :
                str_(str)
            {
            }

            /**
             * Constructs an STFIdentifierRecord
             * \param strm STFIFstream to read from
             */
            explicit STFIdentifierRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Returns whether the id string is valid
             */
            bool isValid() const {
                return IDENT_STR_.compare(0, 3, str_.data(), str_.size()) == 0;
            }

            /**
             * Packs an STFIdentifierRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, str_);
            }

            /**
             * Unpacks an STFIdentifierRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, str_);
            }

            /**
             * Formats an STFIdentifierRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << str_[0] << str_[1] << str_[2];
            }
    };

    /**
     * \class VersionRecord
     *
     * Defines STF version
     *
     */
    class VersionRecord : public TypeAwareSTFRecord<VersionRecord> {
        private:
            uint32_t major_ = 0;              /**< major verion number */
            uint32_t minor_ = 0;              /**< minor version number */

        public:
            VersionRecord() = default;

            /**
             * Constructs a VersionRecord with the given major and minor numbers
             * \param major major version number
             * \param minor minor version number
             */
            VersionRecord(uint32_t major, uint32_t minor) :
                major_(major),
                minor_(minor)
            {
            }

            /**
             * Constructs a VersionRecord
             * \param strm STFIFstream to read from
             */
            explicit VersionRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Gets the major version number
             */
            uint32_t getMajor() const { return major_; }

            /**
             * Gets the minor version number
             */
            uint32_t getMinor() const { return minor_; }

            /**
             * Packs a VersionRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, major_, minor_);
            }

            /**
             * Unpacks a VersionRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, major_, minor_);
            }

            /**
             * Formats a VersionRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << major_ << '.' << minor_;
            }
    };

    /**
     * \class CommentRecord
     *
     * Defines comment
     *
     */
    class CommentRecord : public TypeAwareSTFRecord<CommentRecord> {
        private:
            SerializableString<uint32_t> data_;                /**< the comment */

        public:
            CommentRecord() = default;

            /**
             * Constructs a CommentRecord
             * \param data comment string data
             */
            explicit CommentRecord(std::string data) :
                data_(std::move(data))
            {
            }

            /**
             * Constructs a CommentRecord
             * \param strm STFIFstream to read from
             */
            explicit CommentRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Gets the comment data
             */
            const std::string& getData() const { return data_; }

            /**
             * Packs a CommentRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, data_);
            }

            /**
             * Unpacks a CommentRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, data_);
            }

            /**
             * Formats a VersionRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << data_;
            }

            /**
             * Writes a CommentRecord to an ostream
             */
            friend std::ostream& operator<<(std::ostream& os, const CommentRecord& comment);
    };

    /**
     * \class ISARecord
     *
     * Record used to hold an ISA family
     */
    class ISARecord : public GenericSingleDataRecord<ISARecord, ISA> {
        public:
            ISARecord() = default;

            /**
             * Constructs an ISARecord
             * \param strm STFIFstream to read from
             */
            explicit ISARecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an ISARecord
             * \param isa isa type
             */
            explicit ISARecord(ISA isa) :
                GenericSingleDataRecord(isa)
            {
            }

            /**
             * Gets the ISA type
             */
            ISA getISA() const { return GenericSingleDataRecord::getData_(); }
    };

    /**
     * \class InstIEMRecord
     *
     * Defines instruction encoding mode
     *
     */
    class InstIEMRecord : public GenericSingleDataRecord<InstIEMRecord, INST_IEM> {
        public:
            InstIEMRecord() = default;

            /**
             * Constructs an InstIEMRecord
             * \param strm STFIFstream to read from
             */
            explicit InstIEMRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstIEMRecord
             * \param mode IEM for the record
             */
            explicit InstIEMRecord(INST_IEM mode) :
                GenericSingleDataRecord(mode)
            {
            }

            /**
             * Gets the mode
             */
            INST_IEM getMode() const { return GenericSingleDataRecord::getData_(); }
    };

    /**
     * \class ForcePCRecord
     *
     * Initializes PC
     *
     */
    class ForcePCRecord : public GenericPCTargetRecord<ForcePCRecord> {
        public:
            ForcePCRecord() = default;

            /**
             * Constructs a ForcePCRecord
             * \param strm STFIFstream to read from
             */
            explicit ForcePCRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an ForcePCRecord
             * \param addr instruction address
             */
            explicit ForcePCRecord(uint64_t addr) :
                GenericPCTargetRecord(addr)
            {
            }
    };

    /**
     * \class EndOfHeaderRecord
     *
     * Marks end of STF header
     *
     */
    class EndOfHeaderRecord : public GenericEmptyRecord<EndOfHeaderRecord> {
        public:
            EndOfHeaderRecord() = default;

            /**
             * Constructs an EndOfHeaderRecord
             * \param strm STFIFstream to read from
             */
            explicit EndOfHeaderRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }
    };

    /**
     * \class PageTableWalkRecord
     *
     * Contains all page table entries necessary to perform a table walk for VA -> PA translation
     */
    class PageTableWalkRecord : public TypeAwareSTFRecord<PageTableWalkRecord> {
        public:
            /**
             * \enum ATTR
             * Defines the different PTE attributes
             */
            enum class ATTR : uint32_t {
                PAGE_ATTR_WRITE_THROUGH      = 0x1,  /**< Page is write-through */
                PAGE_ATTR_CACHE_INHIBITED    = 0x2,  /**< Page is non-cacheable */
                PAGE_ATTR_COHERENT           = 0x4,  /**< Page is coherent */
                PAGE_ATTR_GUARDED            = 0x8,  /**< Page is guarded */
                PAGE_ATTR_SECURED            = 0x10, /**< Page is secured */
                PAGE_ATTR_PRIVILEGED         = 0x20, /**< Page is privileged */
                PAGE_ATTR_MODIFIED           = 0x40  /**< Page is modified */
            };

            /**
             * \class PTE
             *
             * Contains a single PTE
             */
            class PTE {
                private:
                    uint64_t pa_ = page_utils::INVALID_PHYS_ADDR; /**< physical address */
                    uint64_t pte_; /**< raw PTE */

                public:
                    PTE() = default;

                    /**
                     * Constructs a PTE
                     * \param pa physical address
                     * \param pte raw PTE contents
                     */
                    PTE(const uint64_t pa, const uint64_t pte) :
                        pa_(pa),
                        pte_(pte)
                    {
                    }

                    /**
                     * Constructs a PTE
                     * \param reader STFIFstream to read from
                     */
                    explicit PTE(STFIFstream& reader) {
                        unpack_impl(reader);
                    }

                    /**
                     * Serializes a PTE into an STFOFstream
                     * \param writer STFOFstream to use
                     */
                    inline void pack_impl(STFOFstream& writer) const {
                        write_(writer, pa_, pte_);
                    }

                    /**
                     * Deserializes a PTE from an STFIFstream
                     * \param reader STFIFstream to use
                     */
                    __attribute__((always_inline))
                    inline void unpack_impl(STFIFstream& reader) {
                        read_(reader, pa_, pte_);
                    }

                    /**
                     * Gets the PA
                     */
                    uint64_t getPA() const {
                        return pa_;
                    }

                    /**
                     * Gets the raw PTE
                     */
                    uint64_t getPTE() const {
                        return pte_;
                    }

                    /**
                     * Equality operator
                     * \param rhs PTE to compare against
                     */
                    bool operator==(const PTE& rhs) const {
                        return (pa_ == rhs.pa_) && (pte_ == rhs.pte_);
                    }

                    /**
                     * Inequality operator
                     * \param rhs PTE to compare against
                     */
                    bool operator!=(const PTE& rhs) const {
                        return !operator==(rhs);
                    }

                    /**
                     * Gets the PPN from the PTE
                     */
                    uint64_t getPPN() const {
                        static constexpr size_t PTE_SHIFT = 10;
                        return (pte_ >> PTE_SHIFT);
                    }
            };

        private:
            uint64_t va_; /**< virtual address */
            uint64_t index_; /**< instruction index that this PTE becomes valid */
            uint32_t page_size_; /**< size of the page in bytes */
            uint32_t page_size_shift_; /**< PPN is shifted by this amount to get the page address */
            SerializableVector<PTE, uint8_t> ptes_; /**< Raw PTEs that consitute the page walk needed to translate this address */

        public:
            PageTableWalkRecord() = default;

            /**
             * Constructs a PageTableWalkRecord
             * \param strm STFIFstream to read from
             */
            explicit PageTableWalkRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a PageTableWalkRecord
             * \param va virtual address
             * \param index instruction index that this PTE becomes valid
             * \param page_size size of the page in bytes
             * \param ptes Raw PTEs that constitute the page walk needed to translate this address
             */
            PageTableWalkRecord(const uint64_t va, const uint64_t index, const uint32_t page_size, std::vector<PTE> ptes) :
                va_(va),
                index_(index),
                page_size_(page_size),
                page_size_shift_(math_utils::constexpr_log::floor_log2(page_size)),
                ptes_(std::move(ptes))
            {
            }

            /**
             * Packs a PageTableWalkRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, va_, index_, page_size_, ptes_);
            }

            /**
             * Unpacks a PageTableWalkRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                // Bundle the POD members together in a single read
                read_(reader, va_, index_, page_size_);
                // Read the PTEs
                reader >> ptes_;
                // Calculate page size shift
                page_size_shift_ = math_utils::constexpr_log::floor_log2(page_size_);
            }

            /**
             * Gets the VA
             */
            uint64_t getVA() const {
                return va_;
            }

            /**
             * Sets the instruction index for the first access to this translation
             */
            void setFirstAccessIndex(uint64_t index) {
                index_ = index;
            }

            /**
             * Gets the instruction index for the first access to this translation
             */
            uint64_t getFirstAccessIndex() const {
                return index_;
            }

            /**
             * Gets the size of the page in bytes
             */
            uint32_t getPageSize() const {
                return page_size_;
            }

            /**
             * Gets the PTEs in the table walk
             */
            const std::vector<PTE>& getPTEs() const {
                return ptes_;
            }

            /**
             * Gets the leaf PTE in the table walk
             */
            const PTE& getLeafPTE() const {
                return ptes_.back();
            }

            /**
             * Gets the physical page address
             */
            uint64_t getPhysicalPageAddr() const {
                return getLeafPTE().getPPN() << page_size_shift_;
            }

            /**
             * Gets the number of PTEs read during the table walk
             */
            size_t getNumPTEs() const {
                return ptes_.size();
            }

            /**
             * Sets the instruction index when this PTE becomes valid
             * \param index Index to set
             */
            void setIndex(uint64_t index) {
                index_ = index;
            }

            /**
             * Equality operator
             * \param rhs PageTableWalkRecord to compare against
             */
            bool operator==(const PageTableWalkRecord& rhs) const {
                return (va_ == rhs.va_) &&
                       (page_size_ == rhs.page_size_) &&
                       (ptes_.size() == rhs.ptes_.size()) &&
                       // Reverse search since the leaf nodes are probably going to change more frequently
                       std::equal(ptes_.rbegin(), ptes_.rend(), rhs.ptes_.rbegin());
            }

            /**
             * Inequality operator
             * \param rhs PageTableWalkRecord to compare against
             */
            bool operator!=(const PageTableWalkRecord& rhs) const {
                return !operator==(rhs);
            }

            /**
             * Formats a PageTableWalkRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                format_utils::formatVA(os, getVA());
                os << " size ";
                format_utils::formatHex(os, getPageSize());
                os << " first_count ";
                format_utils::formatDec(os, getFirstAccessIndex(), format_utils::PTE_ACCESS_INDEX_WIDTH);
                os << std::endl;

                if(getNumPTEs() > 0) {
                    format_utils::formatLabel(os, "PTDESC");
                    os << "Num Descriptors: " << std::dec << getNumPTEs() << std::endl;
                    size_t i = 0;
                    for(const auto& p: getPTEs()) {
                        format_utils::formatWidth(os, "DESC", format_utils::PTE_DESC_LABEL_WIDTH);
                        format_utils::formatDec(os, i);
                        os << "  PA:";
                        format_utils::formatVA(os, p.getPA()); // In this case we want the PA to have all 64 bits
                        os << " DESC:";
                        format_utils::formatData(os, p.getPTE());
                        os << std::endl;
                        ++i;
                    }
                }
            }
    };

    /**
     * Writes a PageTableWalkRecord::PTE to an STFOFstream
     */
    STFOFstream& operator<<(STFOFstream& writer, const PageTableWalkRecord::PTE& rec);

    /**
     * Reads a PageTableWalkRecord::PTE from an STFIFstream
     */
    STFIFstream& operator>>(STFIFstream& reader, PageTableWalkRecord::PTE& rec);

    /**
     * Formats a PageTableWalkRecord to an ostream
     */
    std::ostream& operator<<(std::ostream& os, const PageTableWalkRecord& pte);

    /**
     * \class ProcessIDExtRecord
     *
     * ProcessIDExtRecord;
     *  Extendted Process ID Record.
     *
     */
    class ProcessIDExtRecord : public TypeAwareSTFRecord<ProcessIDExtRecord> {
        private:
            uint32_t tgid_;              /**< process ID */
            uint32_t tid_;               /**< thread ID */
            uint32_t asid_;              /**< Address Space ID */

        public:
            ProcessIDExtRecord() = default;

            /**
             * Constructs a ProcessIDExtRecord
             * \param strm STFIFstream to read from
             */
            explicit ProcessIDExtRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a ProcessIDExtRecord
             * \param tgid tgid
             * \param tid tid
             * \param asid asid
             */
            ProcessIDExtRecord(uint32_t tgid, uint32_t tid, uint32_t asid) :
                tgid_(tgid),
                tid_(tid),
                asid_(asid)
            {
            }

            /**
             * Gets the TGID
             */
            uint32_t getTGID() const { return tgid_; }

            /**
             * Gets the TID
             */
            uint32_t getTID() const { return tid_; }

            /**
             * Gets the ASID
             */
            uint32_t getASID() const { return asid_; }

            /**
             * Packs a ProcessIDExtRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, tgid_, tid_, asid_);
            }

            /**
             * Unpacks a ProcessIDExtRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, tgid_, tid_, asid_);
            }

            /**
             * Formats a ProcessIDExtRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << "asid = ";
                format_utils::formatHex(os, asid_);
                os << " tgid = ";
                format_utils::formatHex(os, tgid_);
                os << " tid = ";
                format_utils::formatHex(os, tid_);
            }
    };


    /**
     * \class EventRecord
     *
     * Event number
     *
     */
    class EventRecord : public TypeAwareSTFRecord<EventRecord> {
        public:
            static constexpr uint32_t INTERRUPT_MASK = (1U << 31); /**< Interrupt mask */
            static constexpr uint32_t SPECIAL_MASK = (1U << 30); /**< Special event mask */

            /**
             * \enum TYPE
             *
             * Defines the different event types
             */
            enum class TYPE : uint32_t {
                // Synchronous Exceptions/Traps
                INST_ADDR_MISALIGN          = 0x0, /**< Misaligned instruction exception */
                INST_ADDR_FAULT             = 0x1, /**< Instruction address fault */
                ILLEGAL_INST                = 0x2, /**< Illegal instruction exception */
                BREAKPOINT                  = 0x3, /**< Breakpoint event */
                LOAD_ADDR_MISALIGN          = 0x4, /**< Misaligned load exception */
                LOAD_ACCESS_FAULT           = 0x5, /**< Load address fault */
                STORE_ADDR_MISALIGN         = 0x6, /**< Misaligned store exception */
                STORE_ACCESS_FAULT          = 0x7, /**< Store address fault */
                USER_ECALL                  = 0x8, /**< User-mode syscall */
                SUPERVISOR_ECALL            = 0x9, /**< Supervisor-mode syscall */
                HYPERVISOR_ECALL            = 0xa, /**< Hypervisor-mode syscall */
                MACHINE_ECALL               = 0xb, /**< Machine-mode syscall */
                INST_PAGE_FAULT             = 0xc, /**< Instruction page fault */
                LOAD_PAGE_FAULT             = 0xd, /**< Load page fault */
                STORE_PAGE_FAULT            = 0xf, /**< Store page fault */

                // Interrupts/Asynchronous Exceptions
                INT_USER_SOFTWARE           = 0x0 | INTERRUPT_MASK, /**< User-mode software interrupt */
                INT_SUPERVISOR_SOFTWARE     = 0x1 | INTERRUPT_MASK, /**< Supervisor-mode software interrupt */
                INT_HYPERVISOR_SOFTWARE     = 0x2 | INTERRUPT_MASK, /**< Hypervisor-mode software interrupt */
                INT_MACHINE_SOFTWARE        = 0x3 | INTERRUPT_MASK, /**< Machine-mode software interrupt */
                INT_USER_TIMER              = 0x4 | INTERRUPT_MASK, /**< User-mode timer interrupt */
                INT_SUPERVISOR_TIMER        = 0x5 | INTERRUPT_MASK, /**< Supervisor-mode timer interrupt */
                INT_HYPERVISOR_TIMER        = 0x6 | INTERRUPT_MASK, /**< Hypervisor-mode timer interrupt */
                INT_MACHINE_TIMER           = 0x7 | INTERRUPT_MASK, /**< Machine-mode timer interrupt */
                INT_USER_EXT                = 0x8 | INTERRUPT_MASK, /**< User-mode external interrupt */
                INT_SUPERVISOR_EXT          = 0x9 | INTERRUPT_MASK, /**< Supervisor-mode external interrupt */
                INT_HYPERVISOR_EXT          = 0xa | INTERRUPT_MASK, /**< Hypervisor-mode external interrupt */
                INT_MACHINE_EXT             = 0xb | INTERRUPT_MASK, /**< Machine-mode external interrupt */
                INT_COPROCESSOR             = 0xc | INTERRUPT_MASK, /**< Coprocessor interrupt */
                INT_HOST                    = 0xd | INTERRUPT_MASK, /**< Host interrupt */

                // Special Events
                MODE_CHANGE                 = 0x0 | SPECIAL_MASK    /**< Instruction causes an execution mode change */
            };

        private:
            TYPE event_;
            SerializableVector<uint64_t, uint8_t> content_;

        public:
            EventRecord() = default;

            /**
             * Constructs an EventRecord
             * \param strm STFIFstream to read from
             */
            explicit EventRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an EventRecord
             * \param event event number
             * \param content_data Event content data
             */
            explicit EventRecord(const TYPE event, const std::vector<uint64_t>& content_data) :
                event_(event),
                content_(content_data)
            {
            }

            /**
             * Constructs an EventRecord
             * \param event event number
             * \param content_data Event content data
             */
            template<typename ... T>
            explicit EventRecord(const TYPE event, const T... content_data) :
                EventRecord(event, {content_data...})
            {
                static_assert(sizeof...(content_data) > 0,
                              "Must specify at least one content data element");
                static_assert(type_utils:: are_same<uint64_t, T...>::value,
                              "Variadic EventRecord constructor is only valid for uint64_t");
            }

            /**
             * Packs an EventRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, event_, content_);
            }

            /**
             * Unpacks an EventRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, event_, content_);
            }

            /**
             * Gets the event
             */
            inline TYPE getEvent() const {
                return event_;
            }

            /**
             * Gets whether the event is a mode change
             */
            inline bool isModeChange() const { return getEvent() == TYPE::MODE_CHANGE; }

            /**
             * Gets whether the event is an interrupt
             */
            inline bool isInterrupt() const { return static_cast<uint32_t>(event_) & INTERRUPT_MASK; }

            /**
             * Gets whether the event is a fault
             */
            inline bool isFault() const { return !isModeChange() && !isInterrupt() && !isSyscall(); }

            /**
             * Gets whether the event is a syscall
             */
            inline bool isSyscall() const { return getEvent() >= TYPE::USER_ECALL && getEvent() <= TYPE::MACHINE_ECALL; }

            /**
             * Gets the data associated with the event
             */
            const auto& getData() const {
                return content_;
            }

            /**
             * Formats an EventRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const;
    };

    /**
     * Prints an EventRecord::TYPE to an std::ostream
     * \param os std::ostream to print to
     * \param event_type Event type to print
     */
    std::ostream& operator<<(std::ostream& os, EventRecord::TYPE event_type);

    // Have to define this after declaring the EventRecord::TYPE ostream operator
    inline void EventRecord::format_impl(std::ostream& os) const {
        os << event_ << ' ' << content_;
    }

    /**
     * \class EventPCTargetRecord
     *
     * Defines new PC target of an event
     *
     */
    class EventPCTargetRecord : public GenericPCTargetRecord<EventPCTargetRecord> {
        public:
            EventPCTargetRecord() = default;

            /**
             * Constructs an EventPCTargetRecord
             * \param strm STFIFstream to read from
             */
            explicit EventPCTargetRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an EventPCTargetRecord
             * \param addr address
             */
            explicit EventPCTargetRecord(uint64_t addr) :
                GenericPCTargetRecord(addr)
            {
            }
    };

    /**
     * \class InstPCTargetRecord
     *
     * Defines new PC target of a taken branch
     *
     */
    class InstPCTargetRecord : public GenericPCTargetRecord<InstPCTargetRecord> {
        public:
            InstPCTargetRecord() = default;

            /**
             * Constructs an InstPCTargetRecord
             * \param strm STFIFstream to read from
             */
            explicit InstPCTargetRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstPCTargetRecord
             * \param addr address
             */
            explicit InstPCTargetRecord(uint64_t addr) :
                GenericPCTargetRecord(addr)
            {
            }
    };

    /**
     * \class InstRegRecord
     *
     * Defines operand register number and content
     * MSB of the register number indicates source (0) or destination (1)
     *
     */
    class InstRegRecord : public TypeAwareSTFRecord<InstRegRecord> {
        private:
            Registers::STF_REG reg_ = Registers::STF_REG::STF_REG_INVALID;         /**< operand register number */
            Registers::STF_REG_OPERAND_TYPE operand_type_ = Registers::STF_REG_OPERAND_TYPE::REG_RESERVED; /**< Whether this is a state, source, or dest */
            uint64_t data_;                                                        /**< operand register number */

        public:
            InstRegRecord() = default;

            /**
             * Constructs an InstRegRecord
             * \param strm STFIFstream to read from
             */
            explicit InstRegRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param operand_type register operand type
             * \param data register data
             */
            InstRegRecord(const Registers::STF_REG reg, const Registers::STF_REG_OPERAND_TYPE operand_type, const uint64_t data) :
                reg_(reg),
                operand_type_(operand_type),
                data_(data)
            {
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param reg_type register type
             * \param operand_type register operand type
             * \param data register data
             */
            InstRegRecord(const Registers::STF_REG_packed_int reg,
                          const Registers::STF_REG_TYPE reg_type,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          const uint64_t data) :
                InstRegRecord(static_cast<Registers::STF_REG>(Registers::Codec::combineRegType(reg, reg_type)), operand_type, data)
            {
            }

            /**
             * Gets the register number
             */
            Registers::STF_REG getReg() const { return reg_; }

            /**
             * Gets the register operand type
             */
            Registers::STF_REG_OPERAND_TYPE getOperandType() const { return operand_type_; }

            /**
             * Gets the register data
             */
            uint64_t getData() const { return data_; }

            /**
             * Sets the register data
             */
            void setData(uint64_t data) { data_ = data; }

            /**
             * Packs an InstRegRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer,
                       Registers::Codec::packRegNum(reg_),
                       Registers::Codec::packRegMetadata(reg_, operand_type_),
                       data_);
            }

            /**
             * Unpacks an InstRegRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                Registers::STF_REG_packed_int reg;
                Registers::STF_REG_metadata_int reg_metadata;
                read_(reader, reg, reg_metadata, data_);
                Registers::Codec::decode(reg, reg_metadata, reg_, operand_type_);
            }

            /**
             * Formats an InstRegRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << operand_type_ << ' ' << reg_ << ' ';
                format_utils::formatHex(os, data_);
            }
    };

    /**
     * \class InstMemContentRecord
     *
     * Defines memory data of load/store
     *
     */
    class InstMemContentRecord : public GenericSingleDataRecord<InstMemContentRecord, uint64_t> {
        public:
            InstMemContentRecord() = default;

            /**
             * Constructs an InstMemContentRecord
             * \param strm STFIFstream to read from
             */
            explicit InstMemContentRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstMemContentRecord
             * \param data data
             */
            explicit InstMemContentRecord(uint64_t data) :
                GenericSingleDataRecord<InstMemContentRecord, uint64_t>(data)
            {
            }

            /**
             * Gets the data
             */
            uint64_t getData() const { return GenericSingleDataRecord<InstMemContentRecord, uint64_t>::getData_(); }
    };

    /**
     * \class InstMemAccessRecord
     *
     * Defines memory access address, size, and attributes
     *
     */
    class InstMemAccessRecord : public TypeAwareSTFRecord<InstMemAccessRecord> {
        private:
            uint64_t address_ = 0;                              /**< memory access address */
            uint16_t size_ = 0;                                 /**< memory access size */
            uint16_t attr_ = 0;                                 /**< memory access attributes */
            INST_MEM_ACCESS type_ = INST_MEM_ACCESS::INVALID;   /**< memory access type */

        public:
            InstMemAccessRecord() = default;

            /**
             * Constructs an InstMemAccessRecord
             * \param strm STFIFstream to read from
             */
            explicit InstMemAccessRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstMemAccessRecord
             * \param address address
             * \param size size
             * \param attr attr
             * \param type type
             */
            InstMemAccessRecord(uint64_t address, uint16_t size, uint16_t attr, INST_MEM_ACCESS type) :
                address_(address),
                size_(size),
                attr_(attr),
                type_(type)
            {
            }

            /**
             * Reinitializes the record
             */
            void reset() {
                address_ = 0;
                size_ = 0;
                attr_ = 0;
                type_ = INST_MEM_ACCESS::INVALID;
            }

            /**
             * Gets the address
             */
            uint64_t getAddress() const { return address_; }

            /**
             * Gets the size
             */
            uint16_t getSize() const { return size_; }

            /**
             * Gets the attributes
             */
            uint16_t getAttr() const { return attr_; }

            /**
             * Gets the type
             */
            INST_MEM_ACCESS getType() const { return type_; }

            /**
             * Sets the address
             * \param address address to set
             */
            void setAddress(uint64_t address) { address_ = address; }

            /**
             * Sets the size
             * \param size size to set
             */
            void setSize(uint16_t size) { size_ = size; }

            /**
             * Sets the attributes
             * \param attr attributes to set
             */
            void setAttr(uint16_t attr) { attr_ = attr; }

            /**
             * Sets the type
             * \param type type to set
             */
            void setType(INST_MEM_ACCESS type) { type_ = type; }

            /**
             * Packs an InstMemAccessRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, address_, size_, attr_, type_);
            }

            /**
             * Unpacks an InstMemAccessRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, address_, size_, attr_, type_);
            }

            /**
             * Formats an InstMemAccessRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << type_ << ' ';
                format_utils::formatHex(os, address_);
                os << ' ';
                format_utils::formatHex(os, attr_);
                os << ' ' << size_;
            }
    };

    /**
     * \class InstOpcode32Record
     *
     * Defines 32-bit instruction opcode
     *
     */
    class InstOpcode32Record : public GenericOpcodeRecord<InstOpcode32Record, uint32_t> {
        public:
            InstOpcode32Record() = default;

            /**
             * Constructs an InstOpcode32Record
             * \param strm STFIFstream to read from
             */
            explicit InstOpcode32Record(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstOpcode32Record
             * \param opcode opcode
             */
            explicit InstOpcode32Record(uint32_t opcode) :
                GenericOpcodeRecord(opcode)
            {
            }
    };

    /**
     * \class InstOpcode16Record
     *
     * Defines 16-bit instruction opcode
     *
     */
    class InstOpcode16Record : public GenericOpcodeRecord<InstOpcode16Record, uint16_t> {
        public:
            InstOpcode16Record() = default;

            /**
             * Constructs an InstOpcode16Record
             * \param strm STFIFstream to read from
             */
            explicit InstOpcode16Record(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstOpcode16Record
             * \param opcode opcode
             */
            explicit InstOpcode16Record(uint16_t opcode) :
                GenericOpcodeRecord(opcode)
            {
            }
    };

    /**
     * \class InstMicroOpRecord
     *
     * Defines instruction micro-op
     *
     */
    class InstMicroOpRecord : public TypeAwareSTFRecord<InstMicroOpRecord> {
        private:
            uint8_t size_;               /**< size of the micro-op */
            uint32_t microop_;           /**< micro-op */

        public:
            InstMicroOpRecord() = default;

            /**
             * Constructs an InstMicroOpRecord
             * \param strm STFIFstream to read from
             */
            explicit InstMicroOpRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstMicroOpRecord
             * \param size size
             * \param microop microop
             */
            InstMicroOpRecord(uint8_t size, uint32_t microop) :
                size_(size),
                microop_(microop)
            {
            }

            /**
             * Gets the size
             */
            uint8_t getSize() const { return size_; }

            /**
             * Gets the micro-op
             */
            uint32_t getMicroOp() const { return microop_; }

            /**
             * Packs an InstMicroOpRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, size_, microop_);
            }

            /**
             * Unpacks an InstMicroOpRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, size_, microop_);
            }

            /**
             * Formats an InstMicroOpRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << size_ << ' ';
                format_utils::formatHex(os, microop_);
            }
    };

    /**
     * \class InstReadyRegRecord
     *
     * Mark destination register as ready
     *
     */
    class InstReadyRegRecord : public GenericSingleDataRecord<InstReadyRegRecord, uint16_t> {
        public:
            InstReadyRegRecord() = default;

            /**
             * Constructs an InstReadyRegRecord
             * \param strm STFIFstream to read from
             */
            explicit InstReadyRegRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an InstReadyRegRecord
             * \param reg register number
             */
            explicit InstReadyRegRecord(uint16_t reg) :
                GenericSingleDataRecord<InstReadyRegRecord, uint16_t>(reg)
            {
            }

            /**
             * Gets the register number
             */
            uint16_t getReg() const { return GenericSingleDataRecord<InstReadyRegRecord, uint16_t>::getData_(); }
    };

    /**
     * \class BusMasterAccessRecord
     *
     * Defines memory access attributions
     *
     */
    class BusMasterAccessRecord : public TypeAwareSTFRecord<BusMasterAccessRecord> {
        private:
            uint64_t address_;              /**< memory access address */
            uint16_t size_;                 /**< memory access data size */
            BUS_MASTER src_type_;           /**< memory access initiator type */
            uint8_t src_idx_;               /**< memory access initiator index */
            uint32_t attr_;                 /**< memory access attributes */
            BUS_MEM_ACCESS access_type_;    /**< memory access type */

        public:
            BusMasterAccessRecord() = default;

            /**
             * Constructs a BusMasterAccessRecord
             * \param strm STFIFstream to read data from
             */
            explicit BusMasterAccessRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a BusMasterAccessRecord
             * \param address address
             * \param size size
             * \param src_type initiator type
             * \param src_idx initiator index
             * \param attr attr
             * \param access_type access type
             */
            BusMasterAccessRecord(uint64_t address,
                                uint16_t size,
                                BUS_MASTER src_type,
                                uint8_t src_idx,
                                uint32_t attr,
                                BUS_MEM_ACCESS access_type) :
                address_(address),
                size_(size),
                src_type_(src_type),
                src_idx_(src_idx),
                attr_(attr),
                access_type_(access_type)
            {
            }

            /**
             * Packs a BusMasterAccessRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, address_, size_, src_type_, src_idx_, attr_, access_type_);
            }

            /**
             * Unpacks a BusMasterAccessRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, address_, size_, src_type_, src_idx_, attr_, access_type_);
            }

            /**
             * Formats a BusMasterAccessRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << access_type_ << ' ';
                format_utils::formatHex(os, address_);
                os << ' ' << src_type_ << src_idx_ << ' ' << size_ << ' ';
                format_utils::formatHex(os, attr_);
            }
    };

    /**
     * \class BusMasterContentRecord
     *
     * Defines memory data of bus master read/write
     *
     */
    class BusMasterContentRecord : public GenericSingleDataRecord<BusMasterContentRecord, uint64_t> {
        public:
            BusMasterContentRecord() = default;

            /**
             * Constructs a BusMasterContentRecord
             * \param strm STFIFstream to read data from
             */
            explicit BusMasterContentRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs an BusMasterContentRecord
             * \param data data
             */
            explicit BusMasterContentRecord(uint64_t data) :
                GenericSingleDataRecord<BusMasterContentRecord, uint64_t>(data)
            {
            }

            /**
             * Gets the data
             */
            uint64_t getData() const { return GenericSingleDataRecord<BusMasterContentRecord, uint64_t>::getData_(); }
    };

    /**
     * \class TraceInfoRecord
     *
     * Defines trace generator and features
     *
     */
    class TraceInfoRecord : public TypeAwareSTFRecord<TraceInfoRecord> {
        private:
            STF_GEN generator_ = STF_GEN::STF_GEN_RESERVED;          /**< The generator used to create the trace */
            uint8_t major_version_;                                  /**< The major version of the generator */
            uint8_t minor_version_;                                  /**< The minor version of the generator */
            uint8_t minor_minor_version_;                            /**< The minor minor version of the generator */
            SerializableString<uint16_t> comment_;                   /**< Additional comments */
            mutable std::string version_str_;                        /**< Cached version string */

            /**
             * \struct VersionReader
             * Reads a formatted version string
             */
            struct VersionReader : std::ctype<char> {
                VersionReader() :
                    std::ctype<char>(getTable())
                {
                }

                /**
                 * Returns the table of characters that should be treated as whitespace
                 */
                static inline std::ctype_base::mask const* getTable() {
                    static thread_local std::vector<std::ctype_base::mask> rc(table_size, std::ctype_base::mask());
                    rc['.'] = std::ctype_base::space;
                    rc[' '] = std::ctype_base::space;
                    rc['\n'] = std::ctype_base::space;
                    rc['\t'] = std::ctype_base::space;

                    return &rc[0];
                }
            };

        public:
            TraceInfoRecord() = default;

            /**
             * Constructs a TraceInfoRecord
             * \param strm STFIFstream to read from
             */
            explicit TraceInfoRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a TraceInfoRecord
             * \param generator generator
             * \param major_version major version
             * \param minor_version minor version
             * \param minor_minor_version minor minor version
             * \param comment comment
             */
            TraceInfoRecord(STF_GEN generator,
                            uint8_t major_version,
                            uint8_t minor_version,
                            uint8_t minor_minor_version,
                            const std::string& comment) :
                generator_(generator),
                major_version_(major_version),
                minor_version_(minor_version),
                minor_minor_version_(minor_minor_version),
                comment_(comment)
            {
            }

            /**
             * Gets the generator
             */
            STF_GEN getGenerator() const { return generator_; }

            /**
             * Returns a formatted version string
             */
            const std::string& getVersionString() const {
                if(STF_EXPECT_FALSE(version_str_.empty())) {
                    std::stringstream ss;
                    ss << static_cast<uint16_t>(major_version_)
                       << '.'
                       << static_cast<uint16_t>(minor_version_)
                       << '.'
                       << static_cast<uint16_t>(minor_minor_version_);
                    version_str_ = ss.str();
                }

                return version_str_;
            }

            /**
             * Gets the comment
             */
            const std::string& getComment() const {
                return comment_;
            }

            /**
             * Sets the generator
             * \param generator Generator value to set
             */
            void setGenerator(const STF_GEN generator) {
                generator_ = generator;
            }

            /**
             * Sets the version from a formatted version string
             * \param ver_str Version string to parse
             */
            void setVersion(const std::string_view ver_str) {
                version_str_.clear();

                std::stringstream ss(ver_str.data());
                ss.imbue(std::locale(std::locale(), new VersionReader()));

                if(!ss.eof()) {
                    ss >> major_version_;
                }
                else {
                    major_version_ = 0;
                }

                if(!ss.eof()) {
                    ss >> minor_version_;
                }
                else {
                    minor_version_ = 0;
                }

                if(!ss.eof()) {
                    ss >> minor_minor_version_;
                }
                else {
                    minor_minor_version_ = 0;
                }
            }

            /**
             * Sets the major version
             * \param ver version number to set
             */
            void setMajorVersion(const uint8_t ver) {
                version_str_.clear();
                major_version_ = ver;
            }

            /**
             * Gets the major version
             */
            uint8_t getMajorVersion() const {
                return major_version_;
            }

            /**
             * Sets the minor version
             * \param ver version number to set
             */
            void setMinorVersion(const uint8_t ver) {
                version_str_.clear();
                minor_version_ = ver;
            }

            /**
             * Gets the minor version
             */
            uint8_t getMinorVersion() const {
                return minor_version_;
            }

            /**
             * Sets the minor-minor version
             * \param ver version number to set
             */
            void setMinorMinorVersion(const uint8_t ver) {
                version_str_.clear();
                minor_minor_version_ = ver;
            }

            /**
             * Gets the minor-minor version
             */
            uint8_t getMinorMinorVersion() const {
                return minor_minor_version_;
            }

            /**
             * Sets the comment
             * \param comment Comment string
             */
            void setComment(const char* comment) {
                comment_ = comment;
            }

            /**
             * Sets the comment
             * \param comment Comment string
             */
            void setComment(const std::string_view comment) {
                comment_ = std::string(comment);
            }

            /**
             * Sets the comment
             * \param comment Comment string
             */
            void setComment(const std::string& comment) {
                comment_ = comment;
            }

            /**
             * Returns whether the version number has been set
             */
            bool isVersionSet() const {
                return (major_version_ != 0) || (minor_version_ != 0) || (minor_minor_version_ != 0);
            }

            /**
             * Packs a TraceInfoRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, generator_, major_version_, minor_version_, minor_minor_version_, comment_);
            }

            /**
             * Unpacks a TraceInfoRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                // Bundle the POD members together in a single read
                read_(reader, generator_, major_version_, minor_version_, minor_minor_version_);
                // Read the comment
                reader >> comment_;
            }

            /**
             * Formats a TraceInfoRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                os << generator_ << ' ' << getVersionString() << ' ' << comment_;
            }
    };


    /**
     * Writes a TraceInfoRecord to an ostream
     */
    std::ostream& operator<<(std::ostream& os, const TraceInfoRecord& rec);

    /**
     * \class TraceInfoFeatureRecord
     *
     * Defines which features are supported by this trace
     */
    class TraceInfoFeatureRecord : public GenericSingleDataRecord<TraceInfoFeatureRecord, uint64_t> {
        public:
            TraceInfoFeatureRecord() = default;

            /**
             * Constructs an TraceInfoFeatureRecord
             * \param strm STFIFstream to read from
             */
            explicit TraceInfoFeatureRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a TraceInfoFeatureRecord
             * \param features Features to enable
             */
            explicit TraceInfoFeatureRecord(TRACE_FEATURES features) :
                TraceInfoFeatureRecord(enums::to_int(features))
            {
            }

            /**
             * Constructs a TraceInfoFeatureRecord
             * \param features Features to enable
             */
            explicit TraceInfoFeatureRecord(uint64_t features) :
                GenericSingleDataRecord<TraceInfoFeatureRecord, uint64_t>(features)
            {
            }

            /**
             * Enables a feature
             * \param feature Feature to enable
             */
            void setFeature(TRACE_FEATURES feature) {
                setFeature(enums::to_int(feature));
            }

            /**
             * Enables a feature
             * \param feature Feature to enable
             */
            void setFeature(uint64_t feature) {
                GenericSingleDataRecord<TraceInfoFeatureRecord, uint64_t>::setData_(getFeatures() | feature);
            }

            /**
             * Gets all enabled features
             */
            uint64_t getFeatures() const { return GenericSingleDataRecord<TraceInfoFeatureRecord, uint64_t>::getData_(); }

            /**
             * Checks whether a feature is enabled
             * \param feature feature to check
             */
            bool hasFeature(TRACE_FEATURES feature) const {
                return getFeatures() & enums::to_int(feature);
            }
    };

} // end namespace stf

#endif
