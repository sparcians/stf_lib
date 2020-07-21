#ifndef __STF_RECORD_HPP__
#define __STF_RECORD_HPP__

#include <memory>
#include <type_traits>

#include "stf_descriptor.hpp"
#include "stf_enums.hpp"
#include "stf_ifstream.hpp"
#include "stf_ofstream.hpp"
#include "stf_packed_container.hpp"
#include "stf_record_pointers.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class STFRecord
     *
     * Defines the main STF record data structure
     *
     */
    class STFRecord {
        private:
            const descriptors::internal::Descriptor desc_;

        protected:
            /**
             * Writes an arithmetic data type to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T>
            static inline typename std::enable_if<std::is_arithmetic<T>::value || std::is_enum<T>::value>::type
            write_(STFOFstream& writer, const T data) {
                writer << data;
            }

            /**
             * Writes a generic data type to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename T>
            static inline typename std::enable_if<!(std::is_enum<T>::value || std::is_arithmetic<T>::value)>::type
            write_(STFOFstream& writer, const T& data) {
                writer << data;
            }

            /**
             * Writes an arbitrary collection of trivially-copyable types to an STFOFstream using a PackedContainer
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename... Ts>
            static inline typename std::enable_if<type_utils::are_trivially_copyable<Ts...>::value>::type
            write_(STFOFstream& writer, Ts&&... data) {
                writer << PackedContainer<std::remove_cv_t<std::remove_reference_t<Ts>>...>(std::forward<Ts>(data)...);
            }

            /**
             * Writes an arbitrary collection of non-trivially-copyable types to an STFOFstream
             * \param writer STFOFstream to use
             * \param data Data to write
             */
            template<typename... Ts>
            static inline typename std::enable_if<!type_utils::are_trivially_copyable<Ts...>::value>::type
            write_(STFOFstream& writer, Ts&&... data) {
                (writer << ... << data);
            }

            /**
             * Reads an enum data type from an STFIFstream
             * \param reader STFIFstream to use
             * \param data Data to write
             */
            template<typename T>
            __attribute__((always_inline))
            static inline void read_(STFIFstream& reader, T& data) {
                reader >> data;
            }

            /**
             * Reads an arbitrary collection of trivially-copyable types from an STFIFstream using a PackedContainerView
             * \param reader STFIFstream to use
             * \param data Data to read
             */
            template<typename... Ts>
            __attribute__((always_inline))
            static inline typename std::enable_if<type_utils::are_trivially_copyable<Ts...>::value>::type
            read_(STFIFstream& reader, Ts&&... data) {
                PackedContainerView<std::remove_reference_t<Ts>...> temp;
                reader >> temp;
                temp.unpack(std::forward<Ts>(data)...);
            }

            /**
             * Reads an arbitrary collection of non-trivially-copyable types from an STFIFstream
             * \param reader STFIFstream to use
             * \param data Data to read
             */
            template<typename... Ts>
            __attribute__((always_inline))
            static inline typename std::enable_if<!type_utils::are_trivially_copyable<Ts...>::value>::type
            read_(STFIFstream& reader, Ts&&... data) {
                (reader >> ... >> data);
            }

        public:
            /**
             * \typedef UniqueHandle
             * unique_ptr to a const STFRecord
             */
            using UniqueHandle = STFRecordConstUniqueHandle;

            /**
             * Constructs an STFRecord
             * \param desc Descriptor for the record
             */
            STFRecord(const descriptors::internal::Descriptor desc) :
                desc_(desc)
            {
            }

            virtual inline ~STFRecord() = default;

            /**
             * Returns a reference to this record as the desired type
             */
            template<typename T>
            typename std::enable_if<std::is_base_of<STFRecord, T>::value, T&>::type
            as() {
                return static_cast<T&>(*this);
            }

            /**
             * Returns a reference to this record as the desired type
             */
            template<typename T>
            typename std::enable_if<std::is_base_of<STFRecord, T>::value, const T&>::type
            as() const {
                return static_cast<const T&>(*this);
            }

            /**
             * Gets the descriptor
             */
            descriptors::internal::Descriptor getDescriptor() const {
                return desc_;
            }

            /**:
             * Writes the record to an STFOFstream
             * \note Name chosen to avoid collision in Spike codebase
             */
            virtual void pack(STFOFstream& reader) const = 0;

            /**
             * Creates a copy of the record
             */
            virtual STFRecord::UniqueHandle clone() const = 0;

            /**
             * Returns whether this internal::Descriptor corresponds to an instruction record
             * \param desc internal::Descriptor to check
             */
            static constexpr bool isInstructionRecord(const descriptors::internal::Descriptor desc) {
                return (desc == descriptors::internal::Descriptor::STF_INST_OPCODE16) ||
                       (desc == descriptors::internal::Descriptor::STF_INST_OPCODE32);
            }

            /**
             * Returns whether this encoded::Descriptor corresponds to an instruction record
             * \param desc encoded::Descriptor to check
             */
            static constexpr bool isInstructionRecord(const descriptors::encoded::Descriptor desc) {
                return (desc == descriptors::encoded::Descriptor::STF_INST_OPCODE16) ||
                       (desc == descriptors::encoded::Descriptor::STF_INST_OPCODE32);
            }

            /**
             * Returns whether this is an instruction record
             */
            bool isInstructionRecord() const {
                return isInstructionRecord(getDescriptor());
            }

            /**
             * Formats a record into an ostream
             * \param os ostream to use
             */
            virtual void format(std::ostream& os) const = 0;
    };

    /**
     * Grabs ownership of the specified record, casting it to the desired type in the process
     * \param rec Record to grab ownership of
     */
    template<typename T>
    inline ConstUniqueRecordHandle<T> grabRecordOwnership(STFRecord::UniqueHandle& rec) {
        return pointer_utils::grabOwnership<STFRecord, T>(rec);
    }

    /**
     * Grabs ownership of the specified record, casting it to the desired type in the process
     * \param new_rec Pointer that will gain ownership of the record
     * \param rec Record to grab ownership of
     */
    template<typename T>
    inline void grabRecordOwnership(ConstUniqueRecordHandle<T>& new_rec, STFRecord::UniqueHandle& rec) {
        pointer_utils::grabOwnership<STFRecord, T>(new_rec, rec);
    }

} // end namespace stf

#endif
