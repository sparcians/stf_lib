#ifndef __STF_SERIALIZABLE_CONTAINER_HPP__
#define __STF_SERIALIZABLE_CONTAINER_HPP__

#include <string>
#include <vector>

#include "format_utils.hpp"
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
                ContainerT(rhs)
            {
            }

            /**
             * Writes the container to an STFOFstream
             * \param writer STFOFstream to use
             */
            template<typename U = value_type>
            inline typename std::enable_if<!std::is_trivially_copyable<U>::value>::type
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
            inline typename std::enable_if<std::is_trivially_copyable<U>::value>::type
            pack(STFOFstream& writer) const {
                writer << static_cast<SerializedSizeT>(this->size());
                writer << static_cast<const ContainerT&>(*this);
            }

            /**
             * Reads the container from an STFIFstream
             * \param reader STFIFstream to use
             */
            template<typename U = value_type>
            inline typename std::enable_if<!std::is_trivially_copyable<U>::value>::type
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
            inline typename std::enable_if<std::is_trivially_copyable<U>::value>::type
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
            static inline typename std::enable_if<std::is_enum<U>::value>::type
            formatElement_(std::ostream& os, const T element) {
                os << element;
            }

            template<typename U = T>
            static inline typename std::enable_if<!std::is_enum<U>::value && std::is_integral<U>::value>::type
            formatElement_(std::ostream& os, const T element) {
                stf::format_utils::formatHex(os, element);
            }

            template<typename U = T>
            static inline typename std::enable_if<!std::is_enum<U>::value && !std::is_integral<U>::value>::type
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
                SerializableContainer<std::vector<T>, SerializedSizeT>(rhs)
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
                bool first = true;
                for(const auto v: vec) {
                    if(STF_EXPECT_TRUE(!first)) {
                        os << ", ";
                    }
                    else {
                        first = false;
                    }
                    SerializableVector::formatElement_(os, v);
                }
                os << ']';

                return os;
            }
    };

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
