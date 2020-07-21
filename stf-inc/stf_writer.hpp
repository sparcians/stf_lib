
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
#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_ofstream.hpp"
#include "stf_reader_writer_base.hpp"
#include "stf_reg_def.hpp"
#include "stf_record.hpp"
#include "stf_record_pointers.hpp"
#include "util.hpp"

namespace stf {
    class STFRecord;
    class VersionRecord;
    class CommentRecord;
    class ISARecord;
    class InstIEMRecord;
    class ForcePCRecord;
    class TraceInfoRecord;
    class TraceInfoFeatureRecord;

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

        private:
            std::unique_ptr<STFOFstream> stream_;
            uint32_t init_flags_ = 0;    /**< flags indicate certain records, ie. IEM, PC initialized */
            size_t inst_count_ = 0;    /**< instruction count */
            descriptors::encoded::Descriptor last_desc_ = descriptors::encoded::Descriptor::STF_RESERVED; /**< Last record type written */

            std::vector<CommentRecord> header_comments_;
            bool header_comments_written_ = false;

            UniqueRecordHandle<ISARecord> isa_;
            bool isa_written_ = false;

            UniqueRecordHandle<InstIEMRecord> initial_iem_;
            bool initial_iem_written_ = false;

            UniqueRecordHandle<ForcePCRecord> initial_pc_;
            bool initial_pc_written_ = false;

            std::vector<TraceInfoRecord> trace_info_records_;
            bool trace_info_records_written_ = false;

            UniqueRecordHandle<TraceInfoFeatureRecord> trace_features_;
            bool trace_features_written_ = false;

            bool header_started_ = false;
            bool header_finalized_ = false;
            bool wrote_inst_memory_access_content_pair_ = false;
            bool wrote_bus_memory_access_content_pair_ = false;
            bool wrote_event_record_group_ = false;
            bool wrote_page_table_walk_ = false;
            bool wrote_reg_ = false;

            void updateInitFlags_ (const descriptors::internal::Descriptor desc) {
                if (inst_count_ == 0) {
                    init_flags_ |= (1 << enums::to_int(desc));
                }
            }

            void checkFlagsInitialized_ () const {
                // currently, the following records are required before 1st instruction;
                stf_assert((init_flags_ & (1 << enums::to_int(descriptors::internal::Descriptor::STF_VERSION))) > 0, "Version missing");
                //stf_assert((init_flags_ & (1 << enums::to_int(descriptors::internal::Descriptor::STF_COMMENT))) > 0, "Comment missing");
                stf_assert((init_flags_ & (1 << enums::to_int(descriptors::internal::Descriptor::STF_INST_IEM))) > 0, "IEM missing");
                stf_assert((init_flags_ & (1 << enums::to_int(descriptors::internal::Descriptor::STF_FORCE_PC))) > 0, "Initial PC missing");
                stf_assert((init_flags_ & (1 << enums::to_int(descriptors::internal::Descriptor::STF_TRACE_INFO))) > 0, "Trace info missing");
            }

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
             */
            explicit STFWriter(std::string_view filename, int compression_level = -1);

            /**
             * Opens the specified file for writing
             *
             * \param filename The trace file name
             * \param compression_level Compression level to use (-1 for default).
             */
            void open(std::string_view filename, int compression_level = -1);

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
            void addHeaderComments(const std::vector<ConstUniqueRecordHandle<CommentRecord>>& comments);

            /**
             * Sets header ISA
             * \param isa ISA to set
             */
            void setISA(ISA isa);

            /**
             * Sets header IEM
             * \param iem IEM to set
             */
            void setHeaderIEM(INST_IEM iem);

            /**
             * Sets header PC
             * \param pc PC to set
             */
            void setHeaderPC(uint64_t pc);

            /**
             * Adds trace info to header
             * \param rec trace info to add
             */
            void addTraceInfo(const TraceInfoRecord& rec);

            /**
             * Adds trace info records to header
             * \param records trace info records to add
             */
            void addTraceInfoRecords(const std::vector<ConstUniqueRecordHandle<TraceInfoRecord>>& records);

            /**
             * Adds trace feature to header
             * \param trace_feature feature to add
             */
            void setTraceFeature(uint64_t trace_feature);

            /**
             * Adds trace feature to header
             * \param trace_feature feature to add
             */
            void setTraceFeature(TRACE_FEATURES trace_feature);

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
