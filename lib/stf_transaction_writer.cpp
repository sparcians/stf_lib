#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_transaction_writer.hpp"

namespace stf {
    void STFTransactionWriter::setProtocolId(const protocols::ProtocolId protocol_id) {
        protocol_id_ = STFRecord::make<ProtocolIdRecord>(protocol_id);
    }

    // cppcheck-suppress unusedFunction
    void STFTransactionWriter::addClock(const ClockId clock_id, const std::string_view name) {
        ClockRegistry::registerClock(clock_id, name);
        clock_ids_.emplace_back(clock_id, name);
    }

    void STFTransactionWriter::flushHeader() {
        stf_assert(!header_finalized_, "Cannot write anything else to the header after it has been finalized");

        if(!header_comments_written_ && !header_comments_.empty()) {
            stf_assert(!(protocol_id_written_ || trace_info_records_written_ || trace_features_written_),
                       "Header comment block must come before any other header records");
            for(const auto& c: header_comments_) {
                *this << c;
            }
            header_comments_written_ = true;
        }

        if(!trace_info_records_written_ && !trace_info_records_.empty()) {
            stf_assert(!protocol_id_written_, "TRACE_INFO records must come before PROTOCOL_ID record");
            stf_assert(!trace_features_written_, "TRACE_INFO records must come before TRACE_INFO_FEATURE record");
            for(const auto& r: trace_info_records_) {
                *this << r;
            }
            trace_info_records_written_ = true;
        }

        if(!trace_features_written_ && trace_features_) {
            stf_assert(trace_info_records_written_, "TRACE_INFO records must come before TRACE_INFO_FEATURE record");
            stf_assert(!protocol_id_written_, "TRACE_INFO_FEATURE record must come before PROTOCOL_ID record");
            *this << *trace_features_;
            trace_features_written_ = true;
        }

        if(!protocol_id_written_ && protocol_id_) {
            stf_assert(trace_info_records_written_,
                       "TRACE_INFO record must come before PROTOCOL_ID record");
            stf_assert(trace_features_written_,
                       "TRACE_INFO_FEATURE record must come before PROTOCOL_ID record");
            stf_assert(!clock_ids_written_,
                       "PROTOCOL_ID record must come before CLOCK_ID record");
            *this << *protocol_id_;
            protocol_id_written_ = true;
        }

        if(!clock_ids_written_) {
            stf_assert(protocol_id_written_, "PROTOCOL_ID record must come before CLOCK_ID record");
            stf_assert(!clock_ids_.empty(), "At least one clock ID must be specified");
            for(const auto& clock_id: clock_ids_) {
                *this << clock_id;
            }
            clock_ids_written_ = true;
        }
    }

    void STFTransactionWriter::finalizeHeader() {
        if(header_finalized_) {
            return;
        }

        stf_assert(header_started_, "Attempted to finalize the header before anything has been written to it");

        if(!(header_comments_written_ &&
             trace_info_records_written_ &&
             trace_features_written_ &&
             protocol_id_written_ &&
             clock_ids_written_)) {
            flushHeader();
        }

        STFWriterBase::finalizeHeader();
    }

    int STFTransactionWriter::close() {
        protocol_id_.reset();
        protocol_id_written_ = false;
        return STFWriterBase::close();
    }

    STFTransactionWriter& STFTransactionWriter::operator<<(const STFRecord& rec) {
        const auto desc = rec.getId();

        switch(desc) {
            case descriptors::internal::Descriptor::STF_IDENTIFIER:
            case descriptors::internal::Descriptor::STF_VERSION:
                stf_assert(!headerStarted(), "Attempted to write more than one " << desc << " record!");
                break;
            case descriptors::internal::Descriptor::STF_COMMENT:
                stf_assert(headerStarted(), "Attempted to write a comment before STF_IDENTIFIER or VERSION records");
                break;
            case descriptors::internal::Descriptor::STF_TRACE_INFO:
            case descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE:
            case descriptors::internal::Descriptor::STF_PROTOCOL_ID:
            case descriptors::internal::Descriptor::STF_CLOCK_ID:
            case descriptors::internal::Descriptor::STF_END_HEADER:
                stf_assert(!headerFinalized(), "Attempted to write " << desc << " record outside of the header");
                stf_assert(headerStarted(), "Attempted to write " << desc << " before the header has started");
                break;
            case descriptors::internal::Descriptor::STF_TRANSACTION:
            case descriptors::internal::Descriptor::STF_TRANSACTION_DEPENDENCY:
                stf_assert(headerFinalized(), "Attempted to write " << desc << " record before finalizing the header");
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
            case descriptors::internal::Descriptor::STF_ISA:
            case descriptors::internal::Descriptor::STF_PROCESS_ID_EXT:
            case descriptors::internal::Descriptor::STF_VLEN_CONFIG:
            case descriptors::internal::Descriptor::STF_ISA_EXTENDED:
            case descriptors::internal::Descriptor::STF_INST_IEM:
            case descriptors::internal::Descriptor::STF_FORCE_PC:
                stf_throw("Attempted to write instruction record " << desc << " in a transaction trace");
            case descriptors::internal::Descriptor::STF_RESERVED:
            case descriptors::internal::Descriptor::__RESERVED_END:
                stf_throw("Attempted to write reserved record: " << desc);
        }

        STFWriterBase::operator<<(rec);
        return *this;
    }
} // end namespace stf
