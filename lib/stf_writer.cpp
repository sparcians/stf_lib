#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_writer.hpp"

namespace stf {
    void STFWriter::setISA(ISA isa) {
        isa_ = STFRecord::make<ISARecord>(isa);
    }

    void STFWriter::setHeaderIEM(INST_IEM iem) {
        initial_iem_ = STFRecord::make<InstIEMRecord>(iem);
    }

    void STFWriter::setHeaderPC(uint64_t pc) {
        initial_pc_ = STFRecord::make<ForcePCRecord>(pc);
    }

    void STFWriter::setVLen(const vlen_t vlen) {
        vlen_config_ = STFRecord::make<VLenConfigRecord>(vlen);
    }

    void STFWriter::flushHeader() {
        stf_assert(!header_finalized_, "Cannot write anything else to the header after it has been finalized");

        if(!header_comments_written_ && !header_comments_.empty()) {
            stf_assert(!(isa_written_ || initial_iem_written_ || initial_pc_written_ || trace_info_records_written_ || trace_features_written_),
                       "Header comment block must come before any other header records");
            for(const auto& c: header_comments_) {
                *this << c;
            }
            header_comments_written_ = true;
        }

        if(!isa_written_ && isa_) {
            stf_assert(!(initial_iem_written_ || initial_pc_written_ || trace_info_records_written_ || trace_features_written_),
                       "ISA record must come before IEM, FORCE_PC, TRACE_INFO, or TRACE_INFO_FEATURE records");
            *this << *isa_;
            isa_written_ = true;
        }

        if(!initial_iem_written_ && initial_iem_) {
            stf_assert(isa_written_, "ISA record must come before IEM record");
            stf_assert(!(initial_pc_written_ || trace_info_records_written_ || trace_features_written_),
                       "IEM record must come before FORCE_PC, TRACE_INFO, or TRACE_INFO_FEATURE records");
            *this << *initial_iem_;
            initial_iem_written_ = true;
        }

        if(!trace_info_records_written_ && !trace_info_records_.empty()) {
            stf_assert(isa_written_, "ISA record must come before TRACE_INFO record");
            stf_assert(initial_iem_written_, "IEM record must come before TRACE_INFO record");
            stf_assert(!initial_pc_written_, "TRACE_INFO records must come before FORCE_PC record");
            stf_assert(!trace_features_written_, "TRACE_INFO records must come before TRACE_INFO_FEATURE record");
            for(const auto& r: trace_info_records_) {
                *this << r;
            }
            trace_info_records_written_ = true;
        }

        if(!trace_features_written_ && trace_features_) {
            stf_assert(isa_written_, "ISA record must come before TRACE_INFO_FEATURE record");
            stf_assert(initial_iem_written_, "IEM record must come before TRACE_INFO_FEATURE record");
            stf_assert(trace_info_records_written_, "TRACE_INFO records must come before TRACE_INFO_FEATURE record");
            stf_assert(!initial_pc_written_, "TRACE_INFO_FEATURE record must come before FORCE_PC record");
            *this << *trace_features_;
            trace_features_written_ = true;
        }

        if(!initial_pc_written_ && initial_pc_) {
            stf_assert(isa_written_, "ISA record must come before FORCE_PC record");
            stf_assert(initial_iem_written_, "IEM record must come before FORCE_PC record");
            stf_assert(trace_info_records_written_,
                       "TRACE_INFO record must come before FORCE_PC record");
            stf_assert(trace_features_written_,
                       "TRACE_INFO_FEATURE record must come before FORCE_PC record");
            *this << *initial_pc_;
            initial_pc_written_ = true;
        }

        if(!vlen_config_written_ && vlen_config_) {
            *this << *vlen_config_;
            vlen_config_written_ = true;
        }
    }

    // cppcheck-suppress unusedFunction
    void STFWriter::finalizeHeader() {
        if(header_finalized_) {
            return;
        }

        stf_assert(header_started_, "Attempted to finalize the header before anything has been written to it");

        // Set up any ISA/IEM-dependent options
        const bool is_riscv = isa_ && isa_->getISA() == ISA::RISCV;
        const bool is_riscv64 = is_riscv && initial_iem_ && initial_iem_->getMode() == INST_IEM::STF_INST_IEM_RV64;

        // RV64 traces should use 64 bit events
        if(!force_32bit_events_ && is_riscv64) {
            setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_EVENT64);
        }

        if(!(header_comments_written_ &&
             isa_written_ &&
             initial_iem_written_ &&
             initial_pc_written_ &&
             trace_info_records_written_ &&
             trace_features_written_ &&
             (vlen_config_written_ || !vlen_config_))) { // VLenConfigRecord is optional
            flushHeader();
        }

        STFWriterBase::finalizeHeader();
    }

    int STFWriter::close() {
        isa_.reset();
        isa_written_ = false;
        initial_iem_.reset();
        initial_iem_written_ = false;
        initial_pc_.reset();
        initial_pc_written_ = false;
        vlen_config_.reset();
        vlen_config_written_ = false;
        last_desc_ = descriptors::encoded::Descriptor::STF_RESERVED;
        wrote_inst_memory_access_content_pair_ = false;
        wrote_bus_memory_access_content_pair_ = false;
        return STFWriterBase::close();
    }

    STFWriter& STFWriter::operator<<(const STFRecord& rec) {
        const auto desc = rec.getId();
        const auto encoded_desc = descriptors::conversion::toEncoded(desc);
        const bool last_was_memory_access = last_desc_ == descriptors::encoded::Descriptor::STF_INST_MEM_ACCESS;
        const bool last_was_bus_access = last_desc_ == descriptors::encoded::Descriptor::STF_BUS_MASTER_ACCESS;
        const bool cur_is_memory_content = encoded_desc == descriptors::encoded::Descriptor::STF_INST_MEM_CONTENT;
        const bool cur_is_bus_content = encoded_desc == descriptors::encoded::Descriptor::STF_BUS_MASTER_CONTENT;

        stf_assert(!last_was_memory_access || cur_is_memory_content,
                   descriptors::encoded::Descriptor::STF_INST_MEM_CONTENT
                   << " must follow "
                   << descriptors::encoded::Descriptor::STF_INST_MEM_ACCESS);

        stf_assert(!last_was_bus_access || cur_is_bus_content,
                   descriptors::encoded::Descriptor::STF_BUS_MASTER_CONTENT
                   << " must follow "
                   << descriptors::encoded::Descriptor::STF_BUS_MASTER_ACCESS);

        stf_assert(encoded_desc >= last_desc_ ||
                   STFRecord::isInstructionRecord(last_desc_) ||
                   (wrote_inst_memory_access_content_pair_ && desc == descriptors::internal::Descriptor::STF_INST_MEM_ACCESS) ||
                   (wrote_bus_memory_access_content_pair_ && desc == descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS) ||
                   (wrote_event_record_group_ && desc == descriptors::internal::Descriptor::STF_EVENT) ||
                   ((wrote_page_table_walk_ || wrote_reg_) && desc == descriptors::internal::Descriptor::STF_INST_PC_TARGET) ||
                   (wrote_page_table_walk_ && desc == descriptors::internal::Descriptor::STF_INST_REG) ||
                   (desc == descriptors::internal::Descriptor::STF_COMMENT),
                   "Attempted out of order write. " << desc << " should come before " << last_desc_);
        switch(desc) {
            case descriptors::internal::Descriptor::STF_IDENTIFIER:
            case descriptors::internal::Descriptor::STF_VERSION:
                stf_assert(!headerStarted(), "Attempted to write more than one " << desc << " record!");
                break;
            case descriptors::internal::Descriptor::STF_COMMENT:
                stf_assert(headerStarted(), "Attempted to write a comment before STF_IDENTIFIER or VERSION records");
                break;
            case descriptors::internal::Descriptor::STF_ISA:
            case descriptors::internal::Descriptor::STF_TRACE_INFO:
            case descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE:
            case descriptors::internal::Descriptor::STF_PROCESS_ID_EXT:
            case descriptors::internal::Descriptor::STF_VLEN_CONFIG:
            case descriptors::internal::Descriptor::STF_END_HEADER:
                stf_assert(!headerFinalized(), "Attempted to write " << desc << " record outside of the header"); //FALLTHRU
            case descriptors::internal::Descriptor::STF_INST_IEM:
            case descriptors::internal::Descriptor::STF_FORCE_PC:
                stf_assert(headerStarted(), "Attempted to write " << desc << " before the header has started");
                break;
            case descriptors::internal::Descriptor::STF_INST_OPCODE32:
            case descriptors::internal::Descriptor::STF_INST_OPCODE16:
            case descriptors::internal::Descriptor::STF_INST_REG:
            case descriptors::internal::Descriptor::STF_INST_READY_REG:
            case descriptors::internal::Descriptor::STF_INST_PC_TARGET:
            case descriptors::internal::Descriptor::STF_EVENT:
            case descriptors::internal::Descriptor::STF_EVENT_PC_TARGET:
            case descriptors::internal::Descriptor::STF_INST_MEM_ACCESS:
            case descriptors::internal::Descriptor::STF_INST_MEM_CONTENT:
            case descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS:
            case descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT:
            case descriptors::internal::Descriptor::STF_PAGE_TABLE_WALK:
            case descriptors::internal::Descriptor::STF_INST_MICROOP:
                stf_assert(headerFinalized(), "Attempted to write " << desc << " record before finalizing the header");
                break;
            case descriptors::internal::Descriptor::STF_PROTOCOL_ID:
            case descriptors::internal::Descriptor::STF_CLOCK_ID:
            case descriptors::internal::Descriptor::STF_TRANSACTION:
            case descriptors::internal::Descriptor::STF_TRANSACTION_DEPENDENCY:
                stf_throw("Attempted to write transaction record " << desc << " in an instruction trace");
            case descriptors::internal::Descriptor::STF_RESERVED:
            case descriptors::internal::Descriptor::__RESERVED_END:
                stf_throw("Attempted to write reserved record: " << desc);
        }

        STFWriterBase::operator<<(rec);

        wrote_inst_memory_access_content_pair_ = last_was_memory_access && cur_is_memory_content;
        wrote_bus_memory_access_content_pair_ = last_was_bus_access && cur_is_bus_content;

        const bool last_was_event = (last_desc_ == descriptors::encoded::Descriptor::STF_EVENT);
        wrote_event_record_group_ =
            ((encoded_desc == descriptors::encoded::Descriptor::STF_EVENT_PC_TARGET) && last_was_event);

        wrote_page_table_walk_ = (encoded_desc == descriptors::encoded::Descriptor::STF_PAGE_TABLE_WALK);
        wrote_reg_ = (encoded_desc == descriptors::encoded::Descriptor::STF_INST_REG);

        last_desc_ = encoded_desc;

        return *this;
    }
} // end namespace stf
