
// <STF_Reader> -*- HPP -*-

/**
 * \brief This file defines APIs for reading STF files.
 */

#ifndef __STF_READER_HPP__
#define __STF_READER_HPP__

#include <cstdio>
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "stf.hpp"
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_ifstream.hpp"
#include "stf_record.hpp"
#include "stf_reader_writer_base.hpp"

// Compatibility fixes for systems where major() and minor() are defined by an include in <sys/types.h>
#ifdef major
    #undef major
#endif
#ifdef minor
    #undef minor
#endif

namespace stf {
    class VersionRecord;
    class CommentRecord;
    class ISARecord;
    class InstIEMRecord;
    class ForcePCRecord;
    class ProcessIDExtRecord;
    class TraceInfoRecord;
    class TraceInfoFeatureRecord;
    class STFWriter;

    /**
     * \class STFReader
     *
     * Holds internal state of an STF reader instance.
     */
    class STFReader : public STFReaderWriterBase {
        private:
            std::unique_ptr<STFIFstream> stream_;
            STFRecord::ConstHandle<VersionRecord> version_;
            std::vector<STFRecord::ConstHandle<CommentRecord>> header_comments_;
            STFRecord::ConstHandle<ISARecord> isa_;
            STFRecord::ConstHandle<InstIEMRecord> initial_iem_;
            STFRecord::ConstHandle<ForcePCRecord> initial_pc_;
            STFRecord::ConstHandle<ProcessIDExtRecord> initial_process_id_;
            std::vector<STFRecord::ConstHandle<TraceInfoRecord>> trace_info_records_;
            STFRecord::ConstHandle<TraceInfoFeatureRecord> trace_features_;

            /**
             * Reads the STF header
             */
            void readHeader_();

            /**
             * Returns whether the header was valid
             */
            void validateHeader_() const;

            /**
             * Opens the specified file with an STFIFstream
             */
            void initSimpleStreamAndOpen_(std::string_view filename);

            /**
             * Opens the specified file with an external process through an STFIFstream
             */
            void initSimpleStreamAndOpenProcess_(std::string_view cmd, std::string_view filename);

        public:
            STFReader() = default;

            /**
             * Constructs an STFReader and opens the specified file
             * \param filename file to open
             * \param force_single_threaded_stream If true, forces single threaded mode
             */
            explicit STFReader(std::string_view filename, bool force_single_threaded_stream = false);

            /**
             * \brief Check STF and trace versions for compatibility
             * \return true if the STF libray supports the trace; otherwise false
             */
            void checkVersion() const
            {
                stf::checkVersion(major(), minor());
            }

            /**
             * \brief Open the trace reader
             */
            void open(std::string_view filename, bool force_single_threaded_stream = false);

            /**
             * Get major version
             */
            uint32_t major() const;

            /**
             * Get minor version
             */
            uint32_t minor() const;

            /**
             * Gets the initial PC
             */
            uint64_t getInitialPC() const;

            /**
             * Gets the initial IEM
             */
            INST_IEM getInitialIEM() const;

            /**
             * Gets the ISA
             */
            ISA getISA() const;

            /**
             * Gets the initial TGID
             */
            uint32_t getInitialTGID() const;

            /**
             * Gets the initial TID
             */
            uint32_t getInitialTID() const;

            /**
             * Gets the initial ASID
             */
            uint32_t getInitialASID() const;

            /**
             * Closes the file
             */
            int close();

            /**
             * Reads into an STFRecord
             * \param rec Record to read into
             */
            inline STFReader& operator>>(STFRecord::UniqueHandle& rec) {
                stream_->operator>>(rec);
                return *this;
            }

            /**
             * Returns whether the underlying stream is still valid
             */
            explicit operator bool() {
                return stream_ && stream_->operator bool();
            }

            /**
             * Seeks the file by the specified number of instructions
             */
            void seek(const size_t num_instructions) {
                stream_->seek(num_instructions);
            }

            /**
             * Gets the trace info records
             */
            const std::vector<STFRecord::ConstHandle<TraceInfoRecord>>& getTraceInfo() const {
                return trace_info_records_;
            }

            /**
             * Gets the latest trace info record
             */
            const TraceInfoRecord& getLatestTraceInfo() const;

            /**
             * Gets the trace feature record
             */
            const STFRecord::ConstHandle<TraceInfoFeatureRecord>& getTraceFeatures() const {
                return trace_features_;
            }

            /**
             * Returns the number of records read so far
             */
            size_t numRecordsRead() const {
                return stream_->getNumRecords();
            }

            /**
             * Returns the number of instructions read so far
             */
            size_t numInstsRead() const {
                return stream_->getNumInsts();
            }

            /**
             * Copies the header to the specifed STFWriter
             * \param stf_writer STFWriter to use
             */
            void copyHeader(STFWriter& stf_writer) const;

            /**
             * Gets the current PC
             */
            uint64_t getPC() const {
                return stream_->getPC();
            }

            /**
             * Dumps the header to the specified std::ostream
             * \param os ostream to use
             */
            void dumpHeader(std::ostream& os) const;

            /**
             * Gets the vlen parameter for the trace
             */
            vlen_t getVLen() const {
                return stream_->getVLen();
            }
    };
} // end namespace stf
// __STF_READER_HPP__
#endif
