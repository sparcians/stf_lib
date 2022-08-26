#ifndef __STF_OBJECT_HPP__
#define __STF_OBJECT_HPP__

#include "boost_wrappers/setup.hpp"

#include "stf_ifstream.hpp"
#include "stf_ofstream.hpp"
#include "stf_packed_container.hpp"
#include "stf_pool.hpp"
#include "stf_factory.hpp"
#include "stf_vector_view.hpp"
#include "stf_object_id.hpp"
#include "type_utils.hpp"

namespace stf {
    /**
     * \class STFBaseObject
     *
     * Base class of all STF objects. Provides some methods needed by all derived STF objects along with a template-free way to refer to them.
     */
    class STFBaseObject {
        protected:
            /**
             * Writes an arithmetic data type to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T>
            static inline std::enable_if_t<type_utils::is_arithmetic_or_enum_v<T>>
            write_(STFOFstream& writer, const T data) {
                writer << data;
            }

            /**
             * Writes a generic data type to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T>
            static inline std::enable_if_t<std::negation_v<type_utils::is_arithmetic_or_enum<T>>>
            write_(STFOFstream& writer, const T& data) {
                writer << data;
            }

            /**
             * Writes an arbitrary collection of trivially-copyable types to an STFOFstream using a PackedContainer
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename... Ts>
            static inline std::enable_if_t<type_utils::are_trivially_copyable_v<Ts...>>
            write_(STFOFstream& writer, Ts&&... data) {
                writer << PackedContainer<std::remove_cv_t<std::remove_reference_t<Ts>>...>(std::forward<Ts>(data)...);
            }

            /**
             * Writes an arbitrary collection of non-trivially-copyable types to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename... Ts>
            static inline std::enable_if_t<std::negation_v<type_utils::are_trivially_copyable<Ts...>>>
            write_(STFOFstream& writer, Ts&&... data) {
                (writer << ... << data);
            }

            /**
             * Writes a ConstVectorView to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T>
            static inline void write_(STFOFstream& writer, const ConstVectorView<T>& data) {
                writer << data;
            }

            /**
             * Writes a ConstVectorView to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T>
            static inline void write_(STFOFstream& writer, ConstVectorView<T>&& data) {
                writer << data;
            }

            /**
             * Writes the object pointed to by a std::unique_ptr to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T, typename Deleter>
            static inline void write_(STFOFstream& writer, const std::unique_ptr<T, Deleter>& data) {
                writer << *data;
            }

            /**
             * Reads an enum data type from an STFIFstream
             * \param reader STFIFstream to use
             * \param data Data to write
             */
            template<typename T>
            __attribute__((always_inline))
            static inline size_t read_(STFIFstream& reader, T& data) {
                reader >> data;

                return sizeof(T);
            }

            /**
             * Reads an arbitrary collection of trivially-copyable types from an STFIFstream using a PackedContainerView
             * \param reader STFIFstream to use
             * \param data Data to read
             */
            template<typename... Ts>
            __attribute__((always_inline))
            static inline std::enable_if_t<type_utils::are_trivially_copyable_v<Ts...>, size_t>
            read_(STFIFstream& reader, Ts&&... data) {
                PackedContainerView<std::remove_reference_t<Ts>...> temp;
                reader >> temp;
                temp.unpack(std::forward<Ts>(data)...);

                return temp.size();
            }

            /**
             * Reads an arbitrary collection of non-trivially-copyable types from an STFIFstream
             * \param reader STFIFstream to use
             * \param data Data to read
             */
            template<typename... Ts>
            __attribute__((always_inline))
            static inline std::enable_if_t<std::negation_v<type_utils::are_trivially_copyable<Ts...>>, size_t>
            read_(STFIFstream& reader, Ts&&... data) {
                (reader >> ... >> data);

                return (0 + ... + sizeof(Ts));
            }

            /**
             * Reads a VectorView from an STFIFstream
             * \param reader STFIFstream to use
             * \param data Data to read
             */
            template<typename T>
            __attribute__((always_inline))
            static inline size_t read_(STFIFstream& reader, VectorView<T>& data) {
                reader >> data;

                return data.size();
            }

            /**
             * Reads a VectorView from an STFIFstream
             * \param reader STFIFstream to use
             * \param data Data to read
             */
            template<typename T>
            __attribute__((always_inline))
            static inline size_t read_(STFIFstream& reader, VectorView<T>&& data) {
                reader >> data;

                return data.size();
            }

        public:
            /**:
             * Writes the object to an STFOFstream
             * \note Name chosen to avoid collision in Spike codebase
             */
            virtual void pack(STFOFstream& writer) const = 0;
            virtual inline ~STFBaseObject() = default;
    };

    /**
     * \class STFObject
     *
     * STF object that provides an enum ID value and an interface to the pool used to construct the object.
     */
    template<typename BaseType, typename TypeIdEnum, typename FactoryEnum = TypeIdEnum>
    class STFObject : public STFBaseObject {
        protected:
            const TypeIdEnum id_; /**< Enum value that was used to construct the object */

        public:
            /**
             * \typedef pool_type
             * Pool class used to construct objects that derive from this class
             */
            using pool_type = STFPool<BaseType, TypeIdEnum>;

            /**
             * \typedef factory_type
             * Factory class used to construct objects that derive from this class
             */
            using factory_type = Factory<BaseType>;

            /**
             * \typedef id_type
             * Enum type used to construct objects that derive from this class
             */
            using id_type = TypeIdEnum;

            /**
             * \typedef factory_id_type
             * Enum type used to construct objects that derive from this class
             */
            using factory_id_type = FactoryEnum;

            /**
             * \typedef UniqueHandle
             * unique_ptr to a const STFObject
             */
            using UniqueHandle = typename pool_type::ConstBaseObjectPointer;

            /**
             * \typedef Handle
             * unique_ptr to a derived class of STFObject
             */
            template<typename T>
            using Handle = typename pool_type::template ObjectPointer<T>;

            /**
             * \typedef ConstHandle
             * unique_ptr to a const derived class of STFObject
             */
            template<typename T>
            using ConstHandle = typename pool_type::template ConstObjectPointer<T>;

            /**
             * Constructs an STFObject
             * \param id ID of the object
             */
            explicit STFObject(const TypeIdEnum id) :
                id_(id)
            {
            }

            /**
             * Gets the id that was used to construct this object
             */
            inline TypeIdEnum getId() const {
                return id_;
            }

            /**
             * Formats a object into an ostream
             * \param os ostream to use
             */
            virtual void format(std::ostream& os) const = 0;

            /**
             * Creates a copy of the object
             */
            virtual UniqueHandle clone() const = 0;

            /**
             * Returns a pointer to this object as the desired type
             */
            template<typename T>
            inline std::enable_if_t<std::is_base_of_v<BaseType, T>, T*>
            as_ptr() {
                return static_cast<T*>(this);
            }

            /**
             * Returns a pointer to this object as the desired type
             */
            template<typename T>
            inline std::enable_if_t<std::is_base_of_v<BaseType, T>, const T*>
            as_ptr() const {
                return static_cast<const T*>(this);
            }

            /**
             * Returns a reference to this object as the desired type
             */
            template<typename T>
            inline std::enable_if_t<std::is_base_of_v<BaseType, T>, T&>
            as() {
                return *as_ptr<T>();
            }

            /**
             * Returns a reference to this object as the desired type
             */
            template<typename T>
            inline std::enable_if_t<std::is_base_of_v<BaseType, T>, const T&>
            as() const {
                return *as_ptr<T>();
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param obj Object to grab ownership of
             */
            template<typename T>
            static inline ConstHandle<T> grabOwnership(UniqueHandle& obj) {
                return pool_type::template grabOwnership<T>(obj);
            }

            /**
             * Grabs ownership of the specified object, casting it to the desired type in the process
             * \param new_obj Pointer that will gain ownership of the object
             * \param obj Object to grab ownership of
             */
            template<typename T>
            static inline void grabOwnership(ConstHandle<T>& new_obj, UniqueHandle& obj) {
                pool_type::template grabOwnership<T>(new_obj, obj);
            }

            /**
             * Constructs a record of the specified type, returning it as a Handle
             * \param args Arguments to pass to record constructor
             */
            template<typename T, typename ... Args>
            static inline Handle<T> make(Args&&... args) {
                return pool_type::template make<T>(std::forward<Args>(args)...);
            }
    };

    /**
     * \class TypeAwareSTFObject
     *
     * Mix-in class that makes an STFObject aware of its own type and associates that type with an enum value.
     *
     */
    template<typename Type, typename BaseType, typename BaseType::id_type id_value>
    class TypeAwareSTFObject : public BaseType {
        private:
            using PoolType = typename BaseType::pool_type;
            using IdType = typename BaseType::id_type;
            using FactoryIdType = typename BaseType::factory_id_type;

            static_assert(std::is_base_of_v<STFObject<BaseType, IdType, typename BaseType::factory_id_type>, BaseType>,
                        "BaseType must inherit from STFObject");

            inline void pack(STFOFstream& writer) const final {
                static_cast<const Type*>(this)->writeTraceId(writer);
                static_cast<const Type*>(this)->pack_impl(writer);
            }

        protected:
            using BaseType::getId;
            using BaseType::write_;

            TypeAwareSTFObject() :
                BaseType(getTypeId())
            {
            }

        public:
            /**
             * Returns the enum value associated with Type
             */
            static inline constexpr IdType getTypeId() {
                return id_value;
            }

            /**
             * Writes the object ID to the trace
             * \param writer Writer to use
             */
            static inline void writeTraceId(STFOFstream& writer) {
                write_(writer, ObjectIdConverter::toTrace(getTypeId()));
            }

            /**
             * Helper function that can clone any STFObject
             */
            inline typename PoolType::ConstBaseObjectPointer clone() const final {
                return BaseType::pool_type::template construct<Type>(*static_cast<const Type*>(this));
            }

            /**
             * Constructs a record of the specified type, returning it as a Handle
             * \param args Arguments to pass to record constructor
             */
            template<typename ... Args>
            static inline auto make(Args&&... args) {
                return STFObject<BaseType, IdType>::template make<Type>(std::forward<Args>(args)...);
            }

            /**
             * Clones the object, returning it as a type-specific handle
             */
            inline auto copy() const {
                return make(*static_cast<const Type*>(this));
            }

            /**
             * Formats a record into an ostream
             * \param os ostream to use
             */
            void format(std::ostream& os) const final {
                os << getId() << ' ';
                static_cast<const Type*>(this)->format_impl(os);
            }
    };
} // end namespace stf

#endif
