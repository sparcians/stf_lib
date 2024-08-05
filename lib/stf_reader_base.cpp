#include "stf_exception.hpp"
#include "stf_env_var.hpp"
#include "stf_reader_base.hpp"
#include "stf_record_types.hpp"
#include "stf_compressed_ifstream.hpp"
#include "stf_compressed_ifstream_single_threaded.hpp"
#include "zstd/stf_zstd_decompressor.hpp"

namespace stf {
    STFReaderBase::~STFReaderBase() = default;

    void STFReaderBase::validateHeader_() const {
        stf_assert(!trace_info_records_.empty(), "TRACE_INFO record missing from header");
        stf_assert(trace_features_, "TRACE_INFO_FEATURE record missing from header");
    }

    void STFReaderBase::initSimpleStreamAndOpen_(const std::string_view filename) {
        stream_ = std::make_unique<STFIFstream>();
        stream_->open(filename);
    }

    void STFReaderBase::initSimpleStreamAndOpenProcess_(const std::string_view cmd, const std::string_view filename) {
        stream_ = std::make_unique<STFIFstream>();
        stream_->openWithProcess(cmd, filename);
    }

    void STFReaderBase::open(const std::string_view filename, const bool force_single_threaded_stream) {
        stf_assert(!operator bool(), "Attempted to open STFReaderBase that was already open");

        switch(getFileType_(filename)) {
            case STF_FILE_TYPE::ZSTF:
                if(force_single_threaded_stream || STFBooleanEnvVar("STF_SINGLE_THREADED")) {
                    stream_ = std::make_unique<STFCompressedIFstreamSingleThreaded<ZSTDDecompressor>>(filename);
                }
                else {
                    stream_ = std::make_unique<STFCompressedIFstream<ZSTDDecompressor>>(filename);
                }
                break;
            case STF_FILE_TYPE::STF_GZ:
                initSimpleStreamAndOpenProcess_("gzip -dc ", filename);
                break;
            case STF_FILE_TYPE::STF_XZ:
                initSimpleStreamAndOpenProcess_("xz -dc ", filename);
                break;
            case STF_FILE_TYPE::STF_SH:
                initSimpleStreamAndOpenProcess_("sh ", filename);
                break;
            case STF_FILE_TYPE::UNKNOWN:
                std::cerr << "File " << filename << " has an unrecognized extension. Attempting to open as a normal STF." << std::endl; // FALLTHRU
            case STF_FILE_TYPE::STDIO:
            case STF_FILE_TYPE::STF:
                initSimpleStreamAndOpen_(filename);
                break;
        };

        stf_assert(operator bool(), "fail to open trace file " << filename);

        readHeader_();
    }

    uint32_t STFReaderBase::major() const {
        stf_assert(version_, "Attempted to get version without opening a file");
        return version_->getMajor();
    }

    /**
     * Get minor version
     */
    uint32_t STFReaderBase::minor() const {
        stf_assert(version_, "Attempted to get version without opening a file");
        return version_->getMinor();
    }

    // cppcheck-suppress unusedFunction
    const TraceInfoRecord& STFReaderBase::getLatestTraceInfo() const {
        return *trace_info_records_.back();
    }

    const std::vector<std::string>& STFReaderBase::getHeaderCommentsString() {
        if(!header_comments_.empty() && header_comments_str_.empty()) {
            for(const auto& c : header_comments_) {
                header_comments_str_.emplace_back(c->getData());
            }
        }
        return header_comments_str_;
    }

    int STFReaderBase::close() {
        version_.reset();
        header_comments_.clear();
        trace_info_records_.clear();
        trace_features_.reset();

        if(stream_) {
            return stream_->close();
        }

        return 0;
    }

} // end namespace stf
