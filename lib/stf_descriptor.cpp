#include "stf_descriptor.hpp"

namespace stf {
    namespace descriptors {
        namespace internal {
            std::ostream& operator<<(std::ostream& os, const Descriptor desc) {
                switch(desc) {
                    case Descriptor::STF_RESERVED:
                        os << "RESERVED";
                        return os;
                    case Descriptor::STF_IDENTIFIER:
                        os << "STF_IDENTIFIER";
                        return os;
                    case Descriptor::STF_VERSION:
                        os << "VERSION";
                        return os;
                    case Descriptor::STF_COMMENT:
                        os << "COMMENT";
                        return os;
                    case Descriptor::STF_ISA:
                        os << "ISA";
                        return os;
                    case Descriptor::STF_INST_IEM:
                        os << "INST_IEM";
                        return os;
                    case Descriptor::STF_TRACE_INFO:
                        os << "TRACE_INFO";
                        return os;
                    case Descriptor::STF_TRACE_INFO_FEATURE:
                        os << "TRACE_INFO_FEATURE";
                        return os;
                    case Descriptor::STF_PROCESS_ID_EXT:
                        os << "PROCESS_ID_EXT";
                        return os;
                    case Descriptor::STF_END_HEADER:
                        os << "END_HEADER";
                        return os;
                    case Descriptor::STF_INST_OPCODE32:
                        os << "INST_OPCODE32";
                        return os;
                    case Descriptor::STF_INST_OPCODE16:
                        os << "INST_OPCODE16";
                        return os;
                    case Descriptor::STF_INST_REG:
                        os << "INST_REG";
                        return os;
                    case Descriptor::STF_INST_READY_REG:
                        os << "INST_READY_REG";
                        return os;
                    case Descriptor::STF_FORCE_PC:
                        os << "FORCE_PC";
                        return os;
                    case Descriptor::STF_INST_PC_TARGET:
                        os << "INST_PC_TARGET";
                        return os;
                    case Descriptor::STF_EVENT:
                        os << "EVENT";
                        return os;
                    case Descriptor::STF_EVENT_PC_TARGET:
                        os << "EVENT_PC_TARGET";
                        return os;
                    case Descriptor::STF_INST_MEM_ACCESS:
                        os << "INST_MEM_ACCESS";
                        return os;
                    case Descriptor::STF_INST_MEM_CONTENT:
                        os << "INST_MEM_CONTENT";
                        return os;
                    case Descriptor::STF_BUS_MASTER_ACCESS:
                        os << "BUS_MASTER_ACCESS";
                        return os;
                    case Descriptor::STF_BUS_MASTER_CONTENT:
                        os << "BUS_MASTER_CONTENT";
                        return os;
                    case Descriptor::STF_PAGE_TABLE_WALK:
                        os << "PAGE_TABLE_WALK";
                        return os;
                    case Descriptor::STF_INST_MICROOP:
                        os << "INST_MICROOP";
                        return os;
                    case Descriptor::STF_RESERVED_END:
                        os << "RESERVED_END";
                        return os;
                };

                os << "UNKNOWN_" + std::to_string(enums::to_printable_int(desc));

                return os;
            }
        } // end namespace internal

        namespace encoded {
            std::ostream& operator<<(std::ostream& os, const Descriptor desc) {
                return os << conversion::toInternal(desc);
            }
        } // end namespace encoded
    } // end namespace descriptors
} // end namespace stf
