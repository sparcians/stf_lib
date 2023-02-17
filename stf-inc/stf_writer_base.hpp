#ifndef __STF_WRITER_BASE_HPP__
#define __STF_WRITER_BASE_HPP__

#include <string>
#include <string_view>
#include <vector>

#include "stf_compressed_chunked_base.hpp"
#include "stf_generator.hpp"
#include "stf_ofstream.hpp"
#include "stf_reader_writer_base.hpp"
#include "stf_record.hpp"

namespace stf {
    class CommentRecord;
    class TraceInfoRecord;
    class TraceInfoFeatureRecord;

    /**
     * \class STFWriterBase
     *
     * Base class for all STF writers
     */
    class STFWriterBase : public STFReaderWriterBase<STFOFstream> {
        public:
            static constexpr int DEFAULT_GZIP_COMPRESS = 6; /**< Default GZIP compression level */
            static constexpr int DEFAULT_XZ_COMPRESS = 3;   /**< Default XZ compression level */
            static constexpr size_t DEFAULT_CHUNK_SIZE = STFCompressedChunkedBase::DEFAULT_CHUNK_SIZE; /**< Default compressed trace chunk size */

        protected:
            std::vector<CommentRecord> header_comments_; /**< Header commment records */
            bool header_comments_written_ = false; /**< Set to true when the header comments have been written to the trace */

            std::vector<TraceInfoRecord> trace_info_records_; /**< Trace info records */
            bool trace_info_records_written_ = false; /**< Set to true when the trace info records have been written to the trace */

            STFRecord::Handle<TraceInfoFeatureRecord> trace_features_; /**< Trace feature records */
            bool trace_features_written_ = false; /**< Set to true when the trace feature records have been written to the trace */

            bool header_started_ = false; /**< Set to true when the header has been started */
            bool header_finalized_ = false; /**< Set to true when the header is finalized */

            /**
             * Opens the specified file with an STFOFstream
             */
            void initSimpleStreamAndOpen_(std::string_view filename);

            /**
             * Opens the specified file with an external process through an STFOFstream
             */
            void initSimpleStreamAndOpenProcess_(std::string_view cmd, std::string_view filename);

        public:
            STFWriterBase() = default;

            /**
             * Constructor
             *
             * \param filename The trace file name
             * \param compression_level Compression level to use (-1 for default).
             * \param chunk_size Chunk size to use (Defaults to DEFAULT_CHUNK_SIZE)
             */
            explicit STFWriterBase(const std::string_view filename, // cppcheck-suppress passedByValue
                                   const int compression_level = -1,
                                   const size_t chunk_size = DEFAULT_CHUNK_SIZE);

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
            inline void flush () {
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
            inline void setTraceFeature(const FeatureType trace_feature) {
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
            inline void disableTraceFeature(const FeatureType trace_feature) {
                disableTraceFeature(static_cast<TRACE_FEATURES>(trace_feature));
            }

            /**
             * Removes trace feature from header
             * \param trace_feature feature to remove
             */
            virtual void disableTraceFeature(TRACE_FEATURES trace_feature);

            /**
             * Flushes header
             */
            virtual void flushHeader() = 0;

            /**
             * Finalizes header
             */
            virtual void finalizeHeader();

            /**
             * Returns whether header is finalized
             */
            inline bool headerFinalized() const { return header_finalized_; }

            /**
             * Returns whether header is started
             */
            inline bool headerStarted() const { return header_started_; }

            /**
             * Closes the file
             */
            virtual int close();

            /**
             * Returns the number of records written so far
             */
            inline size_t numRecordsWritten() const {
                return getNumRecords_();
            }

            /**
             * Writes an STFRecord
             * \param rec Record to write
             */
            virtual inline STFWriterBase& operator<<(const STFRecord& rec) {
                *stream_ << rec;
                return *this;
            }
    };
} // end namespace stf

#endif
