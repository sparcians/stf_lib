#include "stf_pc_tracker.hpp"
#include "stf_inst.hpp"

namespace stf {
    void PCTracker::track(const ForcePCRecord& rec) {
        next_pc_ = rec.getAddr() + pc_offset_;
    }

    void PCTracker::track(const STFInst& inst) {
        pc_ = inst.pc();

        if(STF_EXPECT_FALSE(inst.isTakenBranch())) {
            next_pc_ = inst.branchTarget();
        }
        else {
            next_pc_ += inst.opcodeSize();
        }
    }

} // end namespace stf
