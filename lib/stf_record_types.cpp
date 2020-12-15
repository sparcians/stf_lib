#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "format_utils.hpp"
#include "stf_enums.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"
#include "stf_record_factory.hpp"
#include "stf_record_pool.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"
#include "stf_serializable_container.hpp"
#include "stf_writer.hpp"

#define REGISTER_RECORD(desc, cls) \
    namespace RecordFactoryRegistrations { \
        inline static STFRecordConstUniqueHandle cls##_FactoryMethod(STFIFstream& strm) { \
            return STFRecordPool::construct<cls>(strm); \
        } \
        template<> \
        class RecordFactoryRegistration<cls> { \
            public: \
                explicit RecordFactoryRegistration(descriptors::encoded::Descriptor) { \
                    RecordFactory::get_().registerRecordFactory_(descriptors::encoded::Descriptor::desc, cls##_FactoryMethod); \
                    STFRecordPool::registerDeleter<cls>(descriptors::internal::Descriptor::desc); \
                } \
        }; \
        static const RecordFactoryRegistration<cls> registration_##cls(descriptors::encoded::Descriptor::desc); \
    } \
    template<> \
    descriptors::internal::Descriptor TypeAwareSTFRecord<cls>::getTypeDescriptor() { \
        return descriptors::internal::Descriptor::desc; \
    }

namespace stf {
    REGISTER_RECORD(STF_IDENTIFIER, STFIdentifierRecord)

    REGISTER_RECORD(STF_VERSION, VersionRecord)

    std::ostream& operator<<(std::ostream& os, const CommentRecord& comment) {
        format_utils::formatLabel(os, "    COMMENT");
        comment.format_impl(os);
        return os;
    }

    REGISTER_RECORD(STF_COMMENT, CommentRecord)

    REGISTER_RECORD(STF_ISA, ISARecord)

    REGISTER_RECORD(STF_INST_IEM, InstIEMRecord)

    REGISTER_RECORD(STF_FORCE_PC, ForcePCRecord)

    REGISTER_RECORD(STF_VLEN_CONFIG, VLenConfigRecord)

    REGISTER_RECORD(STF_END_HEADER, EndOfHeaderRecord)

    STFOFstream& operator<<(STFOFstream& writer, const PageTableWalkRecord::PTE& rec) {
        rec.pack_impl(writer);
        return writer;
    }

    STFIFstream& operator>>(STFIFstream& reader, PageTableWalkRecord::PTE& rec) {
        rec.unpack_impl(reader);
        return reader;
    }

    std::ostream& operator<<(std::ostream& os, const PageTableWalkRecord& pte) {
        format_utils::formatLabel(os, "PTE");
        pte.format_impl(os);
        return os;
    }

    REGISTER_RECORD(STF_PAGE_TABLE_WALK, PageTableWalkRecord)

    REGISTER_RECORD(STF_PROCESS_ID_EXT, ProcessIDExtRecord)

    REGISTER_RECORD(STF_EVENT, EventRecord)

    std::ostream& operator<<(std::ostream& os, const EventRecord::TYPE event_type) {
        switch(event_type) {
            case EventRecord::TYPE::INST_ADDR_MISALIGN:
                return os << "INST_ADDR_MISALIGN";
            case EventRecord::TYPE::INST_ADDR_FAULT:
                return os << "INST_ADDR_FAULT";
            case EventRecord::TYPE::ILLEGAL_INST:
                return os << "ILLEGAL_INST";
            case EventRecord::TYPE::BREAKPOINT:
                return os << "BREAKPOINT";
            case EventRecord::TYPE::LOAD_ADDR_MISALIGN:
                return os << "LOAD_ADDR_MISALIGN";
            case EventRecord::TYPE::LOAD_ACCESS_FAULT:
                return os << "LOAD_ACCESS_FAULT";
            case EventRecord::TYPE::STORE_ADDR_MISALIGN:
                return os << "STORE_ADDR_MISALIGN";
            case EventRecord::TYPE::STORE_ACCESS_FAULT:
                return os << "STORE_ACCESS_FAULT";
            case EventRecord::TYPE::USER_ECALL:
                return os << "USER_ECALL";
            case EventRecord::TYPE::SUPERVISOR_ECALL:
                return os << "SUPERVISOR_ECALL";
            case EventRecord::TYPE::HYPERVISOR_ECALL:
                return os << "HYPERVISOR_ECALL";
            case EventRecord::TYPE::MACHINE_ECALL:
                return os << "MACHINE_ECALL";
            case EventRecord::TYPE::INST_PAGE_FAULT:
                return os << "INST_PAGE_FAULT";
            case EventRecord::TYPE::LOAD_PAGE_FAULT:
                return os << "LOAD_PAGE_FAULT";
            case EventRecord::TYPE::STORE_PAGE_FAULT:
                return os << "STORE_PAGE_FAULT";
            case EventRecord::TYPE::INT_USER_SOFTWARE:
                return os << "INT_USER_SOFTWARE";
            case EventRecord::TYPE::INT_SUPERVISOR_SOFTWARE:
                return os << "INT_SUPERVISOR_SOFTWARE";
            case EventRecord::TYPE::INT_HYPERVISOR_SOFTWARE:
                return os << "INT_HYPERVISOR_SOFTWARE";
            case EventRecord::TYPE::INT_MACHINE_SOFTWARE:
                return os << "INT_MACHINE_SOFTWARE";
            case EventRecord::TYPE::INT_USER_TIMER:
                return os << "INT_USER_TIMER";
            case EventRecord::TYPE::INT_SUPERVISOR_TIMER:
                return os << "INT_SUPERVISOR_TIMER";
            case EventRecord::TYPE::INT_HYPERVISOR_TIMER:
                return os << "INT_HYPERVISOR_TIMER";
            case EventRecord::TYPE::INT_MACHINE_TIMER:
                return os << "INT_MACHINE_TIMER";
            case EventRecord::TYPE::INT_USER_EXT:
                return os << "INT_USER_EXT";
            case EventRecord::TYPE::INT_SUPERVISOR_EXT:
                return os << "INT_SUPERVISOR_EXT";
            case EventRecord::TYPE::INT_HYPERVISOR_EXT:
                return os << "INT_HYPERVISOR_EXT";
            case EventRecord::TYPE::INT_MACHINE_EXT:
                return os << "INT_MACHINE_EXT";
            case EventRecord::TYPE::INT_COPROCESSOR:
                return os << "INT_COPROCESSOR";
            case EventRecord::TYPE::INT_HOST:
                return os << "INT_HOST";
            case EventRecord::TYPE::MODE_CHANGE:
                return os << "MODE_CHANGE";
        };

        stf_throw("Invalid EventRecord::TYPE value: " << enums::to_printable_int(event_type));
    }

    REGISTER_RECORD(STF_EVENT_PC_TARGET, EventPCTargetRecord)

    REGISTER_RECORD(STF_INST_PC_TARGET, InstPCTargetRecord)

    REGISTER_RECORD(STF_INST_REG, InstRegRecord)

    REGISTER_RECORD(STF_INST_MEM_CONTENT, InstMemContentRecord)

    REGISTER_RECORD(STF_INST_MEM_ACCESS, InstMemAccessRecord)

    REGISTER_RECORD(STF_INST_OPCODE32, InstOpcode32Record)

    REGISTER_RECORD(STF_INST_OPCODE16, InstOpcode16Record)

    REGISTER_RECORD(STF_INST_MICROOP, InstMicroOpRecord)

    REGISTER_RECORD(STF_INST_READY_REG, InstReadyRegRecord)

    REGISTER_RECORD(STF_BUS_MASTER_ACCESS, BusMasterAccessRecord)

    REGISTER_RECORD(STF_BUS_MASTER_CONTENT, BusMasterContentRecord)

    std::ostream& operator<<(std::ostream& os, const TraceInfoRecord& rec) {
        format_utils::formatLabel(os, "GENERATOR");
        os << rec.getGenerator() << std::endl;
        format_utils::formatLabel(os, "GEN_VERSION");
        os << rec.getVersionString() << std::endl;
        format_utils::formatLabel(os, "GEN_COMMENT");
        os << rec.getComment() << std::endl;
        return os;
    }

    REGISTER_RECORD(STF_TRACE_INFO, TraceInfoRecord)

    REGISTER_RECORD(STF_TRACE_INFO_FEATURE, TraceInfoFeatureRecord)

} // end namespace stf
