
// <STF_Writer> -*- HPP -*-

/**
 * \brief This file defines an API for reading STF files.
 */

#ifndef __STF_WRITER_HPP__
#define __STF_WRITER_HPP__

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

#include "stf.hpp"
#include "stf_compressed_chunked_base.hpp"
#include "stf_enums.hpp"
#include "stf_generator.hpp"
#include "stf_exception.hpp"
#include "stf_ofstream.hpp"
#include "stf_reader_writer_base.hpp"
#include "stf_reg_def.hpp"
#include "stf_record.hpp"
#include "util.hpp"

namespace stf {
    class VersionRecord;
    class CommentRecord;
    class ISARecord;
    class InstIEMRecord;
    class ForcePCRecord;
    class TraceInfoRecord;
    class TraceInfoFeatureRecord;
    class VLenConfigRecord;

    /**
     * \class STFWriter
     *
     * Holds the internal state of a STF [trace] writer instance.
     * Currently, there are 30 STF records; init_flags_ hold required records for initialization.
     */
    class STFWriter : public STFReaderWriterBase
    {
        public:
            static constexpr int DEFAULT_GZIP_COMPRESS = 6; /**< Default GZIP compression level */
            static constexpr int DEFAULT_XZ_COMPRESS = 3;   /**< Default XZ compression level */
            static constexpr size_t DEFAULT_CHUNK_SIZE = STFCompressedChunkedBase::DEFAULT_CHUNK_SIZE; /**< Default compressed trace chunk size */

        private:
            std::unique_ptr<STFOFstream> stream_;
            uint32_t init_flags_ = 0;    /**< flags indicate certain records, ie. IEM, PC initialized */
            size_t inst_count_ = 0;    /**< instruction count */
            descriptors::encoded::Descriptor last_desc_ = descriptors::encoded::Descriptor::STF_RESERVED; /**< Last record type written */

            std::vector<CommentRecord> header_comments_;
            bool header_comments_written_ = false;

            STFRecord::Handle<ISARecord> isa_;
            bool isa_written_ = false;

            STFRecord::Handle<InstIEMRecord> initial_iem_;
            bool initial_iem_written_ = false;

            STFRecord::Handle<ForcePCRecord> initial_pc_;
            bool initial_pc_written_ = false;

            std::vector<TraceInfoRecord> trace_info_records_;
            bool trace_info_records_written_ = false;

            STFRecord::Handle<TraceInfoFeatureRecord> trace_features_;
            bool trace_features_written_ = false;

            STFRecord::Handle<VLenConfigRecord> vlen_config_;
            bool vlen_config_written_ = false;

            bool header_started_ = false;
            bool header_finalized_ = false;
            bool wrote_inst_memory_access_content_pair_ = false;
            bool wrote_bus_memory_access_content_pair_ = false;
            bool wrote_event_record_group_ = false;
            bool wrote_page_table_walk_ = false;
            bool wrote_reg_ = false;
            bool force_32bit_events_ = false;

            /**
             * Opens the specified file with an STFOFstream
             */
            void initSimpleStreamAndOpen_(std::string_view filename);

            /**
             * Opens the specified file with an external process through an STFOFstream
             */
            void initSimpleStreamAndOpenProcess_(std::string_view cmd, std::string_view filename);

        public:

            STFWriter() = default;

            /**
             * Constructor
             *
             * \param filename The trace file name
             * \param compression_level Compression level to use (-1 for default).
             * \param chunk_size Chunk size to use (Defaults to DEFAULT_CHUNK_SIZE)
             */
            explicit STFWriter(std::string_view filename,
                               int compression_level = -1,
                               size_t chunk_size = DEFAULT_CHUNK_SIZE);

            /**
             * Opens the specified file for writing
             *
             * \param filename The trace file name
             * \param compression_level Compression level to use (-1 for default).
             * \param chunk_size Chunk size to use (Defaults to DEFAULT_CHUNK_SIZE)
             */
            void open(std::string_view filename, int compression_level = -1, size_t chunk_size = DEFAULT_CHUNK_SIZE);

            /**
             * \brief Flush the stream
             */
            void flush ()
            {
                stream_->flush();
            }

            /**
             * Adds comment to the header
             * \param comment comment to add
             */
            void addHeaderComment(const std::string& comment);

            /**
             * Adds comments to the header
             * \param comments comments to add
             */
            void addHeaderComments(const std::vector<std::string>& comments);

            /**
             * Adds comments to the header
             * \param comments comments to add
             */
            void addHeaderComments(const std::vector<STFRecord::ConstHandle<CommentRecord>>& comments);

            /**
             * Sets header ISA
             * \param isa ISA to set
             */
            void setISA(const ISA isa);

            /**
             * Sets header IEM
             * \param iem IEM to set
             */
            void setHeaderIEM(const INST_IEM iem);

            /**
             * Sets header PC
             * \param pc PC to set
             */
            void setHeaderPC(const uint64_t pc);

            /**
             * Adds trace info to header
             * \param rec trace info to add
             */
            void addTraceInfo(const TraceInfoRecord& rec);

            /**
             * Adds trace info to header
             * \param rec trace info to add
             */
            void addTraceInfo(TraceInfoRecord&& rec);

            /**
             * Adds trace info to header
             * \param generator trace generator
             * \param major_version generator major version
             * \param minor_version generator minor version
             * \param minor_minor_version generator minor-minor version
             * \param comment additional comment
             */
            void addTraceInfo(const STF_GEN generator,
                              const uint8_t major_version,
                              const uint8_t minor_version,
                              const uint8_t minor_minor_version,
                              const std::string& comment);

            /**
             * Adds trace info records to header
             * \param records trace info records to add
             */
            void addTraceInfoRecords(const std::vector<STFRecord::ConstHandle<TraceInfoRecord>>& records);

            /**
             * Adds trace feature to header
             * \param trace_feature feature to add
             */
            template<typename FeatureType>
            void setTraceFeature(const FeatureType trace_feature) {
                setTraceFeature(static_cast<TRACE_FEATURES>(trace_feature));
            }

            /**
             * Adds trace feature to header
             * \param trace_feature feature to add
             */
            void setTraceFeature(TRACE_FEATURES trace_feature);

            /**
             * Removes trace feature from header
             * \param trace_feature feature to remove
             */
            template<typename FeatureType>
            void disableTraceFeature(const FeatureType trace_feature) {
                disableTraceFeature(static_cast<TRACE_FEATURES>(trace_feature));
            }

            /**
             * Removes trace feature from header
             * \param trace_feature feature to remove
             */
            void disableTraceFeature(TRACE_FEATURES trace_feature);

            /**
             * Sets the vlen parameter
             * \param vlen vlen value to set
             */
            void setVLen(vlen_t vlen);

            /**
             * Flushes header
             */
            void flushHeader();

            /**
             * Finalizes header
             */
            void finalizeHeader();

            /**
             * Returns whether header is finalized
             */
            bool headerFinalized() const { return header_finalized_; }

            /**
             * Returns whether header is started
             */
            bool headerStarted() const { return header_started_; }

            /**
             * Closes the file
             */
            int close();

            /**
             * Returns the number of records written so far
             */
            size_t numRecordsWritten() const {
                return stream_->getNumRecords();
            }

            /**
             * Returns the number of instructions read so far
             */
            size_t numInstsWritten() const {
                return stream_->getNumInsts();
            }

            /**
             * Writes an STFRecord
             * \param rec Record to write
             */
            STFWriter& operator<<(const STFRecord& rec);

            /**
             * Returns whether the underlying stream is still valid
             */
            explicit operator bool() const {
                return stream_ && *stream_;
            }
        };
} // end namespace stf
// __STF_WRITER_HPP__
#endif
