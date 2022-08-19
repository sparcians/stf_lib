#include <ostream>

#include "format_utils.hpp"
#include "stf_enums.hpp"
#include "stf_factory.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    std::ostream& operator<<(std::ostream& os, const CommentRecord& comment) {
        format_utils::formatLabel(os, "    COMMENT");
        comment.format_impl(os);
        return os;
    }

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
            case EventRecord::TYPE::GUEST_INST_PAGE_FAULT:
                return os << "GUEST_INST_PAGE_FAULT";
            case EventRecord::TYPE::GUEST_LOAD_PAGE_FAULT:
                return os << "GUEST_LOAD_PAGE_FAULT";
            case EventRecord::TYPE::VIRTUAL_INST:
                return os << "VIRTUAL_INST";
            case EventRecord::TYPE::GUEST_STORE_PAGE_FAULT:
                return os << "GUEST_STORE_PAGE_FAULT";
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

    std::ostream& operator<<(std::ostream& os, const TraceInfoRecord& rec) {
        format_utils::formatLabel(os, "GENERATOR");
        os << rec.getGenerator() << std::endl;
        format_utils::formatLabel(os, "GEN_VERSION");
        os << rec.getVersionString() << std::endl;
        format_utils::formatLabel(os, "GEN_COMMENT");
        os << rec.getComment() << std::endl;
        return os;
    }

    // REQUIRED to properly instantiate RecordFactory and all STFRecord types
    // Should only be specified ONCE in a .cpp file
    FINALIZE_FACTORY(STFRecord)
} // end namespace stf
