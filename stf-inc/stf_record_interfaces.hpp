#ifndef __STF_RECORD_INTERFACES_HPP__
#define __STF_RECORD_INTERFACES_HPP__

#include "format_utils.hpp"
#include "stf_object.hpp"
#include "stf_record.hpp"
#include "stf_serializable_container.hpp"

namespace stf {
    /**
     * \typedef TypeAwareSTFRecord
     *
     * STFRecord that can introspect its own type
     *
     */
    template<typename T, descriptors::internal::Descriptor desc>
    using TypeAwareSTFRecord = TypeAwareSTFObject<T, STFRecord, desc>;

    /**
     * \class GenericEmptyRecord
     *
     * Generic class for data-less records
     *
     */
    template<typename T, descriptors::internal::Descriptor desc>
    class GenericEmptyRecord : public TypeAwareSTFRecord<T, desc> {
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
    template<typename ClassT, typename DataT, descriptors::internal::Descriptor desc>
    class GenericSingleDataRecord : public TypeAwareSTFRecord<ClassT, desc> {
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
            // cppcheck-suppress duplInheritedMember
            inline void pack_impl(STFOFstream& writer) const {
                TypeAwareSTFRecord<ClassT, desc>::write_(writer, data_);
            }

            /**
             * Unpacks the record from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                TypeAwareSTFRecord<ClassT, desc>::read_(reader, data_);
            }

            /**
             * Formats the record into an ostream
             * \param os ostream to use
             */
            template<typename T = DataT>
            inline std::enable_if_t<std::is_enum_v<T>>
            format_impl(std::ostream& os) const {
                os << data_;
            }

            /**
             * Formats the record into an ostream
             * \param os ostream to use
             */
            template<typename T = DataT>
            inline std::enable_if_t<std::negation_v<std::is_enum<T>>>
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
    template<typename ClassT, typename VectorDataT, typename VectorSizeT, descriptors::internal::Descriptor desc>
    class SerializableVectorRecord : public TypeAwareSTFRecord<ClassT, desc> {
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
                TypeAwareSTFRecord<ClassT, desc>::write_(writer, data_);
            }

            /**
             * Unpacks the record from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                TypeAwareSTFRecord<ClassT, desc>::read_(reader, data_);
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
    template <typename T, descriptors::internal::Descriptor desc>
    class GenericAddressRecord : public GenericSingleDataRecord<T, uint64_t, desc> {
        protected:
            GenericAddressRecord() = default;

            /**
             * Constructs a GenericAddressRecord
             * \param addr address
             */
            explicit GenericAddressRecord(uint64_t addr) :
                GenericSingleDataRecord<T, uint64_t, desc>(addr)
            {
            }

        public:
            /**
             * Gets the address
             */
            uint64_t getAddr() const { return GenericSingleDataRecord<T, uint64_t, desc>::getData_(); }
    };

    /**
     * \class GenericPCTargetRecord
     *
     * Generic record that holds an instruction PC target address
     *
     */
    template<typename T, descriptors::internal::Descriptor desc>
    class GenericPCTargetRecord : public GenericAddressRecord<T, desc> {
        protected:
            GenericPCTargetRecord() = default;

            /**
             * Constructs a GenericPCTargetRecord
             * \param addr Target address
             */
            explicit GenericPCTargetRecord(uint64_t addr) :
                GenericAddressRecord<T, desc>(addr)
            {
            }

        public:
            /**
             * Packs an InstPCTargetRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            // cppcheck-suppress duplInheritedMember
            inline void pack_impl(STFOFstream& writer) const {
                writer.trackPC(*static_cast<const T*>(this));
                GenericAddressRecord<T, desc>::pack_impl(writer);
            }

            /**
             * Unpacks an InstPCTargetRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            // cppcheck-suppress duplInheritedMember
            inline void unpack_impl(STFIFstream& reader) {
                GenericAddressRecord<T, desc>::unpack_impl(reader);
                reader.trackPC(*static_cast<T*>(this));
            }
    };

    /**
     * \class GenericOpcodeRecord
     *
     * Defines an instruction opcode
     *
     */
    template <typename ClassT, typename OpcodeT, descriptors::internal::Descriptor desc>
    class GenericOpcodeRecord : public GenericSingleDataRecord<ClassT, OpcodeT, desc> {
        private:
            uint64_t pc_ = 0;

        protected:
            GenericOpcodeRecord() = default;

            /**
             * Constructs an InstOpcode32Record
             * \param opcode opcode
             */
            explicit GenericOpcodeRecord(const OpcodeT opcode) :
                GenericSingleDataRecord<ClassT, OpcodeT, desc>(opcode)
            {
            }

        public:
            /**
             * Gets the opcode
             */
            OpcodeT getOpcode() const { return GenericSingleDataRecord<ClassT, OpcodeT, desc>::getData_(); }

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
            // cppcheck-suppress duplInheritedMember
            inline void pack_impl(STFOFstream& writer) const {
                writer.trackPC(*static_cast<const ClassT*>(this));
                GenericSingleDataRecord<ClassT, OpcodeT, desc>::pack_impl(writer);
                writer.markerRecordCallback();
            }

            /**
             * Unpacks a GenericOpcodeRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            // cppcheck-suppress duplInheritedMember
            inline void unpack_impl(STFIFstream& reader) {
                GenericSingleDataRecord<ClassT, OpcodeT, desc>::unpack_impl(reader);
                reader.trackPC(*static_cast<ClassT*>(this));
                pc_ = reader.getPC();
                reader.markerRecordCallback();
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
