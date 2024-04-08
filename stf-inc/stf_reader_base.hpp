#ifndef __STF_READER_BASE_HPP__
#define __STF_READER_BASE_HPP__

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "stf.hpp"
#include "stf_ifstream.hpp"
#include "stf_reader_writer_base.hpp"
#include "stf_record.hpp"

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
    class TraceInfoRecord;
    class TraceInfoFeatureRecord;
    class STFWriterBase;

    /**
     * \class STFReaderBase
     *
     * Base class for all STF readers
     */
    class STFReaderBase : public STFReaderWriterBase<STFIFstream> {
        protected:
            STFRecord::ConstHandle<VersionRecord> version_; /**< Version record */
            std::vector<STFRecord::ConstHandle<CommentRecord>> header_comments_; /**< Header commment records */
            std::vector<STFRecord::ConstHandle<TraceInfoRecord>> trace_info_records_; /**< Trace info records */
            STFRecord::ConstHandle<TraceInfoFeatureRecord> trace_features_; /**< Trace feature records */

            /**
             * Reads the STF header
             */
            virtual void readHeader_() = 0;

            /**
             * Returns whether the header was valid
             */
            virtual void validateHeader_() const;

            /**
             * Opens the specified file with an STFIFstream
             */
            void initSimpleStreamAndOpen_(std::string_view filename);

            /**
             * Opens the specified file with an external process through an STFIFstream
             */
            void initSimpleStreamAndOpenProcess_(std::string_view cmd, std::string_view filename);

        public:
            STFReaderBase() = default;
            ~STFReaderBase();
            STFReaderBase(STFReaderBase&& rhs) = default;
            STFReaderBase& operator=(STFReaderBase&& rhs) = default;

            /**
             * \brief Check STF and trace versions for compatibility
             */
            inline void checkVersion() const {
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
             * Closes the file
             */
            virtual int close();

            /**
             * Reads into an STFRecord
             * \param rec Record to read into
             */
            inline STFReaderBase& operator>>(STFRecord::UniqueHandle& rec) {
                stream_->operator>>(rec);
                return *this;
            }

            /**
             * Seeks the file by the specified number of instructions
             */
            inline void seek(const size_t num_marker_records) {
                stream_->seek(num_marker_records);
            }

            /**
             * Gets the trace info records
             */
            inline const std::vector<STFRecord::ConstHandle<TraceInfoRecord>>& getTraceInfo() const {
                return trace_info_records_;
            }

            /**
             * Gets the latest trace info record
             */
            const TraceInfoRecord& getLatestTraceInfo() const;

            /**
             * Gets the trace feature record
             */
            inline const STFRecord::ConstHandle<TraceInfoFeatureRecord>& getTraceFeatures() const {
                return trace_features_;
            }

            /**
             * Returns the number of records read so far
             */
            inline size_t numRecordsRead() const {
                return getNumRecords_();
            }

            /**
             * Dumps the header to the specified std::ostream
             * \param os ostream to use
             */
            virtual void dumpHeader(std::ostream& os) const = 0;
    };
} // end namespace stf

#endif
