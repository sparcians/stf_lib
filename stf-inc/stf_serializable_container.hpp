#ifndef __STF_SERIALIZABLE_CONTAINER_HPP__
#define __STF_SERIALIZABLE_CONTAINER_HPP__

#include <string>
#include <vector>

#include "format_utils.hpp"
#include "type_utils.hpp"
#include "util.hpp"

namespace stf {
    class STFIFstream;
    class STFOFstream;

    /**
     * \class SerializableContainer
     *
     * Templated class that wraps an STL container. Packs the size of the container into
     * a SerializedSizeT type instead of a size_t to reduce STF size.
     *
     */
    template<typename ContainerT, typename SerializedSizeT>
    class SerializableContainer : public ContainerT
    {
        private:
            using value_type = typename ContainerT::value_type;

        public:
            SerializableContainer() = default;

            /**
             * Copy-constructs a SerializableContainer
             */
            SerializableContainer(const SerializableContainer&) = default;

            /**
             * Move-constructs a SerializableContainer
             */
            SerializableContainer(SerializableContainer&&) noexcept = default;

            /**
             * Copy-constructs a SerializableContainer from its underlying container type
             */
            explicit SerializableContainer(const ContainerT& rhs) :
                ContainerT(rhs)
            {
            }

            /**
             * Move-constructs a SerializableContainer from its underlying container type
             */
            explicit SerializableContainer(ContainerT&& rhs) :
                ContainerT(std::move(rhs))
            {
            }

            /**
             * Writes the container to an STFOFstream
             * \param writer STFOFstream to use
             */
            template<typename U = value_type>
            inline std::enable_if_t<std::negation_v<std::is_trivially_copyable<U>>>
            pack(STFOFstream& writer) const {
                writer << static_cast<SerializedSizeT>(this->size());
                for(const auto& v: *this) {
                    v.pack(writer);
                }
            }

            /**
             * Writes the container to an STFOFstream
             * \param writer STFOFstream to use
             */
            template<typename U = value_type>
            inline std::enable_if_t<std::is_trivially_copyable_v<U>>
            pack(STFOFstream& writer) const {
                writer << static_cast<SerializedSizeT>(this->size());
                writer << static_cast<const ContainerT&>(*this);
            }

            /**
             * Reads the container from an STFIFstream
             * \param reader STFIFstream to use
             */
            template<typename U = value_type>
            inline std::enable_if_t<std::negation_v<std::is_trivially_copyable<U>>>
            unpack(STFIFstream& reader) {
                SerializedSizeT size;
                reader >> size;
                this->clear();
                this->reserve(size);
                for(size_t i = 0; i < size; ++i) {
                    this->emplace_back(reader);
                }
            }

            /**
             * Reads the container from an STFIFstream
             * \param reader STFIFstream to use
             */
            template<typename U = value_type>
            inline std::enable_if_t<std::is_trivially_copyable_v<U>>
            unpack(STFIFstream& reader) {
                SerializedSizeT size;
                reader >> size;
                this->resize(size);
                reader >> static_cast<ContainerT&>(*this);
            }

            /**
             * Writes the container to an STFOFstream
             * \param writer STFOFstream to use
             * \param data SerializableContainer to use
             */
            friend STFOFstream& operator<<(STFOFstream& writer, const SerializableContainer& data) {
                data.pack(writer);
                return writer;
            }

            /**
             * Reads the container from an STFIFstream
             * \param reader STFIFstream to use
             * \param data SerializableContainer to use
             */
            friend STFIFstream& operator>>(STFIFstream& reader, SerializableContainer& data) {
                data.unpack(reader);
                return reader;
            }

            /**
             * Copy assignment operator
             * \param rhs Value to assign
             */
            SerializableContainer& operator=(const SerializableContainer& rhs) = default;

            /**
             * Copy assignment operator
             * \param rhs Value to assign
             */
            SerializableContainer& operator=(const ContainerT& rhs) {
                ContainerT::operator=(rhs);
                return *this;
            }
    };

    /**
     * \class SerializableVector
     *
     * std::vector wrapped by SerializableContainer. Includes a convenient std::ostream operator.
     */
    template<typename T, typename SerializedSizeT>
    class SerializableVector : public SerializableContainer<std::vector<T>, SerializedSizeT> {
        private:
            template<typename U = T>
            static inline std::enable_if_t<std::is_enum_v<U>>
            formatElement_(std::ostream& os, const T element) {
                os << element;
            }

            template<typename U = T>
            static inline std::enable_if_t<std::conjunction_v<std::negation<std::is_enum<U>>, std::is_integral<U>>>
            formatElement_(std::ostream& os, const T element) {
                stf::format_utils::formatHex(os, element);
            }

            template<typename U = T>
            static inline std::enable_if_t<std::negation_v<type_utils::is_integral_or_enum<U>>>
            formatElement_(std::ostream& os, const T element) {
                os << element;
            }

        public:
            SerializableVector() = default;

            /**
             * Copy-constructs a SerializableVector from its underlying type
             */
            explicit SerializableVector(const std::vector<T>& rhs) :
                SerializableContainer<std::vector<T>, SerializedSizeT>(rhs)
            {
            }

            /**
             * Move-constructs a SerializableVector from its underlying container type
             */
            explicit SerializableVector(std::vector<T>&& rhs) :
                SerializableContainer<std::vector<T>, SerializedSizeT>(std::forward<std::vector<T>>(rhs))
            {
            }

            /**
             * Constructs a SerializableVector from an initializer list
             */
            explicit SerializableVector(std::initializer_list<T> rhs) :
                SerializableContainer<std::vector<T>, SerializedSizeT>(std::vector<T>(rhs))
            {
            }

            using SerializableContainer<std::vector<T>, SerializedSizeT>::operator=;

            /**
             * Formats a SerializableVector to an std::ostream
             * \param os ostream to use
             * \param vec SerializableVector to format
             */
            friend std::ostream& operator<<(std::ostream& os, const SerializableVector& vec) {
                os << '[';

                if(auto it = vec.begin(); it != vec.end()) {
                    SerializableVector::formatElement_(os, *it);

                    for(++it; it != vec.end(); ++it) {
                        os << ", ";
                        SerializableVector::formatElement_(os, *it);
                    }
                }

                os << ']';

                return os;
            }
    };

    /**
     * \class SerializablePackedBitVector
     *
     * Type that holds single bit values in single T-type elements, but transparently packs those bits
     * together into uint8_t elements when writing the vector to an STF
     */
    template<typename T, typename SerializedSizeT>
    class SerializablePackedBitVector : public std::vector<T> {
        private:
            inline std::vector<uint8_t> allocatePackedMask_() const {
                return std::vector<uint8_t>((this->size() + 7) / 8);
            }

        public:
            SerializablePackedBitVector() = default;

            /**
             * Copy-constructs a SerializablePackedBitVector from its underlying type
             */
            explicit SerializablePackedBitVector(const std::vector<T>& rhs) :
                std::vector<T>(rhs)
            {
            }

            /**
             * Move-constructs a SerializablePackedBitVector from its underlying container type
             */
            explicit SerializablePackedBitVector(std::vector<T>&& rhs) :
                std::vector<T>(std::forward<std::vector<T>>(rhs))
            {
            }

            using std::vector<T>::operator=;

            /**
             * Unpacks a SerializablePackedBitVector object from an STFIFstream
             * \param reader STFIFstream to use
             */
            inline void unpack(STFIFstream& reader) {
                SerializedSizeT new_size;
                reader >> new_size;
                this->resize(new_size);
                auto packed_data = allocatePackedMask_();

                reader >> packed_data;

                // Unpack the mask bits so that each element of the vector contains 1 mask bit
                for(size_t i = 0; i < new_size; ++i) {
                    (*this)[i] = (packed_data[i / 8] >> (i & 7)) & 1;
                }
            }

            /**
             * Packs a SerializablePackedBitVector object to an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack(STFOFstream& writer) const {
                auto packed_data = allocatePackedMask_();

                // Every 8 elements from the vector are packed into a single uint8_t
                for(size_t i = 0; i < this->size(); ++i) {
                    // Ideally, we would do it like this:
                    // packed_data[i / 8] |= static_cast<uint8_t>((mask_[i] & 1) << (i & 7));
                    //
                    // However, some GCC versions can be extra picky about compound assignment
                    // operators, so instead:
                    auto& dest = packed_data[i / 8];
                    dest = static_cast<uint8_t>(dest | (((*this)[i] & 1) << (i & 7)));
                }

                writer << static_cast<SerializedSizeT>(this->size());
                writer << packed_data;
            }

            /**
             * Writes the container to an STFOFstream
             * \param writer STFOFstream to use
             * \param data SerializableContainer to use
             */
            friend STFOFstream& operator<<(STFOFstream& writer, const SerializablePackedBitVector& data) {
                data.pack(writer);
                return writer;
            }

            /**
             * Reads the container from an STFIFstream
             * \param reader STFIFstream to use
             * \param data SerializableContainer to use
             */
            friend STFIFstream& operator>>(STFIFstream& reader, SerializablePackedBitVector& data) {
                data.unpack(reader);
                return reader;
            }
    };

    namespace type_utils {
        template<typename T>
        struct is_serializable_vector : std::false_type {};

        template<typename DataType, typename SizeType>
        struct is_serializable_vector<SerializableVector<DataType, SizeType>> : std::true_type {};

        template<typename T>
        struct is_serializable_packed_bit_vector : std::false_type {};

        template<typename DataType, typename SizeType>
        struct is_serializable_packed_bit_vector<SerializablePackedBitVector<DataType, SizeType>> : std::true_type {};

        template<typename Value>
        using is_array_or_serializable_vector_like = std::disjunction<std::is_array<Value>,
                                                                      type_utils::is_serializable_vector<Value>,
                                                                      type_utils::is_serializable_packed_bit_vector<Value>>;
    }

    /**
     * \class SerializableString
     *
     * std::string wrapped by SerializableContainer
     *
     */
    template<typename SerializedSizeT>
    class SerializableString : public SerializableContainer<std::string, SerializedSizeT> {
        public:
            SerializableString() = default;

            /**
             * Copy-constructs a SerializableString from its underlying type
             */
            explicit SerializableString(const std::string& rhs) :
                SerializableContainer<std::string, SerializedSizeT>(rhs)
            {
            }

            /**
             * Move-constructs a SerializableString from its underlying container type
             */
            explicit SerializableString(std::string&& rhs) :
                SerializableContainer<std::string, SerializedSizeT>(rhs)
            {
            }

            using SerializableContainer<std::string, SerializedSizeT>::operator=;

            /**
             * Formats a SerializableString to an std::ostream
             * \param os ostream to use
             * \param str SerializableString to format
             */
            friend std::ostream& operator<<(std::ostream& os, const SerializableString& str) {
                os << static_cast<std::string>(str);
                return os;
            }
    };

} // end namespace stf

#endif
