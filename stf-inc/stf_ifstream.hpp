#ifndef __STF_IFSTREAM_HPP__
#define __STF_IFSTREAM_HPP__

#include <array>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

#include "stf_enum_utils.hpp"
#include "stf_fstream.hpp"
#include "stf_packed_container.hpp"
#include "stf_record_pointers.hpp"
#include "stf_record_pool.hpp"

namespace stf {
    class STFRecord;

    /**
     * \class STFIFstream
     *
     * Base class that supports reading STFRecords from a file
     */
    class STFIFstream : public STFFstream {
        protected:
            /**
             * Virtual method that reads arbitrary buffers from a file
             * Can be overridden for e.g. transparent decompression
             * \param data Buffer for data to be read
             * \param size Size of a single element in data
             * \param num Number of elements in data
             */
            virtual inline size_t fread_(void* data, const size_t size, const size_t num) {
                stf_assert(stream_, "Tried to read from an unopened STFIFstream");
                return fread(data, size, num, stream_);
            }

            /**
             * Reads a single uint8_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            virtual inline size_t fread_u8_(void* data) {
                return STFIFstream::fread_(data, sizeof(uint8_t), 1);
            }

            /**
             * Reads a single uint16_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            virtual inline size_t fread_u16_(void* data) {
                return STFIFstream::fread_u16_(data, 1);
            }

            /**
             * Reads multiple uint16_t's from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            virtual inline size_t fread_u16_(void* data, const size_t num) {
                return STFIFstream::fread_(data, sizeof(uint16_t), num);
            }

            /**
             * Reads a single uint32_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            virtual inline size_t fread_u32_(void* data) {
                return STFIFstream::fread_u32_(data, 1);
            }

            /**
             * Reads multiple uint32_t's from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            virtual inline size_t fread_u32_(void* data, const size_t num) {
                return STFIFstream::fread_(data, sizeof(uint32_t), num);
            }

            /**
             * Reads a single uint64_t from the decompression buffer.
             *
             * \param data Buffer to read data into
             */
            virtual inline size_t fread_u64_(void* data) {
                return STFIFstream::fread_u64_(data, 1);
            }

            /**
             * Reads multiple uint64_t's from the decompression buffer.
             *
             * \param data Buffer to read data into
             * \param num Number of elements to read
             */
            virtual inline size_t fread_u64_(void* data, const size_t num) {
                return STFIFstream::fread_(data, sizeof(uint64_t), num);
            }

            /**
             * Reads a PackedContainerView by allocating and copying into a buffer of the correct size.
             *
             * \param data PackedContainerView to read data into
             * \param size Size of the PackedContainer pointed to by data
             */
            virtual inline size_t freadPackedContainer_(PackedContainerViewBase& data, const size_t size) {
                data.allocateView();
                return STFIFstream::fread_(data.get(), size, 1);
            }

            /**
             * Virtual method that indicates whether we have reached the end of the stream
             */
            virtual inline bool feof_() const {
                return feof(stream_);
            }

            /**
             * Checks whether the file stream is still valid
             */
            inline void checkStream_() const {
                // ferror() is somewhat expensive, so check the value of errno first
                if(STF_EXPECT_FALSE(errno != 0)) {
                    // Get and print the error
                    if (const int e = ferror(stream_)) {
                        stf_throw("Error reading file " << e);
                    }
                }
                // Check for EOF condition
                if (STF_EXPECT_FALSE(feof_())) {
                    throw stf::EOFException();
                }
            }

            /**
             * Reads data into an arbitrary pointer
             * \param data Buffer to read into
             * \param size Number of elements to read
             */
            template<typename T>
            inline void readIntoPtr_(T* data, const size_t size) {
                switch(sizeof(T)) {
                    case sizeof(uint16_t):
                        fread_u16_(data, size);
                        break;
                    case sizeof(uint32_t):
                        fread_u32_(data, size);
                        break;
                    case sizeof(uint64_t):
                        fread_u64_(data, size);
                        break;
                    default:
                        fread_(data, sizeof(T), size);
                        break;
                }
            }

            /**
             * Reads a single data element into an arbitrary pointer
             * \param data Buffer to read into
             */
            template<typename T>
            inline void readIntoPtr_(T* data) {
                switch(sizeof(T)) {
                    case sizeof(uint8_t):
                        fread_u8_(data);
                        break;
                    case sizeof(uint16_t):
                        fread_u16_(data);
                        break;
                    case sizeof(uint32_t):
                        fread_u32_(data);
                        break;
                    case sizeof(uint64_t):
                        fread_u64_(data);
                        break;
                    default:
                        // Check if T is an even multiple of a uint64_t
                        if(sizeof(T) % sizeof(uint64_t) == 0) {
                            fread_u64_(data, sizeof(T) / sizeof(uint64_t));
                        }

                        // Check if T is an even multiple of a uint32_t
                        else if(sizeof(T) % sizeof(uint32_t) == 0) {
                            fread_u32_(data, sizeof(T) / sizeof(uint32_t));
                        }

                        // Check if T is an even multiple of a uint16_t
                        else if(sizeof(T) % sizeof(uint16_t) == 0) {
                            fread_u16_(data, sizeof(T) / sizeof(uint16_t));
                        }

                        // T is an unusual size, so just do a byte-by-byte copy
                        else {
                            readIntoPtr_(data, 1);
                        }
                        break;
                }
            }

            /**
             * Reads an enum value
             */
            template<typename T>
            inline typename std::enable_if<std::is_enum<T>::value, T>::type read_() {
                return static_cast<T>(read_<enums::int_t<T>>());
            }

            /**
             * Reads an arithmetic value
             */
            template<typename T>
            inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type read_() {
                T val = 0;
                readIntoPtr_(&val);
                return val;
            }

            /**
             * Reads an arithmetic or enum data type from an STFIFstream
             * \param data Value is read into this variable
             */
            template<typename T>
            inline typename std::enable_if<std::is_arithmetic<T>::value || std::is_enum<T>::value, STFIFstream&>::type operator>>(T& data) {
                data = read_<T>();
                return *this;
            }

            /**
             * Reads a trivially-copyable data type from an STFIFstream
             * \param data Value is read into this variable
             */
            template<typename T>
            inline typename std::enable_if<!(std::is_arithmetic<T>::value || std::is_enum<T>::value) && std::is_trivially_copyable<T>::value, STFIFstream&>::type operator>>(T& data) {
                readIntoPtr_(&data);
                return *this;
            }

            /**
             * Reads a PackedContainerView from an STFIFstream
             * \param data Value is read into this variable
             */
            template<typename ... Ts>
            inline STFIFstream& operator>>(PackedContainerView<Ts...>& data) {
                freadPackedContainer_(data, PackedContainerView<Ts...>::size());
                return *this;
            }

            /**
             * Reads an object pointed to by a shared_ptr from an STFIFstream
             * \param data Value is read into this variable
             */
            template<typename T>
            inline STFIFstream& operator>>(std::shared_ptr<T>& data) {
                return operator>>(*data);
            }

            /**
             * Reads an std::array from an STFIFstream
             * \param data Array is read into this variable
             */
            template<typename T, std::size_t N>
            inline STFIFstream& operator>>(std::array<T, N>& data) {
                readIntoPtr_(data.data(), N);
                return *this;
            }

            /**
             * Reads an std::vector from an STFIFstream
             * \attention The vector must already be sized to fit the incoming data!
             * Use SerializableVector to handle this automatically.
             * \param data Vector is read into this variable
             */
            template<typename T>
            inline STFIFstream& operator>>(std::vector<T>& data) {
                readIntoPtr_(data.data(), data.size());
                return *this;
            }

            /**
             * Reads an std::string from an STFIFstream
             * \attention The string must already be sized to fit the incoming data!
             * Use SerializableString to handle this automatically.
             * \param data String is read into this variable
             */
            inline STFIFstream& operator>>(std::string& data) {
                readIntoPtr_(data.data(), data.size());
                return *this;
            }

        public:
            /**
             * Opens a file with an external process
             * \param cmd Command to run
             * \param filename Filename to run the command on
             */
            inline void openWithProcess(const std::string_view cmd, const std::string_view filename) {
                STFFstream::openWithProcess(cmd, filename, "r");
            }

            /**
             * Opens a file
             * \param filename Filename to open
             */
            virtual void open(const std::string_view filename) {
                STFFstream::open(filename, "rb");
            }

            friend class STFRecord;
            template<typename T, typename SerializedSizeT>
            friend class SerializableContainer;

            /**
             * Reads an STFRecord
             * \param rec Record is read into this variable
             */
            STFIFstream& operator>>(STFRecordConstUniqueHandle& rec);

            /**
             * Skips the stream forward by the given number of instructions
             * \param num_instructions Number of instructions to skip
             */
            inline virtual void seek(size_t num_instructions) {
                const size_t end_inst_num = num_insts_ + num_instructions;
                STFRecordConstUniqueHandle rec;

                try {
                    while(operator bool() && (num_insts_ < end_inst_num)) {
                        operator>>(rec);
                    }
                }
                catch(const EOFException&) {
                    stf_throw("Attempted to seek past the end of the trace");
                }
            }
    };
} // end namespace stf

#endif
