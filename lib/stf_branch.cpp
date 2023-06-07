#include "format_utils.hpp"
#include "stf_branch.hpp"

namespace stf {
    std::ostream& operator<<(std::ostream& os, const STFBranch& branch) {
        static constexpr int TAKEN_NOT_TAKEN_WIDTH = 5;
        static constexpr int BRANCH_TYPE_WIDTH = 22;
        static constexpr int FIELD_PADDING = 4;

        format_utils::formatDecLeft(os, branch.index(), format_utils::LABEL_WIDTH / 2);
        format_utils::formatDecLeft(os, branch.instIndex(), format_utils::LABEL_WIDTH / 2);
        format_utils::formatVA(os, branch.getPC());
        format_utils::formatSpaces(os, FIELD_PADDING);
        format_utils::formatOpcode(os, branch.getOpcode());

        if(branch.isTaken()) {
            format_utils::formatWidth(os, 'T', TAKEN_NOT_TAKEN_WIDTH);
            format_utils::formatSpaces(os, FIELD_PADDING);
            format_utils::formatVA(os, branch.getTargetPC());
        }
        else {
            format_utils::formatWidth(os, 'N', TAKEN_NOT_TAKEN_WIDTH);
            format_utils::formatSpaces(os, FIELD_PADDING + format_utils::VA_WIDTH);
        }

        if(branch.isMillireturn()) {
            if(branch.isMillicall()) {
                format_utils::formatWidth(os, "MILLIRET/MILLICALL", BRANCH_TYPE_WIDTH);
            }
            else if(branch.isCall()) {
                format_utils::formatWidth(os, "MILLIRET/CALL", BRANCH_TYPE_WIDTH);
            }
            else {
                format_utils::formatWidth(os, "MILLIRETURN", BRANCH_TYPE_WIDTH);
            }
        }
        else if(branch.isReturn()) {
            if(branch.isMillicall()) {
                format_utils::formatWidth(os, "RET/MILLICALL", BRANCH_TYPE_WIDTH);
            }
            else if(branch.isCall()) {
                format_utils::formatWidth(os, "RET/CALL", BRANCH_TYPE_WIDTH);
            }
            else {
                format_utils::formatWidth(os, "RETURN", BRANCH_TYPE_WIDTH);
            }
        }
        else if(branch.isMillicall()) {
            format_utils::formatWidth(os, "MILLICALL", BRANCH_TYPE_WIDTH);
        }
        else if(branch.isCall()) {
            format_utils::formatWidth(os, "CALL", BRANCH_TYPE_WIDTH);
        }
        else if(branch.isIndirect()) {
            format_utils::formatWidth(os, "INDIRECT", BRANCH_TYPE_WIDTH);
        }
        else if(branch.isConditional()) {
            format_utils::formatWidth(os, "COND", BRANCH_TYPE_WIDTH);
        }
        else {
            format_utils::formatWidth(os, "JUMP", BRANCH_TYPE_WIDTH);
        }

        return os;
    }
} // end namespace stf
