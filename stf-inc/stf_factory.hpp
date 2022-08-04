#ifndef __STF_FACTORY_HPP__
#define __STF_FACTORY_HPP__

#include <memory>
#include <array>

#include "stf_factory_decl.hpp"
#include "stf_ifstream.hpp"
#include "stf_object_id.hpp"

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
    template<typename PoolType, typename Enum>
    class Factory {
        private:
            template<typename T>
            friend class FactoryRegistrations::FactoryRegistration;

            using PtrType = typename PoolType::ConstBaseObjectPointer;
            /**
             * \typedef Callback
             * type of callback function used to construct a registered record type
             */
            using Callback = PtrType(*)(STFIFstream& strm);

            enums::EnumArray<Callback, Enum> factory_callbacks_; /**< Array mapping record descriptors to constructors */

            /**
             * Converts an enum into a value that can be used for array lookups. Can be specialized to handle cases where the enum values used in the STF are different from the ones used internally (e.g. STFRecord)
             * \param object_id ID to convert
             */
            static inline size_t convertToIndex_(const Enum object_id) {
                return static_cast<size_t>(ObjectIdConverter::fromTrace(object_id));
            }

            /**
             * Registers a new record type under the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to register under
             * \param callback Constructor callback
             */
            inline void registerObjectFactory_(const Enum object_id,
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
            static inline Factory& get_() {
                static Factory instance;
                return instance;
            }

            /**
             * Constructs an instance of the record type associated with the given descriptors::encoded::Descriptor
             * \param desc descriptors::encoded::Descriptor to construct
             */
            __attribute__((always_inline))
            inline PtrType construct_(STFIFstream& strm) {
                Enum object_id;
                strm >> object_id;

                try {
                    const auto& callback = factory_callbacks_[convertToIndex_(object_id)];
                    if(STF_EXPECT_FALSE(!callback)) {
                        invalid_descriptor_throw("Attempted to construct unregistered object: " << object_id);
                    }
                    auto ptr = callback(strm);
                    strm.readCallback<typename PoolType::base_type>();
                    return ptr;
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
             * \typedef pool_type
             * Pool class used to construct objects for this factory
             */
            using pool_type = PoolType;

            /**
             * \typedef ConstructionIdType
             * Enum type used to look up and construct classes from this factory
             */
            using ConstructionIdType = Enum;

            /**
             * Constructs an STFObject from a trace
             * \param strm Stream to extract data from
             */
            __attribute__((always_inline))
            static inline PtrType construct(STFIFstream& strm) {
                return get_().construct_(strm);
            }
    };

    /**
     * \struct factory_lookup
     *
     * Interface used to get the factory class used by a pool class
     */
    template<typename ObjectType>
    struct factory_lookup {};

} // end namespace stf

/**
 * \def REGISTER_FACTORY_LOOKUP
 *
 */
#define REGISTER_FACTORY_LOOKUP(factory_type, object_type) \
    template<> \
    struct factory_lookup<object_type> { \
        using factory = factory_type; \
    };

/**
 * \def DECLARE_FACTORY
 *
 * Declares a factory typedef and registers the name with the factory_lookup interface
 *
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#define DECLARE_FACTORY(factory_name, object_type, ...) \
    using factory_name = Factory<object_type::pool_type, ## __VA_ARGS__>; \
    REGISTER_FACTORY_LOOKUP(factory_name, object_type)
#pragma clang diagnostic pop

/**
 * \def REGISTER_WITH_FACTORY_NS
 *
 * Registers an STF object under the specified namespace with the factory that will be used to construct it
 *
 */
#define REGISTER_WITH_FACTORY_NS(factory, id_value, ns, cls) \
    namespace FactoryRegistrations { \
        static inline factory::pool_type::ConstBaseObjectPointer cls##_FactoryMethod(STFIFstream& strm) { \
            return factory::pool_type::construct<::ns::cls>(strm); \
        } \
        template<> \
        class FactoryRegistration<::ns::cls> { \
            public: \
                explicit FactoryRegistration(factory::ConstructionIdType) { \
                    factory::get_().registerObjectFactory_(factory::ConstructionIdType::id_value, cls##_FactoryMethod); \
                    factory::pool_type::registerDeleter<::ns::cls>(factory::pool_type::base_type::id_type::id_value); \
                } \
        }; \
        static const FactoryRegistration<::ns::cls> registration_##cls(factory::ConstructionIdType::id_value); \
    } \
    template<> \
    factory::pool_type::base_type::id_type TypeAwareSTFObject<::ns::cls, factory::pool_type::base_type>::getTypeId() { \
        return factory::pool_type::base_type::id_type::id_value; \
    }

/**
 * \def REGISTER_WITH_FACTORY
 *
 * Registers an STF object under the stf namespace with the factory that will be used to construct it
 *
 */
#define REGISTER_WITH_FACTORY(factory, id_value, cls) REGISTER_WITH_FACTORY_NS(factory, id_value, stf, cls)

#endif
