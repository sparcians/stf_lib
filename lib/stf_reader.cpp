#include <iostream>
#include <string>
#include <string_view>
#include "stf_compressed_ifstream.hpp"
#include "stf_compressed_ifstream_single_threaded.hpp"
#include "stf_env_var.hpp"
#include "stf_exception.hpp"
#include "stf_reader.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_writer.hpp"
#include "zstd/stf_zstd_decompressor.hpp"

namespace stf {
    STFReader::STFReader(const std::string_view filename, const bool force_single_threaded_stream) {
        open(filename, force_single_threaded_stream);
    }

    void STFReader::initSimpleStreamAndOpen_(const std::string_view filename) {
        stream_ = std::make_unique<STFIFstream>();
        stream_->open(filename);
    }

    void STFReader::initSimpleStreamAndOpenProcess_(const std::string_view cmd, const std::string_view filename) {
        stream_ = std::make_unique<STFIFstream>();
        stream_->openWithProcess(cmd, filename);
    }

    void STFReader::open(const std::string_view filename, const bool force_single_threaded_stream)
    {
        stf_assert(!operator bool(), "Attempted to open STFReader that was already open");

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

    void STFReader::validateHeader_() const {
        stf_assert(isa_, "ISA record is missing from header");
        stf_assert(initial_iem_, "IEM record is missing from header");
        stf_assert(initial_pc_, "FORCE_PC record is missing from header");
        stf_assert(!trace_info_records_.empty(), "TRACE_INFO record missing from header");
        stf_assert(trace_features_, "TRACE_INFO_FEATURE record missing from header");
    }

    void STFReader::readHeader_() {
        bool complete_header = false;
        try {
            STFRecord::UniqueHandle rec;

            try {
                operator>>(rec);

                stf_assert(rec->getId() == descriptors::internal::Descriptor::STF_IDENTIFIER && rec->as<STFIdentifierRecord>().isValid(),
                           "Specified file looks like an STF, but does not have a valid STF_IDENTIFIER record");
            }
            catch(const InvalidDescriptorException&) {
                stf_throw("Specified file is not an STF");
            }

            operator>>(rec);

            stf_assert(rec->getId() == descriptors::internal::Descriptor::STF_VERSION,
                       "The second record must be the version");

            STFRecord::grabOwnership(version_, rec);

            do {
                operator>>(rec);
                switch(rec->getId()) {
                    case descriptors::internal::Descriptor::STF_COMMENT:
                        header_comments_.emplace_back(STFRecord::grabOwnership<CommentRecord>(rec));
                        break;
                    case descriptors::internal::Descriptor::STF_ISA:
                        stf_assert(!isa_, "Header has multiple ISA records");
                        STFRecord::grabOwnership(isa_, rec);
                        break;
                    case descriptors::internal::Descriptor::STF_INST_IEM:
                        stf_assert(!initial_iem_, "Header has multiple IEM records");
                        STFRecord::grabOwnership(initial_iem_, rec);
                        break;
                    case descriptors::internal::Descriptor::STF_FORCE_PC:
                        stf_assert(!initial_pc_, "Header has multiple FORCE_PC records");
                        STFRecord::grabOwnership(initial_pc_, rec);
                        break;
                    case descriptors::internal::Descriptor::STF_TRACE_INFO:
                        trace_info_records_.emplace_back(STFRecord::grabOwnership<TraceInfoRecord>(rec));
                        break;
                    case descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE:
                        stf_assert(!trace_features_, "Header has multiple TRACE_INFO_FEATURE records");
                        STFRecord::grabOwnership(trace_features_, rec);
                        break;
                    case descriptors::internal::Descriptor::STF_PROCESS_ID_EXT:
                        stf_assert(!initial_process_id_, "Header has multiple PROCESS_ID_EXT records");
                        STFRecord::grabOwnership(initial_process_id_, rec);
                        break;
                    case descriptors::internal::Descriptor::STF_VLEN_CONFIG:
                        // This record is handled internally by the STFIFstream
                        break;
                    case descriptors::internal::Descriptor::STF_END_HEADER:
                        complete_header = true;
                        break;
                    case descriptors::internal::Descriptor::STF_IDENTIFIER:
                    case descriptors::internal::Descriptor::STF_VERSION:
                    case descriptors::internal::Descriptor::STF_INST_OPCODE32:
                    case descriptors::internal::Descriptor::STF_INST_OPCODE16:
                    case descriptors::internal::Descriptor::STF_INST_REG:
                    case descriptors::internal::Descriptor::STF_INST_READY_REG:
                    case descriptors::internal::Descriptor::STF_INST_PC_TARGET:
                    case descriptors::internal::Descriptor::STF_EVENT:
                    case descriptors::internal::Descriptor::STF_EVENT_PC_TARGET:
                    case descriptors::internal::Descriptor::STF_INST_MEM_ACCESS:
                    case descriptors::internal::Descriptor::STF_INST_MEM_CONTENT:
                    case descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS:
                    case descriptors::internal::Descriptor::STF_BUS_MASTER_CONTENT:
                    case descriptors::internal::Descriptor::STF_PAGE_TABLE_WALK:
                    case descriptors::internal::Descriptor::STF_INST_MICROOP:
                    case descriptors::internal::Descriptor::STF_TRANSACTION:
                        stf_throw("Encountered unexpected STF record in header: " << rec->getId());
                        break;
                    // These records can't be constructed
                    case descriptors::internal::Descriptor::STF_RESERVED:
                    case descriptors::internal::Descriptor::RESERVED_END:
                        __builtin_unreachable();
                }
            }
            while(!complete_header);
        }
        catch(const EOFException& e) {
            // It's ok if the file ends while we're processing the header, because we're going to check whether it's complete
            (void) e;
        }
        stf_assert(complete_header, "STF ended with an incomplete header!");
        validateHeader_();
    }

    uint32_t STFReader::major() const {
        stf_assert(version_, "Attempted to get version without opening a file");
        return version_->getMajor();
    }

    /**
     * Get minor version
     */
    uint32_t STFReader::minor() const {
        stf_assert(version_, "Attempted to get version without opening a file");
        return version_->getMinor();
    }

    // cppcheck-suppress unusedFunction
    uint64_t STFReader::getInitialPC() const {
        return initial_pc_->getAddr();
    }

    // cppcheck-suppress unusedFunction
    INST_IEM STFReader::getInitialIEM() const {
        return initial_iem_->getMode();
    }

    ISA STFReader::getISA() const {
        return isa_->getISA();
    }

    // cppcheck-suppress unusedFunction
    uint32_t STFReader::getInitialTGID() const {
        return initial_process_id_ ? initial_process_id_->getTGID() : 0;
    }

    // cppcheck-suppress unusedFunction
    uint32_t STFReader::getInitialTID() const {
        return initial_process_id_ ? initial_process_id_->getTID() : 0;
    }

    // cppcheck-suppress unusedFunction
    uint32_t STFReader::getInitialASID() const {
        return initial_process_id_ ? initial_process_id_->getASID() : 0;
    }

    // cppcheck-suppress unusedFunction
    const TraceInfoRecord& STFReader::getLatestTraceInfo() const {
        return *trace_info_records_.back();
    }

    int STFReader::close() {
        version_.reset();
        header_comments_.clear();
        isa_.reset();
        initial_iem_.reset();
        initial_pc_.reset();
        trace_info_records_.clear();
        trace_features_.reset();

        if(stream_) {
            return stream_->close();
        }

        return 0;
    }

    // cppcheck-suppress unusedFunction
    void STFReader::copyHeader(STFWriter& stf_writer) const {
        stf_writer.addHeaderComments(header_comments_);
        stf_writer.setISA(isa_->getISA());
        stf_writer.setHeaderIEM(initial_iem_->getMode());
        stf_writer.setHeaderPC(initial_pc_->getAddr());
        stf_writer.addTraceInfoRecords(trace_info_records_);
        stf_writer.setTraceFeature(trace_features_->getFeatures());

        if(const vlen_t vlen = getVLen()) {
            stf_writer.setVLen(vlen);
        }
    }

    // cppcheck-suppress unusedFunction
    void STFReader::dumpHeader(std::ostream& os) const {
        version_->format(os);
        os << std::endl;
        for(const auto& c: header_comments_) {
            c->format(os);
            os << std::endl;
        }
        isa_->format(os);
        os << std::endl;
        initial_iem_->format(os);
        os << std::endl;
        initial_pc_->format(os);
        os << std::endl;
        for(const auto& i: trace_info_records_) {
            i->format(os);
            os << std::endl;
        }
        trace_features_->format(os);
        os << std::endl;
    }

} // end namespace stf
