#ifndef __STF_FSTREAM_HPP__
#define __STF_FSTREAM_HPP__

#include <ios>
#include <string_view>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mutex>
#include <set>
#include <vector>

#include "stf_exception.hpp"
#include "stf_pc_tracker.hpp"
#include "stf_protocol_id.hpp"
#include "stf_vlen.hpp"

/**
 * \macro STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK
 *
 * This macro must be invoked before calling the close_() method in an STFFstream-derived class
 */
#define STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK() std::lock_guard<std::mutex> l(STFFstream::open_close_mutex_)

namespace stf {
    /**
     * \class STFFstream
     *
     * Base class that all STF I/O classes inherit from
     */
    class STFFstream {
        private:
            bool used_popen_ = false; /**< Indicates whether we opened the file with an external command */
            static std::mutex open_streams_mutex_; /**< Mutex used to ensure atexit handler is thread-safe */
            static bool lock_open_streams_; /**< Flag used to indicate whether close() methods should also remove STFFstreams from open_streams_ */
            static std::set<STFFstream*> open_streams_; /**< Set containing all streams that need to be closed in the atexit handler */
            static bool atexit_handler_registered_; /**< Flag used to indicate whether atexit handler has already been registered */
            vlen_t vlen_ = 0; /**< Vector vlen parameter - if 0, the parameter has not been set and
                                   attempting to read/write a vector register record will cause an error */
            protocols::ProtocolId protocol_id_ = protocols::ProtocolId::__RESERVED_END;

        protected:
            FILE* stream_ = nullptr; /**< Underlying file stream */
            PCTracker pc_tracker_; /**< Tracks the current and next instruction PC */
            size_t num_records_read_ = 0; /**< Number of records seen so far */
            size_t num_marker_records_ = 0; /**< Number of marker records seen so far */
            bool has_32bit_events_ = false; /**< If true, EventRecord event values are packed into 32 bits */
            std::mutex open_close_mutex_; /**< Ensures open() and close() are not called from more than 1 thread simultaneously */

            STFFstream() = default;

            /**
             * Checks whether the file extension matches the given extension
             * \param filename filename to check
             * \param ext extension to check for
             */
            static inline bool check_extension_(const std::string_view filename, const std::string_view ext) { // cppcheck-suppress passedByValue
                return filename.rfind(ext) != std::string_view::npos;
            }

            /**
             * Popens the given command on the given filename, in read or write mode
             * \param cmd command to run
             * \param filename filename to run command with
             * \param mode read-write mode
             */
            static inline FILE* popen_cmd_(const std::string_view cmd, const std::string_view filename, const char* mode) { // cppcheck-suppress passedByValue
                std::stringstream ss;
                ss << cmd << filename;
                return popen(ss.str().c_str(), mode);
            }

            /**
             * Calls fstat() on the stream and returns the result
             */
            inline struct stat getFileStat_() const {
                stf_assert(!used_popen_, "Cannot get block size of a stream input.");
                stf_assert(stream_, "Attempted to query blocksize without opening a file first.");

                struct stat stat_result;
                stf_assert(fstat(fileno(stream_), &stat_result) == 0, "Failed to stat file");

                return stat_result;
            }

            /**
             * Gets the block size of the filesystem we are reading from
             */
            inline size_t getFSBlockSize_() const {
                struct stat stat_result = getFileStat_();
                return static_cast<size_t>(stat_result.st_blksize);
            }

            /**
             * atexit handler that cleanly closes all open streams
             */
            static void cleanupStreams_();

            /**
             * Registers this instance with the atexit handler
             */
            inline void registerExitHandler_() {
                // Ensuring that open_streams_mutex_, lock_open_streams_, and open_streams_
                // are instantiated before we register the atexit handler
                std::lock_guard<std::mutex> l(open_streams_mutex_);
                lock_open_streams_ = false;
                open_streams_.insert(this);

                // Register the handler if it hasn't already been registered
                if(!atexit_handler_registered_) {
                    stf_assert(atexit(cleanupStreams_) == 0,
                               "Failed to register exit handler for STFFstream");
                    atexit_handler_registered_ = true;
                }
            }

            /**
             * Virtual method that does the actual work of closing a file.
             * WARNING: This function should not be called unless the open/close mutex has been locked with STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK
             * This can be overridden by subclasses, but note that any subclass that overrides it must also override the destructor.
             */
            inline virtual int close_() {
                int retcode = 0;
                if(stream_) {
                    // Always flush the stream
                    fflush(stream_);

                    if(stream_ != stdin && stream_ != stdout) { // don't close stdin/stdout
                        if (used_popen_) {
                            retcode = pclose (stream_);
                        }
                        else if (stream_ != stdout) {
                            retcode = fclose (stream_);
                        }
                    }
                    stream_ = nullptr;
                }
                num_records_read_ = 0;
                num_marker_records_ = 0;

                // If we aren't closing this from the atexit handler, go ahead and remove ourselves
                // from open_streams_
                if(!lock_open_streams_) {
                    std::lock_guard<std::mutex> l(open_streams_mutex_);
                    open_streams_.erase(this);
                }
                return retcode;
            }

        public:
            // Prevents copying any STF I/O objects
            STFFstream(const STFFstream&) = delete;
            void operator=(const STFFstream&) = delete;

            // Any class that overrides close_ must also override the destructor!
            // Also note that the open/close mutex must be manually locked with STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK in any overridden destructor
            virtual inline ~STFFstream() {
                STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK();
                STFFstream::close_();
            }

            /**
             * Returns whether the stream is valid
             */
            virtual inline explicit operator bool() const {
                return stream_;
            }

            /**
             * \brief Open a file using an external process through a pipe
             * \param cmd The command to run
             * \param filename The filename to run the command with
             * \param rw_mode R/W mode
             */
            inline void openWithProcess(const std::string_view cmd, const std::string_view filename, const char* rw_mode) { // cppcheck-suppress passedByValue
                stream_ = popen_cmd_(cmd, filename, rw_mode);
                used_popen_ = true;
                stf_assert(stream_, "Failed to run command: " << cmd << ' ' << filename);
                registerExitHandler_();
            }

            /**
             * \brief Open a regular file
             * \param filename The file to open
             * \param rw_mode R/W mode
             */
            inline void open(const std::string_view filename, const std::string_view rw_mode) { // cppcheck-suppress passedByValue
                STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK();

                stf_assert(!stream_, "Stream is already open. Call close() first.");

                // special handling for stdin/stdout
                if(filename.compare("-") == 0) {
                    if(rw_mode.compare("rb") == 0) {
                        stream_ = stdin;
                    }
                    else if(rw_mode.compare("wb") == 0) {
                        stream_ = stdout;
                    }
                    else {
                        stf_throw("Attempted to open stdin/stdout with invalid mode: " << rw_mode);
                    }
                }
                else {
                    stream_ = fopen (filename.data(), rw_mode.data());
                }
                used_popen_ = false;
                stf_assert(stream_, "Failed to open file: " << filename);
                registerExitHandler_();
            }

            /**
             * \brief close the trace reader/writer
             */
            inline int close() {
                STF_FSTREAM_ACQUIRE_OPEN_CLOSE_LOCK();
                return close_();
            }

            /**
             * Gets the current instruction PC
             */
            inline uint64_t getPC() const {
                return pc_tracker_.getPC();
            }

            /**
             * Updates the PC tracker with the given record
             * \param rec Record to update PC tracker with
             */
            template<typename RecordType>
            inline void trackPC(const RecordType& rec) {
                pc_tracker_.track(rec);
            }

            /**
             * Callback for when any STFObject is read from the trace
             */
            template<typename T>
            inline void readCallback() { // cppcheck-suppress functionStatic
            }

            /**
             * Gets how many records have been read/written
             */
            inline size_t getNumRecords() const {
                return num_records_read_;
            }

            /**
             * Callback for marker records
             */
            virtual inline void markerRecordCallback() {
                ++num_marker_records_;
            }

            /**
             * Sets the vlen parameter
             * \param vlen Vlen value to set
             */
            void setVLen(vlen_t vlen);

            /**
             * Gets the vlen parameter
             */
            inline vlen_t getVLen() const {
                return vlen_;
            }

            /**
             * Returns whether the trace uses 32-bit event records
             */
            inline bool has32BitEvents() const {
                return has_32bit_events_;
            }

            /**
             * Sets whether the trace uses 32-bit event records
             * \param has_32bit_events If true, the trace to be read/written uses 32-bit event records
             */
            inline void set32BitEvents(const bool has_32bit_events) {
                has_32bit_events_ = has_32bit_events;
            }

            /**
             * Gets the current number of marker records read from the trace
             */
            inline size_t getNumMarkerRecords() const {
                return num_marker_records_;
            }

            /**
             * Sets the protocol ID for a transaction trace
             * \param protocol_id Protocol ID value to set
             */
            inline void setProtocolId(const protocols::ProtocolId protocol_id) {
                protocol_id_ = protocol_id;
            }

            /**
             * Gets the protocol ID for a transaction trace
             */
            inline protocols::ProtocolId getProtocolId() const {
                return protocol_id_;
            }
    };

    /**
     * Specialization for STFRecord - just counts how many records have been read/written
     */
    template<>
    inline void STFFstream::readCallback<STFRecord>() {
        ++num_records_read_;
    }

} // end namespace stf

#endif
