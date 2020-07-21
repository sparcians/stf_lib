#ifndef __STF_RECORD_FACTORY_HPP__
#define __STF_RECORD_FACTORY_HPP__

#include <memory>
#include <array>

#include "stf_descriptor.hpp"
#include "stf_enums.hpp"
#include "stf_record_pointers.hpp"

namespace stf {
    class STFRecord;
    class STFIFstream;

    namespace RecordFactoryRegistrations {
        /**
         * \class RecordFactoryRegistration
         *
         * Class that auto-registers an STF record
         *
         */
        template<typename T> class RecordFactoryRegistration;
    } // end namespace RecordFactoryRegistrations

    /**
     * \class RecordFactory
     *
     * Factory class for STF records. Constructs an STF record from an descriptors::encoded::Descriptor.
     *
     */
    class RecordFactory {
        private:
            template<typename T>
            friend class RecordFactoryRegistrations::RecordFactoryRegistration;

            /**
             * \typedef Callback
             * type of callback function used to construct a registered record type
             */
            using Callback = STFRecordConstUniqueHandle(*)(STFIFstream& strm);

            std::array<Callback, descriptors::internal::NUM_DESCRIPTORS> factory_callbacks_; /**< Array mapping record descriptors to constructors */

            /**
             * Registers a new record type under the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to register under
             * \param callback Constructor callback
             */
            inline void registerRecordFactory_(descriptors::encoded::Descriptor desc,
                                               const Callback& callback) {
                try {
                    auto& cur_callback = factory_callbacks_[static_cast<size_t>(descriptors::conversion::toInternal(desc))];
                    stf_assert(!cur_callback, "Attempted to re-register factory for descriptor " << desc);
                    cur_callback = callback;
                }
                catch(const std::out_of_range&) {
                    invalid_descriptor_throw("Attempted to register factory for invalid descriptor " << desc);
                }
            }

            /**
             * Gets RecordFactory singleton
             */
            __attribute__((always_inline))
            inline static RecordFactory& get_() {
                static RecordFactory instance;
                return instance;
            }

            /**
             * Constructs an instance of the record type associated with the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to construct
             */
            __attribute__((always_inline))
            inline STFRecordConstUniqueHandle construct_(descriptors::encoded::Descriptor desc,
                                                  STFIFstream& strm) {
                try {
                    const auto& callback = factory_callbacks_[static_cast<size_t>(descriptors::conversion::toInternal(desc))];
                    if(STF_EXPECT_FALSE(!callback)) {
                        invalid_descriptor_throw("Attempted to construct unregistered descriptor: " << desc);
                    }
                    return callback(strm);
                }
                catch(const std::out_of_range&) {
                    invalid_descriptor_throw("Attempted to construct invalid descriptor: " << desc);
                }
            }

            RecordFactory() = default;
            RecordFactory(const RecordFactory&) = default;
            ~RecordFactory() = default;

        public:
            /**
             * Constructs an instance of the record type associated with the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to construct
             * \param strm Stream to extract data from
             */
            __attribute__((always_inline))
            inline static STFRecordConstUniqueHandle construct(descriptors::encoded::Descriptor desc,
                                                        STFIFstream& strm) {
                return get_().construct_(desc, strm);
            }

    };

} // end namespace stf

#endif
