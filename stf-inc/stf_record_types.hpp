#ifndef __STF_RECORD_TYPES_HPP__
#define __STF_RECORD_TYPES_HPP__

#include <algorithm>
#include <array>
#include <locale>
#include <memory>
#include <string>
#include <vector>
#include "boost_wrappers/small_vector.hpp"
#include "format_utils.hpp"
#include "stf_clock_id.hpp"
#include "stf_descriptor.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"
#include "stf_record_id_manager.hpp"
#include "stf_record_interfaces.hpp"
#include "stf_serializable_container.hpp"
#include "stf_enums.hpp"
#include "stf_generator.hpp"
#include "stf_reg_def.hpp"
#include "stf_vector_view.hpp"
#include "stf_vlen.hpp"
#include "util.hpp"
#include "stf_protocol_data.hpp"

namespace stf {
    /**
     * \class STFIdentifierRecord
     *
     * Indicates the beginning of an STF file
     *
     */
    class STFIdentifierRecord : public TypeAwareSTFRecord<STFIdentifierRecord, descriptors::internal::Descriptor::STF_IDENTIFIER> {
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

    REGISTER_RECORD(STFIdentifierRecord)

    /**
     * \class VersionRecord
     *
     * Defines STF version
     *
     */
    class VersionRecord : public TypeAwareSTFRecord<VersionRecord, descriptors::internal::Descriptor::STF_VERSION> {
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

    REGISTER_RECORD(VersionRecord)

    /**
     * \class CommentRecord
     *
     * Defines comment
     *
     */
    class CommentRecord : public TypeAwareSTFRecord<CommentRecord, descriptors::internal::Descriptor::STF_COMMENT> {
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

    REGISTER_RECORD(CommentRecord)

    /**
     * \class ISARecord
     *
     * Record used to hold an ISA family
     */
    class ISARecord : public GenericSingleDataRecord<ISARecord, ISA, descriptors::internal::Descriptor::STF_ISA> {
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

    REGISTER_RECORD(ISARecord)

    /**
     * \class InstIEMRecord
     *
     * Defines instruction encoding mode
     *
     */
    class InstIEMRecord : public GenericSingleDataRecord<InstIEMRecord, INST_IEM, descriptors::internal::Descriptor::STF_INST_IEM> {
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

    REGISTER_RECORD(InstIEMRecord)

    /**
     * \class ForcePCRecord
     *
     * Initializes PC
     *
     */
    class ForcePCRecord : public GenericPCTargetRecord<ForcePCRecord, descriptors::internal::Descriptor::STF_FORCE_PC> {
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

    REGISTER_RECORD(ForcePCRecord)

    /**
     * \class VLenConfigRecord
     *
     * Sets vlen parameter for a trace
     */
    class VLenConfigRecord : public GenericSingleDataRecord<VLenConfigRecord, vlen_t, descriptors::internal::Descriptor::STF_VLEN_CONFIG> {
        public:
            VLenConfigRecord() = default;

            /**
             * Constructs VLenConfigRecord
             * \param strm STFIFstream to read from
             */
            explicit VLenConfigRecord(STFIFstream& strm) {
                unpack_impl(strm);
                stf_assert(getVLen(), "Attempted to read an invalid VLenConfigRecord");
            }

            /**
             * Constructs a VLenConfigRecord
             * \param vlen vlen parameter value
             */
            explicit VLenConfigRecord(vlen_t vlen) :
                GenericSingleDataRecord(vlen)
            {
                stf_assert(getVLen(), "Attempted to create an invalid VLenConfigRecord");
            }

            /**
             * Packs the record into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                writer.setVLen(getVLen());
                GenericSingleDataRecord::pack_impl(writer);
            }

            /**
             * Unpacks the record from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                GenericSingleDataRecord::unpack_impl(reader);
                reader.setVLen(getVLen());
            }

            /**
             * Gets the vlen
             */
            vlen_t getVLen() const { return GenericSingleDataRecord::getData_(); }
    };

    REGISTER_RECORD(VLenConfigRecord)

    /**
     * \class ProtocolIdRecord
     *
     * Indicates what protocol is used in a transaction trace
     */
    class ProtocolIdRecord : public GenericSingleDataRecord<ProtocolIdRecord, protocols::ProtocolId, descriptors::internal::Descriptor::STF_PROTOCOL_ID> {
        public:
            ProtocolIdRecord() = default;

            /**
             * Constructs ProtocolIdRecord
             * \param strm STFIFstream to read from
             */
            explicit ProtocolIdRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a ProtocolIdRecord
             * \param protocol_id protocol ID value
             */
            explicit ProtocolIdRecord(const protocols::ProtocolId protocol_id) :
                GenericSingleDataRecord(protocol_id)
            {
            }

            /**
             * Packs the record into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                writer.setProtocolId(getId());
                GenericSingleDataRecord::pack_impl(writer);
            }

            /**
             * Unpacks the record from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                GenericSingleDataRecord::unpack_impl(reader);
                reader.setProtocolId(getId());
            }

            /**
             * Gets the protocol id
             */
            protocols::ProtocolId getId() const { return GenericSingleDataRecord::getData_(); }
    };

    REGISTER_RECORD(ProtocolIdRecord)

    /**
     * \class ClockIdRecord
     *
     * Indicates what protocol is used in a transaction trace
     */
    class ClockIdRecord : public TypeAwareSTFRecord<ClockIdRecord, descriptors::internal::Descriptor::STF_CLOCK_ID> {
        private:
            ClockId clock_id_;
            SerializableString<uint16_t> name_;

        public:
            ClockIdRecord() = default;

            /**
             * Constructs ClockIdRecord
             * \param strm STFIFstream to read from
             */
            explicit ClockIdRecord(STFIFstream& strm) {
                unpack_impl(strm);
            }

            /**
             * Constructs a ClockIdRecord
             * \param clock_id clock ID value
             * \param name clock name
             */
            explicit ClockIdRecord(const ClockId clock_id, const std::string& name) :
                clock_id_(clock_id),
                name_(name)
            {
            }

            /**
             * Constructs a ClockIdRecord
             * \param clock_id clock ID value
             * \param name clock name
             */
            explicit ClockIdRecord(const ClockId clock_id, std::string&& name) :
                clock_id_(clock_id),
                name_(std::move(name))
            {
            }

            /**
             * Constructs a ClockIdRecord
             * \param clock_id clock ID value
             * \param name clock name
             */
            explicit ClockIdRecord(const ClockId clock_id, const std::string_view name) :
                clock_id_(clock_id),
                name_(std::string(name))
            {
            }

            /**
             * Packs a ClockIdRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer, clock_id_, name_);
            }

            /**
             * Unpacks a ClockIdRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader, clock_id_, name_);
            }

            /**
             * Formats a ClockIdRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                format_utils::formatLabel(os, name_);
                format_utils::formatDec(os, clock_id_);
            }

            /**
             * Gets the clock id
             */
            ClockId getId() const {
                return clock_id_;
            }

            /**
             * Gets the clock name
             */
            const std::string& getName() const {
                return name_;
            }
    };

    REGISTER_RECORD(ClockIdRecord)

    /**
     * \class EndOfHeaderRecord
     *
     * Marks end of STF header
     *
     */
    class EndOfHeaderRecord : public GenericEmptyRecord<EndOfHeaderRecord, descriptors::internal::Descriptor::STF_END_HEADER> {
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

    REGISTER_RECORD(EndOfHeaderRecord)

    /**
     * \class PageTableWalkRecord
     *
     * Contains all page table entries necessary to perform a table walk for VA -> PA translation
     */
    class PageTableWalkRecord : public TypeAwareSTFRecord<PageTableWalkRecord, descriptors::internal::Descriptor::STF_PAGE_TABLE_WALK> {
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

    REGISTER_RECORD(PageTableWalkRecord)

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
    class ProcessIDExtRecord : public TypeAwareSTFRecord<ProcessIDExtRecord, descriptors::internal::Descriptor::STF_PROCESS_ID_EXT> {
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

    REGISTER_RECORD(ProcessIDExtRecord)

    /**
     * \class EventRecord
     *
     * Event number
     *
     */
    class EventRecord : public TypeAwareSTFRecord<EventRecord, descriptors::internal::Descriptor::STF_EVENT> {
        public:
            static constexpr uint64_t INTERRUPT_MASK = (1ULL << 63); /**< Interrupt mask */
            static constexpr uint64_t SPECIAL_MASK = (1ULL << 62); /**< Special event mask */

            static constexpr uint32_t EVENT32_INTERRUPT_MASK = {1U << 31}; /**< 32-bit event interrupt mask */
            static constexpr uint32_t EVENT32_SPECIAL_MASK = {1U << 30}; /**< 32-bit special event mask */
            /**
             * \enum TYPE
             *
             * Defines the different event types
             */
            enum class TYPE : uint64_t {
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
                GUEST_INST_PAGE_FAULT       = 0x14, /**< Instruction guest-page fault */
                GUEST_LOAD_PAGE_FAULT       = 0x15, /**< Load guest-page fault */
                VIRTUAL_INST                = 0x16, /**< Virtual instruction */
                GUEST_STORE_PAGE_FAULT      = 0x17, /**< Store/AMO guest-page fault */

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

            static constexpr uint32_t EVENT32_TOP_BITS_ = EVENT32_INTERRUPT_MASK | EVENT32_SPECIAL_MASK;
            static constexpr uint64_t EVENT64_TOP_BITS_ = INTERRUPT_MASK | SPECIAL_MASK;
            static constexpr uint64_t EVENT64_ZERO_BITS_ = byte_utils::bitMask<uint64_t, 58>() << 4;
            static constexpr uint32_t EVENT32_ZERO_BITS_ = byte_utils::bitMask<uint32_t, 26>() << 4;
            static constexpr uint64_t EVENT64_TOP_ZERO_BITS_ = EVENT64_ZERO_BITS_ & ~EVENT32_ZERO_BITS_;

            inline uint32_t convertTo32BitEvent_() const {
                return static_cast<uint32_t>(event_) | static_cast<uint32_t>((static_cast<uint64_t>(event_) & EVENT64_TOP_BITS_) >> 32);
            }

            static inline TYPE convertFrom32BitEvent_(const uint32_t event32) {
                return static_cast<TYPE>((event32 & ~EVENT32_TOP_BITS_) |
                                         (static_cast<uint64_t>(event32 & EVENT32_TOP_BITS_) << 32));
            }

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
                EventRecord(event, SerializableVector<uint64_t, uint8_t>(content_data))
            {
            }

            /**
             * Constructs an EventRecord
             * \param event event number
             * \param content_data Event content data
             */
            explicit EventRecord(const TYPE event, const SerializableVector<uint64_t, uint8_t>& content_data) :
                event_(event),
                content_(content_data)
            {
                if(const auto event_u64 = static_cast<uint64_t>(event_); STF_EXPECT_FALSE(event_u64 & EVENT32_TOP_BITS_)) {
                    stf_assert(!(event_u64 & EVENT64_TOP_ZERO_BITS_),
                               "Invalid event type specified: " << std::hex << event_u64);
                    event_ = convertFrom32BitEvent_(static_cast<uint32_t>(event_));
                }
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
                static_assert(type_utils::are_same_v<uint64_t, T...>,
                              "Variadic EventRecord constructor is only valid for uint64_t");
            }

            /**
             * Packs an EventRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                if(writer.has32BitEvents()) {
                    write_(writer, convertTo32BitEvent_(), content_);
                }
                else {
                    write_(writer, event_, content_);
                }
            }

            /**
             * Unpacks an EventRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                if(reader.has32BitEvents()) {
                    uint32_t event32;
                    read_(reader, event32, content_);
                    event_ = convertFrom32BitEvent_(event32);
                }
                else {
                    read_(reader, event_, content_);
                }
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
            inline bool isInterrupt() const { return static_cast<uint64_t>(event_) & INTERRUPT_MASK; }

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

    REGISTER_RECORD(EventRecord)

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
    class EventPCTargetRecord : public GenericPCTargetRecord<EventPCTargetRecord, descriptors::internal::Descriptor::STF_EVENT_PC_TARGET> {
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

    REGISTER_RECORD(EventPCTargetRecord)

    /**
     * \class InstPCTargetRecord
     *
     * Defines new PC target of a taken branch
     *
     */
    class InstPCTargetRecord : public GenericPCTargetRecord<InstPCTargetRecord, descriptors::internal::Descriptor::STF_INST_PC_TARGET> {
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

    REGISTER_RECORD(InstPCTargetRecord)

    /**
     * \class InstRegRecord
     *
     * Defines operand register number and content
     * MSB of the register number indicates source (0) or destination (1)
     *
     */
    class InstRegRecord : public TypeAwareSTFRecord<InstRegRecord, descriptors::internal::Descriptor::STF_INST_REG> {
        public:
            /**
             * \typedef ValueType
             * Underlying data type used in the vector
             */
            using ValueType = uint64_t;

            /**
             * \typedef VectorType
             * Vector type used to store register data
             */
            using VectorType = boost::container::small_vector<ValueType, 1>;

        private:
            Registers::STF_REG reg_ = Registers::STF_REG::STF_REG_INVALID;         /**< operand register number */
            Registers::STF_REG_OPERAND_TYPE operand_type_ = Registers::STF_REG_OPERAND_TYPE::REG_RESERVED; /**< Whether this is a state, source, or dest */
            VectorType data_ = VectorType(1); /**< Register record data */
            mutable vlen_t vlen_ = 0;

            inline size_t calcVectorLen_() const {
                return calcVectorLen(vlen_);
            }

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
             * \param data vector register data
             */
            InstRegRecord(const Registers::STF_REG reg,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          VectorType&& data) :
                reg_(reg),
                operand_type_(operand_type),
                data_(std::move(data))
            {
                const auto data_size = data_.size();
                stf_assert(data_size, "Attempted to construct a register record without any data");
                stf_assert(isVector() || data_size == 1,
                           "Attempted to construct a scalar register record with vector register data");
            }

            /**
             * Copies an InstRegRecord, changing the operand type
             * \param rec record to copy
             * \param operand_type register operand type
             */
            InstRegRecord(const InstRegRecord& rec,
                          const Registers::STF_REG_OPERAND_TYPE operand_type) :
                InstRegRecord(rec.reg_, operand_type, rec.data_)
            {
                vlen_ = rec.vlen_;
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param operand_type register operand type
             * \param data vector register data
             */
            InstRegRecord(const Registers::STF_REG reg,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          const VectorType& data) :
                reg_(reg),
                operand_type_(operand_type),
                data_(data)
            {
                const auto data_size = data_.size();
                stf_assert(data_size, "Attempted to construct a register record without any data");
                stf_assert(isVector() || data_size == 1,
                           "Attempted to construct a scalar register record with vector register data");
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param operand_type register operand type
             * \param data register data
             */
            InstRegRecord(const Registers::STF_REG reg,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          const ValueType data) :
                InstRegRecord(reg, operand_type, VectorType(1, data))
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
                          const ValueType data) :
                InstRegRecord(reg,
                              reg_type,
                              operand_type,
                              VectorType(1, data))
            {
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param reg_type register type
             * \param operand_type register operand type
             * \param data vector register data
             */
            InstRegRecord(const Registers::STF_REG_packed_int reg,
                          const Registers::STF_REG_TYPE reg_type,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          VectorType&& data) :
                InstRegRecord(static_cast<Registers::STF_REG>(Registers::Codec::combineRegType(reg, reg_type)),
                              operand_type,
                              std::move(data))
            {
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param reg_type register type
             * \param operand_type register operand type
             * \param data vector register data
             */
            InstRegRecord(const Registers::STF_REG_packed_int reg,
                          const Registers::STF_REG_TYPE reg_type,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          const VectorType& data) :
                InstRegRecord(static_cast<Registers::STF_REG>(Registers::Codec::combineRegType(reg, reg_type)),
                              operand_type,
                              data)
            {
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param reg_type register type
             * \param operand_type register operand type
             * \param data vector register data
             */
            InstRegRecord(const Registers::STF_REG_packed_int reg,
                          const Registers::STF_REG_TYPE reg_type,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          const std::vector<ValueType>& data) :
                InstRegRecord(reg,
                              reg_type,
                              operand_type,
                              VectorType(data.begin(), data.end()))
            {
            }

            /**
             * Constructs an InstRegRecord
             * \param reg register number
             * \param reg_type register type
             * \param operand_type register operand type
             * \param data vector register data
             */
            InstRegRecord(const Registers::STF_REG_packed_int reg,
                          const Registers::STF_REG_TYPE reg_type,
                          const Registers::STF_REG_OPERAND_TYPE operand_type,
                          std::initializer_list<ValueType> data) :
                InstRegRecord(reg,
                              reg_type,
                              operand_type,
                              VectorType(data))
            {
            }

            /**
             * Calculates the vector length needed to represent a given vlen
             * \param vlen Vlen to use
             */
            template<typename VlenType>
            static inline size_t calcVectorLen(const VlenType vlen) {
                constexpr auto VECTOR_DATA_SIZE = byte_utils::bitSize<decltype(data_)::value_type>();
                constexpr auto SHIFT_AMT = math_utils::constexpr_log::log2(VECTOR_DATA_SIZE);
                return (static_cast<size_t>(vlen) + VECTOR_DATA_SIZE - 1) >> SHIFT_AMT;
            }

            /**
             * Gets the register number
             */
            inline Registers::STF_REG getReg() const { return reg_; }

            /**
             * Gets the register operand type
             */
            inline Registers::STF_REG_OPERAND_TYPE getOperandType() const { return operand_type_; }

            /**
             * Gets the scalar register data
             */
            inline ValueType getScalarData() const {
                stf_assert(!isVector(), "Attempted to get scalar data from a vector register");
                stf_assert(data_.size() == 1, "Invalid data size for scalar register");
                return data_.front();
            }

            /**
             * Sets the register data
             * \param data new value to set
             */
            inline void setScalarData(const ValueType data) {
                stf_assert(!isVector(), "Attempted to set scalar data on a vector register");
                stf_assert(data_.size() == 1, "Invalid data size for scalar register");
                data_.front() = data;
            }

            /**
             * Gets the vector register data
             */
            inline const VectorType& getVectorData() const {
                stf_assert(isVector(), "Attempted to get vector data from a non-vector register");
                return data_;
            }

            /**
             * Sets the register data
             * \param data new value to set
             */
            inline void setVectorData(const VectorType& data) {
                stf_assert(isVector(), "Attempted to set vector data on a scalar register");
                stf_assert(data_.size() == data.size(), "Invalid data size for vector register");
                data_ = data;
            }

            /**
             * Sets the register data
             * \param data new value to set
             */
            inline void setVectorData(const std::vector<ValueType>& data) {
                stf_assert(isVector(), "Attempted to set vector data on a scalar register");
                stf_assert(data_.size() == data.size(), "Invalid data size for vector register");
                std::copy(data.begin(), data.end(), data_.begin());
            }

            /**
             * Copies data from another InstRegRecord. Also updates vlen_ field, but does not change operand_type_;
             * \param rhs InstRegRecord to copy from
             */
            inline void copyFrom(const InstRegRecord& rhs) {
                stf_assert(reg_ == rhs.reg_,
                           "Attempted to copy from register " << rhs.reg_ << " into register " << reg_);
                data_ = rhs.data_;
                vlen_ = rhs.vlen_;
            }

            /**
             * Packs an InstRegRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                // Pack everything including the first data record
                write_(writer,
                       Registers::Codec::packRegNum(reg_),
                       Registers::Codec::packRegMetadata(reg_, operand_type_),
                       data_.front());

                // If it's a vector record, pack the rest of the data
                if(STF_EXPECT_FALSE(isVector())) {
                    vlen_ = writer.getVLen();
                    stf_assert(vlen_, "Attempted to read vector register without setting vlen first");
                    const auto vector_len = calcVectorLen_();
                    stf_assert(vector_len == data_.size(),
                               "Vector register record length ("
                               << data_.size()
                               << ") does not match length required by vlen parameter ("
                               << vector_len
                               << ")");
                    write_(writer, ConstVectorView(data_, 1));
                }
            }

            /**
             * Unpacks an InstRegRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                // Unpack assuming it's a scalar register record
                Registers::STF_REG_packed_int reg;
                Registers::STF_REG_metadata_int reg_metadata;
                read_(reader, reg, reg_metadata, data_.front());
                Registers::Codec::decode(reg, reg_metadata, reg_, operand_type_);

                // If it's actually a vector record, unpack the rest of the vector data
                if(STF_EXPECT_FALSE(isVector())) {
                    vlen_ = reader.getVLen();
                    stf_assert(vlen_, "Attempted to read vector register without setting vlen first");
                    data_.resize(calcVectorLen_());
                    read_(reader, VectorView(data_, 1));
                }
                else {
                    data_.resize(1); // always resize the vector in case this record object is being reused
                }
            }

            /**
             * Formats an InstRegRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                if(STF_EXPECT_FALSE(isVector())) {
                    std::ostringstream ss;
                    ss << operand_type_ << ' ' << reg_ << ' ';
                    const size_t padding = static_cast<size_t>(ss.tellp());
                    os << ss.str();
                    format_utils::formatHex(os, data_.front());

                    for(auto it = std::next(data_.begin()); it != data_.end(); ++it) {
                        os << std::endl;
                        format_utils::formatSpaces(os, padding);
                        format_utils::formatHex(os, *it);
                    }
                }
                else {
                    os << operand_type_ << ' ' << reg_ << ' ';
                    format_utils::formatHex(os, data_.front());
                }
            }

            /**
             * Gets whether this record is a vector register
             */
            inline bool isVector() const {
                return stf::Registers::isVector(reg_);
            }

            /**
             * Gets whether this record is an FP register
             */
            inline bool isFP() const {
                return stf::Registers::isFPR(reg_);
            }

            /**
             * Gets whether this record is an integer register
             */
            inline bool isInt() const {
                return stf::Registers::isGPR(reg_);
            }

            /**
             * Gets the vlen parameter used when this record was read or written
             */
            inline vlen_t getVLen() const {
                return vlen_;
            }

            /**
             * Sets the vlen parameter
             */
            inline void setVLen(const vlen_t vlen) const {
                stf_assert(isVector(), "VLen should only be set on vector register records");
                vlen_ = vlen;
                stf_assert(vlen_, "VLen cannot be 0");
                const auto expected_vector_len = calcVectorLen_();
                stf_assert(data_.size() == expected_vector_len, "Vector data length mismatch: got " << data_.size() << ", expected " << expected_vector_len);
            }
    };

    REGISTER_RECORD(InstRegRecord)

    /**
     * \class InstMemContentRecord
     *
     * Defines memory data of load/store
     *
     */
    class InstMemContentRecord : public GenericSingleDataRecord<InstMemContentRecord, uint64_t, descriptors::internal::Descriptor::STF_INST_MEM_CONTENT> {
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
                GenericSingleDataRecord(data)
            {
            }

            /**
             * Gets the data
             */
            uint64_t getData() const { return GenericSingleDataRecord::getData_(); }
    };

    REGISTER_RECORD(InstMemContentRecord)

    /**
     * \class InstMemAccessRecord
     *
     * Defines memory access address, size, and attributes
     *
     */
    class InstMemAccessRecord : public TypeAwareSTFRecord<InstMemAccessRecord, descriptors::internal::Descriptor::STF_INST_MEM_ACCESS> {
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

    REGISTER_RECORD(InstMemAccessRecord)

    /**
     * \class InstOpcode32Record
     *
     * Defines 32-bit instruction opcode
     *
     */
    class InstOpcode32Record : public GenericOpcodeRecord<InstOpcode32Record, uint32_t, descriptors::internal::Descriptor::STF_INST_OPCODE32> {
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

    REGISTER_RECORD(InstOpcode32Record)

    /**
     * \class InstOpcode16Record
     *
     * Defines 16-bit instruction opcode
     *
     */
    class InstOpcode16Record : public GenericOpcodeRecord<InstOpcode16Record, uint16_t, descriptors::internal::Descriptor::STF_INST_OPCODE16> {
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

    REGISTER_RECORD(InstOpcode16Record)

    /**
     * \class InstMicroOpRecord
     *
     * Defines instruction micro-op
     *
     */
    class InstMicroOpRecord : public TypeAwareSTFRecord<InstMicroOpRecord, descriptors::internal::Descriptor::STF_INST_MICROOP> {
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

    REGISTER_RECORD(InstMicroOpRecord)

    /**
     * \class InstReadyRegRecord
     *
     * Mark destination register as ready
     *
     */
    class InstReadyRegRecord : public GenericSingleDataRecord<InstReadyRegRecord, uint16_t, descriptors::internal::Descriptor::STF_INST_READY_REG> {
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
                GenericSingleDataRecord(reg)
            {
            }

            /**
             * Gets the register number
             */
            uint16_t getReg() const { return GenericSingleDataRecord::getData_(); }
    };

    REGISTER_RECORD(InstReadyRegRecord)

    /**
     * \class BusMasterAccessRecord
     *
     * Defines memory access attributions
     *
     */
    class BusMasterAccessRecord : public TypeAwareSTFRecord<BusMasterAccessRecord, descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS> {
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

    REGISTER_RECORD(BusMasterAccessRecord)

    /**
     * \class BusMasterContentRecord
     *
     * Defines memory data of bus master read/write
     *
     */
    class BusMasterContentRecord : public GenericSingleDataRecord<BusMasterContentRecord, uint64_t, descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT> {
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
                GenericSingleDataRecord(data)
            {
            }

            /**
             * Gets the data
             */
            uint64_t getData() const { return GenericSingleDataRecord::getData_(); }
    };

    REGISTER_RECORD(BusMasterContentRecord)

    /**
     * \class TraceInfoRecord
     *
     * Defines trace generator and features
     *
     */
    class TraceInfoRecord : public TypeAwareSTFRecord<TraceInfoRecord, descriptors::internal::Descriptor::STF_TRACE_INFO> {
        private:
            STF_GEN generator_ = STF_GEN::STF_GEN_RESERVED;          /**< The generator used to create the trace */
            uint8_t major_version_ = 0;                                  /**< The major version of the generator */
            uint8_t minor_version_ = 0;                                  /**< The minor version of the generator */
            uint8_t minor_minor_version_ = 0;                            /**< The minor minor version of the generator */
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
            TraceInfoRecord(const STF_GEN generator,
                            const uint8_t major_version,
                            const uint8_t minor_version,
                            const uint8_t minor_minor_version,
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
            STF_GEN getGenerator() const {
                return generator_;
            }

            /**
             * Checks if the trace was generated with a particular generator
             */
            bool isGenerator(const STF_GEN generator) const {
                return generator_ == generator;
            }

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

    REGISTER_RECORD(TraceInfoRecord)

    /**
     * Writes a TraceInfoRecord to an ostream
     */
    std::ostream& operator<<(std::ostream& os, const TraceInfoRecord& rec);

    /**
     * \class TraceInfoFeatureRecord
     *
     * Defines which features are supported by this trace
     */
    class TraceInfoFeatureRecord : public GenericSingleDataRecord<TraceInfoFeatureRecord, uint64_t, descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE> {
        private:
            void handleStreamFlags_(STFFstream& strm) const {
                // If the trace doesn't support 64 bit events, tell the stream to pack/unpack from 32 bits
                strm.set32BitEvents(!hasFeature(TRACE_FEATURES::STF_CONTAIN_EVENT64));
            }

        public:
            /**
             * \typedef feature_int_type
             *
             * Underlying integer type used by TraceInfoFeatureRecord
             */
            using feature_int_type = enums::int_t<TRACE_FEATURES>;

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
            explicit TraceInfoFeatureRecord(const TRACE_FEATURES features) :
                TraceInfoFeatureRecord(enums::to_int(features))
            {
            }

            /**
             * Constructs a TraceInfoFeatureRecord
             * \param features Features to enable
             */
            explicit TraceInfoFeatureRecord(const feature_int_type features) :
                GenericSingleDataRecord(features)
            {
            }

            /**
             * Packs a TraceInfoFeatureRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                handleStreamFlags_(writer);
                GenericSingleDataRecord::pack_impl(writer);
            }

            /**
             * Unpacks a TraceInfoFeatureRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                GenericSingleDataRecord::unpack_impl(reader);
                handleStreamFlags_(reader);
            }

            /**
             * Enables a feature
             * \param feature Feature to enable
             */
            void setFeature(const TRACE_FEATURES feature) {
                setFeature(enums::to_int(feature));
            }

            /**
             * Enables a feature
             * \param feature Feature to enable
             */
            void setFeature(const feature_int_type feature) {
                GenericSingleDataRecord::setData_(getFeatures() | feature);
            }

            /**
             * Disables a feature
             * \param feature Feature to disable
             */
            void disableFeature(const TRACE_FEATURES feature) {
                disableFeature(enums::to_int(feature));
            }

            /**
             * Disables a feature
             * \param feature Feature to disable
             */
            void disableFeature(const feature_int_type feature) {
                GenericSingleDataRecord::setData_(getFeatures() & ~feature);
            }

            /**
             * Gets all enabled features
             */
            feature_int_type getFeatures() const {
                return GenericSingleDataRecord::getData_();
            }

            /**
             * Checks whether a feature is enabled
             * \param feature feature to check
             */
            bool hasFeature(const TRACE_FEATURES feature) const {
                return getFeatures() & enums::to_int(feature);
            }
    };

    REGISTER_RECORD(TraceInfoFeatureRecord)

    /**
     * Writes a TraceInfoFeatureRecord to an ostream
     */
    std::ostream& operator<<(std::ostream& os, const TraceInfoFeatureRecord& rec);

    /**
     * \class TransactionRecord
     *
     * Represents a timestamped bus transaction in the trace
     *
     */
    class TransactionRecord : public TypeAwareSTFRecord<TransactionRecord, descriptors::internal::Descriptor::STF_TRANSACTION> {
        private:
            uint64_t transaction_id_ = 0;
            uint64_t cycle_delta_ = 0;
            ClockId clock_id_ = INVALID_CLOCK_ID;
            protocols::ProtocolData::UniqueHandle protocol_data_;

            /**
             * Constructs a TransactionRecord
             * \param transaction_id Transaction ID
             * \param cycle_delta Cycle delta from previous transaction
             * \param clock_id Clock domain ID
             * \param protocol_data Protocol data object
             */
            explicit TransactionRecord(const uint64_t transaction_id,
                                       const uint64_t cycle_delta,
                                       const ClockId clock_id,
                                       protocols::ProtocolData::UniqueHandle&& protocol_data) :
                transaction_id_(transaction_id),
                cycle_delta_(cycle_delta),
                clock_id_(clock_id),
                protocol_data_(std::move(protocol_data))
            {
            }

        public:
            TransactionRecord() = default;

            /**
             * Unpacks a TransactionRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            explicit TransactionRecord(STFIFstream& reader) {
                unpack_impl(reader);
            }

            /**
             * Creates a new TransactionRecord. Automatically gets the ID from an RecordIdManager.
             * \param id_manager RecordIdManager that tracks the ID values for the current transaction stream
             * \param clock_id Clock domain ID for this transaction
             * \param cycle_delta Cycle delta between the previous transaction and this one
             * \param protocol_data Protocol data to populate this transaction
             */
            TransactionRecord(RecordIdManager& id_manager,
                              const ClockId clock_id,
                              const uint64_t cycle_delta,
                              protocols::ProtocolData::UniqueHandle&& protocol_data) :
                TransactionRecord(id_manager.getNextId(), cycle_delta, clock_id, std::move(protocol_data))
            {
            }

            /**
             * Creates a new TransactionRecord in the default clock domain. Automatically gets the ID from an RecordIdManager.
             * \param id_manager RecordIdManager that tracks the ID values for the current transaction stream
             * \param cycle_delta Cycle delta between the previous transaction and this one
             * \param protocol_data Protocol data to populate this transaction
             */
            TransactionRecord(RecordIdManager& id_manager,
                              const uint64_t cycle_delta,
                              protocols::ProtocolData::UniqueHandle&& protocol_data) :
                TransactionRecord(id_manager, ClockRegistry::getDefaultClock(), cycle_delta, std::move(protocol_data))
            {
            }

            /**
             * Creates a new TransactionRecord. Automatically gets the ID from an RecordIdManager.
             * \param id_manager RecordIdManager that tracks the ID values for the current transaction stream
             * \param protocol_data Protocol data to populate this transaction
             */
            TransactionRecord(RecordIdManager& id_manager,
                              protocols::ProtocolData::UniqueHandle&& protocol_data) :
                TransactionRecord(id_manager, 0, std::move(protocol_data))
            {
            }

            /**
             * TransactionRecord copy constructor
             * \param rhs Record to copy
             */
            TransactionRecord(const TransactionRecord& rhs) :
                transaction_id_(rhs.transaction_id_),
                cycle_delta_(rhs.cycle_delta_),
                clock_id_(rhs.clock_id_),
                protocol_data_(rhs.protocol_data_->clone())
            {
            }

            /**
             * TransactionRecord move constructor
             */
            TransactionRecord(TransactionRecord&&) = default;

            /**
             * Packs a TransactionRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer,
                       transaction_id_,
                       cycle_delta_,
                       clock_id_);
                protocol_data_->pack(writer);
                writer.markerRecordCallback();
            }

            /**
             * Unpacks a TransactionRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader,
                      transaction_id_,
                      cycle_delta_,
                      clock_id_);
                reader >> protocol_data_;
                reader.markerRecordCallback();
            }

            /**
             * Formats a TransactionRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                format_utils::formatLabel(os, "ID");
                format_utils::formatDec(os, transaction_id_);
                os << std::endl;
                format_utils::formatLabel(os, "CLOCK");
                os << ClockRegistry::getClockName(clock_id_) << std::endl;
                format_utils::formatLabel(os, "DELTA");
                format_utils::formatDec(os, cycle_delta_);
                os << std::endl;
                protocol_data_->format(os);
            }

            /**
             * Sets the protocol data
             * \param protocol_data Protocol data to attach to this transaction
             */
            inline void setProtocolData(protocols::ProtocolData::UniqueHandle&& protocol_data) {
                protocol_data_ = std::move(protocol_data);
            }

            /**
             * Gets the transaction ID
             */
            inline uint64_t getTransactionId() const {
                return transaction_id_;
            }

            /**
             * Gets the cycle delta
             */
            inline uint64_t getCycleDelta() const {
                return cycle_delta_;
            }

            /**
             * Gets the clock domain ID
             */
            inline ClockId getClockId() const {
                return clock_id_;
            }

            /**
             * Gets the protocol data
             */
            inline const auto& getProtocolData() const {
                return protocol_data_;
            }

            /**
             * Gets the protocol data, cast to a specific protocol type
             */
            template<typename T>
            inline const auto& getProtocolAs() const {
                return protocol_data_->as<T>();
            }
    };

    REGISTER_RECORD(TransactionRecord)

    /**
     * \class TransactionDependencyRecord
     *
     * Represents a timestamped bus transaction in the trace
     *
     */
    class TransactionDependencyRecord : public TypeAwareSTFRecord<TransactionDependencyRecord, descriptors::internal::Descriptor::STF_TRANSACTION_DEPENDENCY> {
        private:
            uint64_t dependency_id_ = 0;
            uint64_t cycle_delta_ = 0;
            ClockId clock_id_ = INVALID_CLOCK_ID;

        public:
            TransactionDependencyRecord() = default;

            /**
             * Unpacks a TransactionDependencyRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            explicit TransactionDependencyRecord(STFIFstream& reader) {
                unpack_impl(reader);
            }

            /**
             * Unpacks a TransactionDependencyRecord from an STFIFstream
             * \param dependency_id Transaction dependency ID
             * \param cycle_delta Cycle delta between this dependency and the previous transaction
             * \param clock_id Clock domain ID for this dependency
             */
            explicit TransactionDependencyRecord(const uint64_t dependency_id,
                                                 const uint64_t cycle_delta = 0,
                                                 const ClockId clock_id = ClockRegistry::getDefaultClock()) :
                dependency_id_(dependency_id),
                cycle_delta_(cycle_delta),
                clock_id_(clock_id)
            {
            }

            /**
             * Constructs a TransactionDependencyRecord
             * \param transaction_record TransactionRecord that provides the dependency ID
             * \param cycle_delta Cycle delta between this dependency and the previous transaction
             */
            explicit TransactionDependencyRecord(const TransactionRecord& transaction_record,
                                                 const uint64_t cycle_delta = 0) :
                TransactionDependencyRecord(transaction_record.getTransactionId(), cycle_delta, transaction_record.getClockId())
            {
            }

            /**
             * Packs a TransactionDependencyRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer,
                       dependency_id_,
                       cycle_delta_,
                       clock_id_);
            }

            /**
             * Unpacks a TransactionDependencyRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader,
                      dependency_id_,
                      cycle_delta_,
                      clock_id_);
            }

            /**
             * Formats a TransactionDependencyRecord to an std::ostream
             * \param os ostream to use
             */
            inline void format_impl(std::ostream& os) const {
                format_utils::formatLabel(os, "DEPENDENCY ID");
                format_utils::formatDec(os, dependency_id_);
                os << std::endl;
                format_utils::formatLabel(os, "CLOCK");
                os << ClockRegistry::getClockName(clock_id_) << std::endl;
                format_utils::formatLabel(os, "DELTA");
                format_utils::formatDec(os, cycle_delta_);
                os << std::endl;
            }

            /**
             * Gets the dependency ID
             */
            inline uint64_t getDependencyId() const {
                return dependency_id_;
            }

            /**
             * Gets the cycle delta
             */
            inline uint64_t getCycleDelta() const {
                return cycle_delta_;
            }

            /**
             * Gets the clock domain ID
             */
            inline ClockId getClockId() const {
                return clock_id_;
            }
    };

    REGISTER_RECORD(TransactionDependencyRecord)
} // end namespace stf

#endif
