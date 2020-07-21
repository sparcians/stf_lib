#include "stf_descriptor.hpp"
#include "stf_ifstream.hpp"
#include "stf_enums.hpp"
#include "stf_record.hpp"
#include "stf_record_factory.hpp"

namespace stf {
    STFIFstream& STFIFstream::operator>>(STFRecord::UniqueHandle& rec) {
        try {
            descriptors::encoded::Descriptor desc;
            operator>>(desc);
            rec = RecordFactory::construct(desc, *this);
            recordCallback();
        }
        catch(const InvalidDescriptorException&) {
            // Check to see if the invalid descriptor was because the file ended - if it was we'll raise a new exception
            checkStream_();
            // Otherwise, re-raise the current exception
            throw;
        }
        // Commenting this out because it gives a big speed boost, but this needs more testing to ensure it's safe
        //checkStream_();
        return *this;
    }
} // end namespace stf
