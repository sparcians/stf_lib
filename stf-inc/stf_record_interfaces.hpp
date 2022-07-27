#ifndef __STF_RECORD_INTERFACES_HPP__
#define __STF_RECORD_INTERFACES_HPP__

#include "format_utils.hpp"
#include "stf_object.hpp"
#include "stf_record.hpp"
#include "stf_serializable_container.hpp"

namespace stf {
    /**
     * \class TypeAwareSTFRecord
     *
     * STFRecord that can introspect its own type
     *
     */
    template<typename T>
    class TypeAwareSTFRecord : public STFRecord, public TypeAwareSTFObject<TypeAwareSTFRecord<T>, STFRecord::id_type> {
        private:
            /**
             * Packs a TypeAwareSTFRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack(STFOFstream& writer) const final {
                static_cast<const T*>(this)->pack_impl(writer);
            }

        protected:
            TypeAwareSTFRecord() :
                STFRecord(getTypeId())
            {
            }

        public:
            using type_aware_object = TypeAwareSTFObject<TypeAwareSTFRecord<T>, STFRecord::id_type>;
            using type_aware_object::getTypeId;

            /**
             * Makes a copy of this record
             */
            STFRecord::UniqueHandle clone() const final {
                return STFRecordPool::construct<T>(*static_cast<const T*>(this));
            }

            inline ~TypeAwareSTFRecord() override {
                static_assert(std::is_base_of<TypeAwareSTFRecord<T>, T>::value,
                              "Template parameter passed to TypeAwareSTFRecord must inherit from TypeAwareSTFRecord");
            }

            /**
             * Formats a record into an ostream
             * \param os ostream to use
             */
            void format(std::ostream& os) const final {
                os << getDescriptor() << ' ';
                static_cast<const T*>(this)->format_impl(os);
            }
    };

    /**
     * \class GenericEmptyRecord
     *
     * Generic class for data-less records
     *
     */
    template<typename T>
    class GenericEmptyRecord : public TypeAwareSTFRecord<T> {
        protected:
            GenericEmptyRecord() = default;

        public:
            /**
             * Stub pack method - record is empty so there's nothing to write
             * \param writer writer to use
             */
            // cppcheck-suppress constParameter
            inline void pack_impl(STFOFstream& writer) const { (void)writer; }

            /**
             * Stub unpack method - record is empty so there's nothing to read
             * \param reader reader to use
             */
            // cppcheck-suppress constParameter
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) { (void)reader; }

            /**
             * Stub format method - record is empty so there's nothing to format
             * \param os ostream to use
             */
            // cppcheck-suppress constParameter
            inline void format_impl(std::ostream& os) const { (void)os; }
    };

    /**
     * \class GenericSingleDataRecord
     *
     * Generic record used to hold a single pience of data
     *
     */
    template<typename ClassT, typename DataT>
    class GenericSingleDataRecord : public TypeAwareSTFRecord<ClassT> {
        private:
            DataT data_ = DataT(); /**< underlying data */

        protected:
            GenericSingleDataRecord() = default;

            /**
             * Constructs a GenericSingleDataRecord
             * \param data data for the record
             */
            explicit GenericSingleDataRecord(DataT data) :
                data_(data)
            {
            }

            /**
             * Gets the underlying data
             */
            DataT getData_() const {
                return data_;
            }

            /**
             * Sets the underlying data
             */
            void setData_(DataT data) {
                data_ = data;
            }

        public:
            /**
             * Packs the record into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                TypeAwareSTFRecord<ClassT>::write_(writer, data_);
            }

            /**
             * Unpacks the record from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                TypeAwareSTFRecord<ClassT>::read_(reader, data_);
            }

            /**
             * Formats the record into an ostream
             * \param os ostream to use
             */
            template<typename T = DataT>
            inline typename std::enable_if<std::is_enum<T>::value>::type
            format_impl(std::ostream& os) const {
                os << data_;
            }

            /**
             * Formats the record into an ostream
             * \param os ostream to use
             */
            template<typename T = DataT>
            inline typename std::enable_if<!std::is_enum<T>::value>::type
            format_impl(std::ostream& os) const {
                format_utils::formatHex(os, data_);
            }
    };

    /**
     * \class SerializableVectorRecord
     *
     * Generic record that holds a SerializableVector
     *
     */
    template<typename ClassT, typename VectorDataT, typename VectorSizeT>
    class SerializableVectorRecord : public TypeAwareSTFRecord<ClassT> {
        private:
            SerializableVector<VectorDataT, VectorSizeT> data_;

        protected:
            SerializableVectorRecord() = default;

            /**
             * Constructs a SerializableVectorRecord from a vector of data
             */
            explicit SerializableVectorRecord(std::vector<VectorDataT> data) :
                data_(std::move(data))
            {
            }

        public:
            /**
             * Gets the data
             */
            const SerializableVector<VectorDataT, VectorSizeT>& getData() const {
                return data_;
            }

            /**
             * Packs the record into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                TypeAwareSTFRecord<ClassT>::write_(writer, data_);
            }

            /**
             * Unpacks the record from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                TypeAwareSTFRecord<ClassT>::read_(reader, data_);
            }

            /**
             * Formats the record to an ostream
             * \param os ostream to use
             */
            template<typename T = VectorDataT>
            inline void format_impl(std::ostream& os) const {
                os << data_;
            }
    };

    /**
     * \class GenericAddressRecord
     *
     * Generic record that holds an address
     *
     */
    template <typename T>
    class GenericAddressRecord : public GenericSingleDataRecord<T, uint64_t> {
        protected:
            GenericAddressRecord() = default;

            /**
             * Constructs a GenericAddressRecord
             * \param addr address
             */
            explicit GenericAddressRecord(uint64_t addr) :
                GenericSingleDataRecord<T, uint64_t>(addr)
            {
            }

        public:
            /**
             * Gets the address
             */
            uint64_t getAddr() const { return GenericSingleDataRecord<T, uint64_t>::getData_(); }
    };

    /**
     * \class GenericPCTargetRecord
     *
     * Generic record that holds an instruction PC target address
     *
     */
    template<typename T>
    class GenericPCTargetRecord : public GenericAddressRecord<T> {
        protected:
            GenericPCTargetRecord() = default;

            /**
             * Constructs a GenericPCTargetRecord
             * \param addr Target address
             */
            explicit GenericPCTargetRecord(uint64_t addr) :
                GenericAddressRecord<T>(addr)
            {
            }

        public:
            /**
             * Packs an InstPCTargetRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                writer.trackPC(*static_cast<const T*>(this));
                GenericAddressRecord<T>::pack_impl(writer);
            }

            /**
             * Unpacks an InstPCTargetRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                GenericAddressRecord<T>::unpack_impl(reader);
                reader.trackPC(*static_cast<T*>(this));
            }
    };

    /**
     * \class GenericOpcodeRecord
     *
     * Defines an instruction opcode
     *
     */
    template <typename ClassT, typename OpcodeT>
    class GenericOpcodeRecord : public GenericSingleDataRecord<ClassT, OpcodeT> {
        private:
            uint64_t pc_ = 0;

        protected:
            GenericOpcodeRecord() = default;

            /**
             * Constructs an InstOpcode32Record
             * \param opcode opcode
             */
            explicit GenericOpcodeRecord(const OpcodeT opcode) :
                GenericSingleDataRecord<ClassT, OpcodeT>(opcode)
            {
            }

        public:
            /**
             * Gets the opcode
             */
            OpcodeT getOpcode() const { return GenericSingleDataRecord<ClassT, OpcodeT>::getData_(); }

            /**
             * Gets the PC
             */
            uint64_t getPC() const {
                return pc_;
            }

            /**
             * Packs a GenericOpcodeRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                writer.trackPC(*static_cast<const ClassT*>(this));
                GenericSingleDataRecord<ClassT, OpcodeT>::pack_impl(writer);
                writer.instructionRecordCallback();
            }

            /**
             * Unpacks a GenericOpcodeRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                GenericSingleDataRecord<ClassT, OpcodeT>::unpack_impl(reader);
                reader.trackPC(*static_cast<ClassT*>(this));
                pc_ = reader.getPC();
                reader.instructionRecordCallback();
            }

            /**
             * Gets the size of the opcode
             */
            static constexpr size_t getOpcodeSize() {
                return sizeof(OpcodeT);
            }
    };

} // end namespace stf

#endif
