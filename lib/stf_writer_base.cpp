#include <cerrno>

#include "stf.hpp"
#include "stf_writer_base.hpp"
#include "stf_record_types.hpp"
#include "stf_compressed_ofstream.hpp"
#include "zstd/stf_zstd_compressor.hpp"

namespace stf {
    STFWriterBase::STFWriterBase(const std::string_view filename, const int compression_level, const size_t chunk_size) {
        open(filename, compression_level, chunk_size);
    }

    void STFWriterBase::initSimpleStreamAndOpen_(const std::string_view filename) {
        stream_ = std::make_unique<STFOFstream>();
        stream_->open(filename);
    }

    void STFWriterBase::initSimpleStreamAndOpenProcess_(const std::string_view cmd, const std::string_view filename) {
        stream_ = std::make_unique<STFOFstream>();
        stream_->openWithProcess(cmd, filename);
    }

    void STFWriterBase::open(const std::string_view filename, int compression_level, const size_t chunk_size) {
        static const STFIdentifierRecord STF_IDENTIFIER_RECORD = STFIdentifierRecord();
        static const VersionRecord CUR_VERSION_RECORD(STF_CUR_VERSION_MAJOR, STF_CUR_VERSION_MINOR);

        stf_assert(!stream_ || !*stream_, "[stf_writer] Attempted to open an STFWriterBase that was already open");

        switch(getFileType_(filename)) {
            case STF_FILE_TYPE::ZSTF:
                if(compression_level == -1) {
                    compression_level = ZSTDCompressor::DEFAULT_COMPRESSION_LEVEL;
                }
                stream_ = std::make_unique<STFCompressedOFstream<ZSTDCompressor>>(filename, chunk_size, compression_level);
                break;
            case STF_FILE_TYPE::STF_GZ:
                if(compression_level == -1) {
                    compression_level = DEFAULT_GZIP_COMPRESS;
                }
                {
                    const std::string gz_cmd = "gzip -" + std::to_string(compression_level) + " > ";
                    initSimpleStreamAndOpenProcess_(gz_cmd.c_str(), filename);
                }
                break;
            case STF_FILE_TYPE::STF_XZ:
                if(compression_level == -1) {
                    compression_level = DEFAULT_XZ_COMPRESS;
                }
                {
                    const std::string xz_cmd = "xz -z -" + std::to_string(compression_level) + " > ";
                    initSimpleStreamAndOpenProcess_(xz_cmd.c_str(), filename);
                }
                break;
            case STF_FILE_TYPE::STDIO:
            case STF_FILE_TYPE::STF:
                initSimpleStreamAndOpen_(filename);
                break;
            case STF_FILE_TYPE::UNKNOWN:
                stf_throw("File " << filename << " has an unrecognized extension.");
                break;
            case STF_FILE_TYPE::STF_SH:
                stf_throw(".sh format is not supported by STFWriterBase");
                break;
        };

        stf_assert(stream_ && *stream_, "[stf_writer] Failed to open " << filename << " errno: " << strerror(errno));

        // Write the STF identifier and version records so they are always the first in a trace
        *this << STF_IDENTIFIER_RECORD;
        *this << CUR_VERSION_RECORD;

        header_started_ = true;
    }

    void STFWriterBase::addHeaderComment(const std::string& comment) {
        header_comments_.emplace_back(comment);
    }

    void STFWriterBase::addHeaderComments(const std::vector<std::string>& comments) {
        for(const auto& c: comments) {
            addHeaderComment(c);
        }
    }

    void STFWriterBase::addHeaderComments(const std::vector<STFRecord::ConstHandle<CommentRecord>>& comments) {
        std::transform(std::begin(comments),
                       std::end(comments),
                       std::back_inserter(header_comments_),
                       [](const auto& c) { return *c; }
        );
    }

    void STFWriterBase::addTraceInfo(const TraceInfoRecord& rec) {
        trace_info_records_.emplace_back(rec);
    }

    void STFWriterBase::addTraceInfo(TraceInfoRecord&& rec) {
        trace_info_records_.emplace_back(std::move(rec));
    }

    void STFWriterBase::addTraceInfo(const STF_GEN generator,
                                 const uint8_t major_version,
                                 const uint8_t minor_version,
                                 const uint8_t minor_minor_version,
                                 const std::string& comment) {
        addTraceInfo(TraceInfoRecord(generator,
                                     major_version,
                                     minor_version,
                                     minor_minor_version,
                                     comment));
    }

    void STFWriterBase::addTraceInfoRecords(const std::vector<STFRecord::ConstHandle<TraceInfoRecord>>& records) {
        for(const auto& rec: records) {
            addTraceInfo(*rec);
        }
    }

    void STFWriterBase::setTraceFeature(const TRACE_FEATURES trace_feature) {
        if(!trace_features_) {
            trace_features_ = STFRecord::make<TraceInfoFeatureRecord>(trace_feature);
        }
        else {
            trace_features_->setFeature(trace_feature);
        }
    }

    void STFWriterBase::disableTraceFeature(const TRACE_FEATURES trace_feature) {
        if(!trace_features_) {
            return;
        }

        trace_features_->disableFeature(trace_feature);
    }

    void STFWriterBase::finalizeHeader() {
        static const EndOfHeaderRecord END_OF_HEADER_RECORD = EndOfHeaderRecord();
        *this << END_OF_HEADER_RECORD;
        header_finalized_ = true;
    }

    int STFWriterBase::close() {
        header_comments_.clear();
        header_comments_written_ = false;
        trace_info_records_.clear();
        trace_info_records_written_ = false;
        trace_features_.reset();
        trace_features_written_ = false;
        header_started_ = false;
        header_finalized_ = false;
        return stream_->close();
    }

} // end namespace stf
