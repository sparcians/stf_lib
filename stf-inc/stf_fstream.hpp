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
#include "stf_vlen.hpp"

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

        protected:
            FILE* stream_ = nullptr; /**< Underlying file stream */
            PCTracker pc_tracker_; /**< Tracks the current and next instruction PC */
            size_t num_records_read_ = 0; /**< Number of records seen so far */
            size_t num_insts_ = 0; /**< Number of instructions seen so far */
            bool has_32bit_events_ = false; /**< If true, EventRecord event values are packed into 32 bits */

            STFFstream() = default;

            /**
             * Checks whether the file extension matches the given extension
             * \param filename filename to check
             * \param ext extension to check for
             */
            static inline bool check_extension_(const std::string_view filename, const std::string_view ext) {
                return filename.rfind(ext) != std::string_view::npos;
            }

            /**
             * Popens the given command on the given filename, in read or write mode
             * \param cmd command to run
             * \param filename filename to run command with
             * \param mode read-write mode
             */
            static inline FILE* popen_cmd_(const std::string_view cmd, const std::string_view filename, const char* mode) {
                std::stringstream ss;
                ss << cmd << filename;
                return popen(ss.str().c_str(), mode);
            }

            /**
             * Calls fstat() on the stream and returns the result
             */
            struct stat getFileStat_() const {
                stf_assert(!used_popen_, "Cannot get block size of a stream input.");
                stf_assert(stream_, "Attempted to query blocksize without opening a file first.");

                struct stat stat_result;
                stf_assert(fstat(fileno(stream_), &stat_result) == 0, "Failed to stat file");

                return stat_result;
            }

            /**
             * Gets the block size of the filesystem we are reading from
             */
            size_t getFSBlockSize_() const {
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
            void registerExitHandler_() {
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

        public:
            // Prevents copying any STF I/O objects
            STFFstream(const STFFstream&) = delete;
            void operator=(const STFFstream&) = delete;

            virtual ~STFFstream() {
                if (stream_) {
                    STFFstream::close();
                }
            }

            /**
             * Returns whether the stream is valid
             */
            explicit virtual operator bool() const {
                return stream_;
            }

            /**
             * \brief Open a file using an external process through a pipe
             * \param cmd The command to run
             * \param filename The filename to run the command with
             * \param rw_mode R/W mode
             */
            inline void openWithProcess(const std::string_view cmd, const std::string_view filename, const char* rw_mode) {
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
            inline void open(const std::string_view filename, const std::string_view rw_mode) {
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
            virtual int close()
            {
                int retcode = 0;
                if (stream_) {
                    if(stream_ == stdout) {
                        fflush(stream_); // need to manually flush stdout
                    }
                    else if(stream_ != stdin) { // don't close stdin/stdout
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
                num_insts_ = 0;

                // If we aren't closing this from the atexit handler, go ahead and remove ourselves
                // from open_streams_
                if(!lock_open_streams_) {
                    std::lock_guard<std::mutex> l(open_streams_mutex_);
                    open_streams_.erase(this);
                }
                return retcode;
            }

            /**
             * Gets the current instruction PC
             */
            uint64_t getPC() const {
                return pc_tracker_.getPC();
            }

            /**
             * Updates the PC tracker with the given record
             * \param rec Record to update PC tracker with
             */
            template<typename RecordType>
            void trackPC(const RecordType& rec) {
                pc_tracker_.track(rec);
            }

            /**
             * Callback for all record types - just counts how many records have been read/written
             */
            void recordCallback() {
                ++num_records_read_;
            }

            /**
             * Gets how many records have been read/written
             */
            size_t getNumRecords() const {
                return num_records_read_;
            }

            /**
             * Callback for instruction opcode record types - just counts how many instruction record groups have been read/written
             */
            virtual void instructionRecordCallback() {
                // If this was an instruction record, increment the instruction count
                ++num_insts_;
            }

            /**
             * Gets how many instruction record groups have been read/written
             */
            size_t getNumInsts() const {
                return num_insts_;
            }

            /**
             * Sets the vlen parameter
             * \param vlen Vlen value to set
             */
            void setVLen(vlen_t vlen);

            /**
             * Gets the vlen parameter
             */
            vlen_t getVLen() const {
                return vlen_;
            }

            /**
             * Returns whether the trace uses 32-bit event records
             */
            bool has32BitEvents() const {
                return has_32bit_events_;
            }

            /**
             * Sets whether the trace uses 32-bit event records
             * \param has_32bit_events If true, the trace to be read/written uses 32-bit event records
             */
            void set32BitEvents(const bool has_32bit_events) {
                has_32bit_events_ = has_32bit_events;
            }
    };
} // end namespace stf

#endif
