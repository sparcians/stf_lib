
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
    class STFRecord;
    class VersionRecord;
    class CommentRecord;
    class ISARecord;
    class InstIEMRecord;
    class ForcePCRecord;
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
            friend class STFRecord;

            std::unique_ptr<STFIFstream> stream_;
            ConstUniqueRecordHandle<VersionRecord> version_;
            std::vector<ConstUniqueRecordHandle<CommentRecord>> header_comments_;
            ConstUniqueRecordHandle<ISARecord> isa_;
            ConstUniqueRecordHandle<InstIEMRecord> initial_iem_;
            ConstUniqueRecordHandle<ForcePCRecord> initial_pc_;
            std::vector<ConstUniqueRecordHandle<TraceInfoRecord>> trace_info_records_;
            ConstUniqueRecordHandle<TraceInfoFeatureRecord> trace_features_;

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
             */
            explicit STFReader(std::string_view filename);

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
            void open(std::string_view filename);

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
            const std::vector<ConstUniqueRecordHandle<TraceInfoRecord>>& getTraceInfo() const {
                return trace_info_records_;
            }

            /**
             * Gets the trace feature record
             */
            const ConstUniqueRecordHandle<TraceInfoFeatureRecord>& getTraceFeatures() const {
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
             * \param writer STFWriter to use
             */
            void copyHeader(STFWriter& writer) const;

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
    };
} // end namespace stf
// __STF_READER_HPP__
#endif
