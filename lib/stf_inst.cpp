#include <cstdint>
#include <iterator>
#include <memory>
#include <ostream>
#include <string_view>
#include <vector>

#include "format_utils.hpp"
#include "stf_enums.hpp"
#include "stf_inst.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_reg_def.hpp"

namespace stf {
    std::ostream& operator<<(std::ostream& os, const MemAccess& access) {
        if (format_utils::showPhys()) {
            format_utils::formatSpaces(os, format_utils::PA_WIDTH + 1);
        }
        format_utils::formatOperandLabel(os, "MEM ");
        format_utils::formatLeft(os, access.getType(), format_utils::MEM_ACCESS_FIELD_WIDTH);
        format_utils::formatSpaces(os, 1);
        format_utils::formatHex(os, access.getAttr());
        format_utils::formatSpaces(os, 4);
        format_utils::formatVA(os, access.getAddress());

        /* TODO: Figure out physical address support
        if (format_utils::showPhys()) {
            os << ":";
            format_utils::formatPA(os, access.paddr_);
        }*/
        format_utils::formatSpaces(os, 1);
        format_utils::formatData(os, access.getData());

        return os;
    }

    std::string_view Operand::getLabel() const {
        switch(rec_->getOperandType()) {
            case Registers::STF_REG_OPERAND_TYPE::REG_DEST:
                return dest_label_;
            case Registers::STF_REG_OPERAND_TYPE::REG_SOURCE:
                return src_label_;
            case Registers::STF_REG_OPERAND_TYPE::REG_STATE:
                return state_label_;
            case Registers::STF_REG_OPERAND_TYPE::REG_RESERVED:
                break;
        }
        stf_throw("InstRegRecord did not have a valid operand type");
    }

    std::ostream& operator<<(std::ostream& os, const Operand& reg) {
        if (format_utils::showPhys()) {
            format_utils::formatSpaces(os, format_utils::PA_WIDTH + 1);
        }
        format_utils::formatOperandLabel(os, reg.getLabel());
        format_utils::formatRegisterName(os, reg.rec_->getReg());
        if (format_utils::showPhys()) {
            format_utils::formatSpaces(os, format_utils::PA_WIDTH + 1);
        }
        format_utils::formatData(os, reg.rec_->getData());

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Event& event) {
        if (format_utils::showPhys()) {
            format_utils::formatSpaces(os, format_utils::PA_WIDTH + 1);
        }
        if(event.isModeChange()) {
            format_utils::formatOperandLabel(os, "MODE ");
            const auto& data = event.getData();
            stf_assert(!data.empty(), "Attempted to print mode change event without accompanying EventContentRecord");
            os << static_cast<EXECUTION_MODE>(data.front()) << std::endl;
        }
        else {
            if(event.isSyscall()) {
                format_utils::formatOperandLabel(os, "SYSCALL ");
            }
            else if(event.isInterrupt()) {
                format_utils::formatOperandLabel(os, "INTERRUPT ");
            }
            else {
                format_utils::formatOperandLabel(os, "FAULT ");
            }
            format_utils::formatEvent(os, event.getEvent());
            if (event.dataValid()) {
                bool first_line = true;
                for(const auto& d: event.getData()) {
                    if (format_utils::showPhys()) {
                        format_utils::formatSpaces(os, format_utils::PA_WIDTH + 1);
                    }
                    if(STF_EXPECT_FALSE(first_line)) {
                        first_line = false;
                    }
                    else {
                        format_utils::formatSpaces(os, format_utils::OPERAND_LABEL_WIDTH + format_utils::EVENT_WIDTH);
                    }
                    format_utils::formatHex(os, d, format_utils::EVENT_DATA_WIDTH);
                }
            }
        }
        return os;
    }

    const InstRegRecord STFInst::x0_src_(Registers::STF_REG::STF_REG_X0,
                                         Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                         0);
    const InstRegRecord STFInst::x0_dest_(Registers::STF_REG::STF_REG_X0,
                                          Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                          0);
    const boost::container::flat_map<descriptors::internal::Descriptor, descriptors::internal::Descriptor> STFInst::PAIRED_RECORDS_ {
        {descriptors::internal::Descriptor::STF_INST_MEM_ACCESS, descriptors::internal::Descriptor::STF_INST_MEM_CONTENT},
        {descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS, descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT},
        {descriptors::internal::Descriptor::STF_EVENT, descriptors::internal::Descriptor::STF_EVENT_PC_TARGET}
    };

    const boost::container::flat_set<descriptors::internal::Descriptor> STFInst::SKIPPED_PAIRED_RECORDS_ {
        descriptors::internal::Descriptor::STF_INST_MEM_CONTENT,
        descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT,
        //descriptors::internal::Descriptor::STF_EVENT_PC_TARGET
    };
} // end namespace stf
