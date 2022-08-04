#include "stf_ifstream.hpp"
#include "stf_record.hpp"

namespace stf {
    void STFIFstream::seek(size_t num_instructions) {
        const size_t end_inst_num = num_insts_ + num_instructions;
        STFRecord::UniqueHandle rec;

        try {
            while(operator bool() && (num_insts_ < end_inst_num)) {
                operator>>(rec);
            }
        }
        catch(const EOFException&) {
            stf_throw("Attempted to seek past the end of the trace");
        }
    }
} // end namespace stf
