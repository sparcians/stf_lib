#ifndef __STF_OFSTREAM_HPP__
#define __STF_OFSTREAM_HPP__

#include <array>
#include <cstring>
#include <memory>
#include <vector>
#include "boost_wrappers/small_vector.hpp"

#include "stf_enum_utils.hpp"
#include "stf_fstream.hpp"
#include "stf_vector_view.hpp"
#include "type_utils.hpp"

namespace stf {
    class STFBaseObject;

    /**
     * \class STFOFstream
     *
     * Base class that supports writing STFRecords to a file
     */
    class STFOFstream : public STFFstream {
        protected:
            /**
             * Virtual method that writes arbitrary buffers to a file
             * Can be overridden for e.g. transparent compression
             * \param data Data to be written
             * \param size Size of a single element in data
             * \param num Number of elements in data
             */
            virtual inline size_t fwrite_(const void* data, size_t size, size_t num) {
                const auto res = fwrite(data, size, num, stream_);
                fflush(stream_);
                return res;
            }

            /**
             * Writes data from an arbitrary pointer
             * \param data Pointer to data
             * \param size Number of elements in data
             */
            template<typename T>
            inline void writeFromPtr_(T* data, size_t size)
            {
                stf_assert(stream_, "Tried to write to an unopened STFOFstream");
                size_t num_written = fwrite_(data, sizeof(T), size);
                stf_assert(num_written == size, "Attempted to write " << size << " objects of size " << sizeof(T) << ", but only wrote " << num_written << ": " << strerror(errno));
            }

            /**
             * Write a single arithmetic value
             * \param val Value to write
             */
            template<typename T>
            inline std::enable_if_t<std::is_arithmetic_v<T>> write_(const T val) {
                writeFromPtr_(&val, 1);
            }

            /**
             * Writes a single enum value
             * \param val Value to write
             */
            template<typename T>
            inline std::enable_if_t<std::is_enum_v<T>> write_(const T val) {
                *this << enums::to_int(val);
            }

            /**
             * Writes an arithmetic data type to an STFOFstream
             * \param data Data to write
             */
            template<typename T>
            std::enable_if_t<type_utils::is_arithmetic_or_enum_v<T>, STFOFstream&> operator<<(const T data) {
                write_<T>(data);
                return *this;
            }

            /**
             * Writes a trivially-copyable data type to an STFOFstream
             * \param data Data to write
             */
            template<typename T>
            inline std::enable_if_t<std::conjunction_v<std::negation<type_utils::is_arithmetic_or_enum<T>>,
                                                                std::is_trivially_copyable<T>>, STFOFstream&>
            operator<<(const T& data) {
                writeFromPtr_(&data, 1);
                return *this;
            }

            /**
             * Writes an object pointed to by a shared_ptr to an STFOFstream
             * \param data Data to write
             */
            template<typename T>
            STFOFstream& operator<<(const std::shared_ptr<T>& data) {
                *this << *data;
                return *this;
            }

            /**
             * Writes an std::array to an STFOFstream
             * \param data Data to write
             */
            template<typename T, std::size_t N>
            STFOFstream& operator<<(const std::array<T, N>& data) {
                writeFromPtr_(data.data(), N);
                return *this;
            }

            /**
             * Writes an std::vector to an STFOFstream
             * \attention The vector size must be written first!
             * Use SerializableVector to handle this automatically.
             * \param data Data to write
             */
            template<typename T>
            STFOFstream& operator<<(const std::vector<T>& data) {
                writeFromPtr_(data.data(), data.size());
                return *this;
            }

            /**
             * Writes a boost::container::small_vector to an STFOFstream
             * \attention The vector size must be written first!
             * Use SerializableVector to handle this automatically.
             * \param data Data to write
             */
            template<typename T, size_t N>
            STFOFstream& operator<<(const boost::container::small_vector<T, N>& data) {
                writeFromPtr_(data.data(), data.size());
                return *this;
            }

            /**
             * Reads a VectorView from an STFIFstream
             * \attention The vector must already be sized to fit the incoming data!
             * Use SerializableVector to handle this automatically.
             * \param data Vector is read into this variable
             */
            template<typename T>
            inline STFOFstream& operator<<(const ConstVectorView<T>& data) {
                writeFromPtr_(data.data(), data.size());
                return *this;
            }

            /**
             * Writes an std::string to an STFOFstream
             * \attention The string size must be written first!
             * Use SerializableString to handle this automatically.
             * \param data Data to write
             */
            STFOFstream& operator<<(const std::string& data);

        public:
            /**
             * Opens a file using an external process
             * \param cmd Command to run
             * \param filename Filename to run the command on
             */
            inline void openWithProcess(const std::string_view cmd, const std::string_view filename) {
                STFFstream::openWithProcess(cmd, filename, "w");
            }

            /**
             * Opens a file
             * \param filename Filename to open
             */
            virtual void open(const std::string_view filename) {
                STFFstream::open(filename, "wb");
            }

            /**
             * \brief Flush the stream
             */
            virtual void flush () {
                fflush(stream_);
            }

            /**
             * Writes an STFRecord
             * \note This is virtual in order to allow subclasses to modify their behavior based on record type
             * \param rec Record to write
             */
            virtual STFOFstream& operator<<(const STFBaseObject& rec);

            friend class STFBaseObject;

            template<typename T, typename SerializedSizeT>
            friend class SerializableContainer;
    };
} // end namespace stf

#endif
