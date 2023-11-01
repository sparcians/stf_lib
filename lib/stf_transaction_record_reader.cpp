#include "stf_record_types.hpp"
#include "stf_transaction_reader.hpp"
#include "stf_transaction_writer.hpp"

namespace stf {
    void STFTransactionRecordReader::readHeader_() {
        bool complete_header = false;
        try {
            STFRecord::UniqueHandle rec;

            try {
                operator>>(rec);

                stf_assert(rec->getId() == descriptors::internal::Descriptor::STF_IDENTIFIER && rec->as<STFIdentifierRecord>().isValid(),
                           "Specified file looks like an STF, but does not have a valid STF_IDENTIFIER record");
            }
            catch(const InvalidDescriptorException&) {
                stf_throw("Specified file is not an STF");
            }

            operator>>(rec);

            stf_assert(rec->getId() == descriptors::internal::Descriptor::STF_VERSION,
                       "The second record must be the version");

            STFRecord::grabOwnership(version_, rec);

            do {
                operator>>(rec);
                switch(rec->getId()) {
                    case descriptors::internal::Descriptor::STF_COMMENT:
                        header_comments_.emplace_back(STFRecord::grabOwnership<CommentRecord>(rec));
                        break;
                    case descriptors::internal::Descriptor::STF_TRACE_INFO:
                        trace_info_records_.emplace_back(STFRecord::grabOwnership<TraceInfoRecord>(rec));
                        break;
                    case descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE:
                        stf_assert(!trace_features_, "Header has multiple TRACE_INFO_FEATURE records");
                        STFRecord::grabOwnership(trace_features_, rec);
                        break;
                    case descriptors::internal::Descriptor::STF_PROTOCOL_ID:
                        // This record is handled internally by the STFIFstream
                        stf_assert((getProtocolId() == expected_protocol_) || (expected_protocol_ == protocols::ProtocolId::__RESERVED_END),
                                   "Expected protocol " << expected_protocol_ << ", but trace contains " << getProtocolId());
                        break;
                    case descriptors::internal::Descriptor::STF_CLOCK_ID:
                        {
                            const auto& clock_id_rec = rec->as<ClockIdRecord>();
                            ClockRegistry::registerClock(clock_id_rec.getClockId(), clock_id_rec.getName());
                        }
                        break;
                    case descriptors::internal::Descriptor::STF_END_HEADER:
                        complete_header = true;
                        break;
                    case descriptors::internal::Descriptor::STF_ISA:
                    case descriptors::internal::Descriptor::STF_INST_IEM:
                    case descriptors::internal::Descriptor::STF_FORCE_PC:
                    case descriptors::internal::Descriptor::STF_PROCESS_ID_EXT:
                    case descriptors::internal::Descriptor::STF_VLEN_CONFIG:
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
                        stf_throw("Attempted to open an instruction trace with a transaction reader");
                    case descriptors::internal::Descriptor::STF_TRANSACTION:
                    case descriptors::internal::Descriptor::STF_TRANSACTION_DEPENDENCY:
                    case descriptors::internal::Descriptor::STF_IDENTIFIER:
                    case descriptors::internal::Descriptor::STF_VERSION:
                        stf_throw("Encountered unexpected STF record in header: " << rec->getId());
                    // These records can't be constructed
                    case descriptors::internal::Descriptor::STF_RESERVED:
                    case descriptors::internal::Descriptor::__RESERVED_END:
                        __builtin_unreachable();
                }
            }
            while(!complete_header);
        }
        catch(const EOFException& e) {
            // It's ok if the file ends while we're processing the header, because we're going to check whether it's complete
            (void) e;
        }
        stf_assert(complete_header, "STF ended with an incomplete header!");
        validateHeader_();
    }

    // cppcheck-suppress unusedFunction
    void STFTransactionRecordReader::copyHeader(STFTransactionWriter& stf_writer) const {
        stf_writer.addHeaderComments(header_comments_);
        stf_writer.addTraceInfoRecords(trace_info_records_);
        stf_writer.setTraceFeature(trace_features_->getFeatures());
        stf_writer.setProtocolId(getProtocolId());
        for(const auto& clock_info: ClockRegistry::dumpClocks()) {
            stf_writer.addClock(clock_info.first, clock_info.second);
        }
    }

    // cppcheck-suppress unusedFunction
    void STFTransactionRecordReader::dumpHeader(std::ostream& os) const {
        version_->format(os);
        os << std::endl;
        for(const auto& c: header_comments_) {
            c->format(os);
            os << std::endl;
        }
        for(const auto& i: trace_info_records_) {
            i->format(os);
            os << std::endl;
        }
        trace_features_->format(os);
        os << std::endl;
        os << "PROTOCOL " << getProtocolId() << std::endl;
    }
} // end namespace stf
