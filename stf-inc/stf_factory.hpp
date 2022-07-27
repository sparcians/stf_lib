#ifndef __STF_PROTOCOL_FACTORY_HPP__
#define __STF_PROTOCOL_FACTORY_HPP__

#include <memory>
#include <array>

namespace stf {
    class STFIFstream;

    namespace FactoryRegistrations {
        /**
         * \class FactoryRegistration
         *
         * Class that auto-registers a type into a factory
         *
         */
        template<typename T> class FactoryRegistration;
    } // end namespace FactoryRegistrations

    /**
     * \class Factory
     *
     * Generic factory class that constructs objects from enums.
     *
     */
    template<typename Enum, size_t NUM_TYPES, typename FactoryObject, typename ConstUniqueHandle>
    class Factory {
        private:
            template<typename T>
            friend class FactoryRegistrations::FactoryRegistration;

            /**
             * \typedef Callback
             * type of callback function used to construct a registered record type
             */
            using Callback = ConstUniqueHandle(*)(STFIFstream& strm);

            std::array<Callback, NUM_TYPES> factory_callbacks_; /**< Array mapping record descriptors to constructors */

            inline static size_t convertToIndex_(const Enum object_id) {
                return static_cast<size_t>(object_id);
            }

            /**
             * Registers a new record type under the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to register under
             * \param callback Constructor callback
             */
            inline void registerRecordFactory_(const Enum object_id,
                                               const Callback& callback) {
                try {
                    auto& cur_callback = factory_callbacks_[convertToIndex_(object_id)];
                    stf_assert(!cur_callback, "Attempted to re-register factory for " << object_id);
                    cur_callback = callback;
                }
                catch(const std::out_of_range&) {
                    invalid_descriptor_throw("Attempted to register factory for invalid id " << object_id);
                }
            }

            /**
             * Gets RecordFactory singleton
             */
            __attribute__((always_inline))
            inline static Factory& get_() {
                static Factory instance;
                return instance;
            }

            /**
             * Constructs an instance of the record type associated with the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to construct
             */
            __attribute__((always_inline))
            inline ConstUniqueHandle construct_(const Enum object_id, STFIFstream& strm) {
                try {
                    const auto& callback = factory_callbacks_[convertToIndex_(object_id)];
                    if(STF_EXPECT_FALSE(!callback)) {
                        invalid_descriptor_throw("Attempted to construct unregistered object: " << object_id);
                    }
                    return callback(strm);
                }
                catch(const std::out_of_range&) {
                    invalid_descriptor_throw("Attempted to construct invalid object: " << object_id);
                }
            }

            Factory() = default;
            Factory(const Factory&) = default;
            ~Factory() = default;

        public:
            /**
             * Constructs an instance of the record type associated with the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to construct
             * \param strm Stream to extract data from
             */
            __attribute__((always_inline))
            inline static ConstUniqueHandle construct(const Enum object_id, STFIFstream& strm) {
                return get_().construct_(object_id, strm);
            }

    };

} // end namespace stf

#endif
