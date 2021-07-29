#include <cerrno>
#include <cstring>
#include <string>
#include <string_view>

#include "stf.hpp"
#include "stf_exception.hpp"
#include "stf_record.hpp"
#include "stf_record_types.hpp"
#include "stf_writer.hpp"
#include "stf_compressed_ofstream.hpp"
#include "zstd/stf_zstd_compressor.hpp"

namespace stf {
    STFWriter::STFWriter(const std::string_view filename, const int compression_level, const size_t chunk_size) {
        open(filename, compression_level, chunk_size);
    }

    void STFWriter::initSimpleStreamAndOpen_(const std::string_view filename) {
        stream_ = std::make_unique<STFOFstream>();
        stream_->open(filename);
    }

    void STFWriter::initSimpleStreamAndOpenProcess_(const std::string_view cmd, const std::string_view filename) {
        stream_ = std::make_unique<STFOFstream>();
        stream_->openWithProcess(cmd, filename);
    }

    void STFWriter::open(const std::string_view filename, int compression_level, const size_t chunk_size) {
        static const STFIdentifierRecord STF_IDENTIFIER_RECORD = STFIdentifierRecord();
        static const VersionRecord CUR_VERSION_RECORD(STF_CUR_VERSION_MAJOR, STF_CUR_VERSION_MINOR);

        stf_assert(!stream_ || !*stream_, "[stf_writer] Attempted to open an STFWriter that was already open");

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
                stf_throw(".sh format is not supported by STFWriter");
                break;
        };

        stf_assert(stream_ && *stream_, "[stf_writer] Failed to open " << filename << " errno: " << strerror(errno));

        // before write any records, initialize the flags and count
        init_flags_ = 0;
        inst_count_ = 0;

        // Write the STF identifier and version records so they are always the first in a trace
        *this << STF_IDENTIFIER_RECORD;
        *this << CUR_VERSION_RECORD;

        header_started_ = true;
    }

    void STFWriter::addHeaderComment(const std::string& comment) {
        header_comments_.emplace_back(comment);
    }

    void STFWriter::addHeaderComments(const std::vector<std::string>& comments) {
        for(const auto& c: comments) {
            addHeaderComment(c);
        }
    }

    void STFWriter::addHeaderComments(const std::vector<ConstUniqueRecordHandle<CommentRecord>>& comments) {
        for(const auto& c: comments) {
            header_comments_.emplace_back(*c);
        }
    }

    void STFWriter::setISA(ISA isa) {
        isa_ = STFRecordPool::make<ISARecord>(isa);
    }

    void STFWriter::setHeaderIEM(INST_IEM iem) {
        initial_iem_ = STFRecordPool::make<InstIEMRecord>(iem);
    }

    void STFWriter::setHeaderPC(uint64_t pc) {
        initial_pc_ = STFRecordPool::make<ForcePCRecord>(pc);
    }

    void STFWriter::addTraceInfo(const TraceInfoRecord& rec) {
        trace_info_records_.emplace_back(rec);
    }

    void STFWriter::addTraceInfo(TraceInfoRecord&& rec) {
        trace_info_records_.emplace_back(std::move(rec));
    }

    void STFWriter::addTraceInfo(const STF_GEN generator,
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

    void STFWriter::addTraceInfoRecords(const std::vector<ConstUniqueRecordHandle<TraceInfoRecord>>& records) {
        for(const auto& rec: records) {
            addTraceInfo(*rec);
        }
    }

    void STFWriter::setTraceFeature(uint64_t trace_feature) {
        if(!trace_features_) {
            trace_features_ = STFRecordPool::make<TraceInfoFeatureRecord>(trace_feature);
        }
        else {
            trace_features_->setFeature(trace_feature);
        }
    }

    void STFWriter::setTraceFeature(TRACE_FEATURES trace_feature) {
        setTraceFeature(enums::to_int(trace_feature));
    }

    void STFWriter::setVLen(const vlen_t vlen) {
        vlen_config_ = STFRecordPool::make<VLenConfigRecord>(vlen);
    }

    void STFWriter::flushHeader() {
        stf_assert(!header_finalized_, "Cannot write anything else to the header after it has been finalized");

        if(!header_comments_written_ && !header_comments_.empty()) {
            stf_assert(!(isa_written_ || initial_iem_written_ || initial_pc_written_ || trace_info_records_written_ || trace_features_written_),
                       "Header comment block must come before any other header records");
            for(const auto& c: header_comments_) {
                *this << c;
            }
            header_comments_written_ = true;
        }

        if(!isa_written_ && isa_) {
            stf_assert(!(initial_iem_written_ || initial_pc_written_ || trace_info_records_written_ || trace_features_written_),
                       "ISA record must come before IEM, FORCE_PC, TRACE_INFO, or TRACE_INFO_FEATURE records");
            *this << *isa_;
            isa_written_ = true;
        }

        if(!initial_iem_written_ && initial_iem_) {
            stf_assert(isa_written_, "ISA record must come before IEM record");
            stf_assert(!(initial_pc_written_ || trace_info_records_written_ || trace_features_written_),
                       "IEM record must come before FORCE_PC, TRACE_INFO, or TRACE_INFO_FEATURE records");
            *this << *initial_iem_;
            initial_iem_written_ = true;
        }

        if(!trace_info_records_written_ && !trace_info_records_.empty()) {
            stf_assert(isa_written_, "ISA record must come before TRACE_INFO record");
            stf_assert(initial_iem_written_, "IEM record must come before TRACE_INFO record");
            stf_assert(!initial_pc_written_, "FORCE_PC record must come before TRACE_INFO record");
            stf_assert(!trace_features_written_, "TRACE_INFO records must come before TRACE_INFO_FEATURE record");
            for(const auto& r: trace_info_records_) {
                *this << r;
            }
            trace_info_records_written_ = true;
        }

        if(!trace_features_written_ && trace_features_) {
            stf_assert(isa_written_, "ISA record must come before TRACE_INFO_FEATURE record");
            stf_assert(initial_iem_written_, "IEM record must come before TRACE_INFO_FEATURE record");
            stf_assert(trace_info_records_written_, "TRACE_INFO records must come before TRACE_INFO_FEATURE record");
            stf_assert(!initial_pc_written_, "FORCE_PC record must come before TRACE_INFO_FEATURE record");
            *this << *trace_features_;
            trace_features_written_ = true;
        }

        if(!initial_pc_written_ && initial_pc_) {
            stf_assert(isa_written_, "ISA record must come before FORCE_PC record");
            stf_assert(initial_iem_written_, "IEM record must come before FORCE_PC record");
            stf_assert(trace_info_records_written_,
                       "TRACE_INFO record must come before FORCE_PC record");
            stf_assert(trace_features_written_,
                       "TRACE_INFO_FEATURE record must come before FORCE_PC record");
            *this << *initial_pc_;
            initial_pc_written_ = true;
        }

        if(!vlen_config_written_ && vlen_config_) {
            *this << *vlen_config_;
            vlen_config_written_ = true;
        }
    }

    void STFWriter::finalizeHeader() {
        static const EndOfHeaderRecord END_OF_HEADER_RECORD = EndOfHeaderRecord();
        if(header_finalized_) {
            return;
        }

        stf_assert(header_started_, "Attempted to finalize the header before anything has been written to it");

        if(!(header_comments_written_ &&
             isa_written_ &&
             initial_iem_written_ &&
             initial_pc_written_ &&
             trace_info_records_written_ &&
             trace_features_written_ &&
             (vlen_config_written_ || !vlen_config_))) { // VLenConfigRecord is optional
            flushHeader();
        }

        *this << END_OF_HEADER_RECORD;

        header_finalized_ = true;
    }

    int STFWriter::close() {
        init_flags_ = 0;
        inst_count_ = 0;
        header_comments_.clear();
        header_comments_written_ = false;
        isa_.reset();
        isa_written_ = false;
        initial_iem_.reset();
        initial_iem_written_ = false;
        initial_pc_.reset();
        initial_pc_written_ = false;
        trace_info_records_.clear();
        trace_info_records_written_ = false;
        trace_features_.reset();
        trace_features_written_ = false;
        header_started_ = false;
        header_finalized_ = false;
        last_desc_ = descriptors::encoded::Descriptor::STF_RESERVED;
        wrote_inst_memory_access_content_pair_ = false;
        wrote_bus_memory_access_content_pair_ = false;
        return stream_->close();
    }

    STFWriter& STFWriter::operator<<(const STFRecord& rec) {
        const auto desc = rec.getDescriptor();
        const auto encoded_desc = descriptors::conversion::toEncoded(desc);
        const bool last_was_memory_access = last_desc_ == descriptors::encoded::Descriptor::STF_INST_MEM_ACCESS;
        const bool last_was_bus_access = last_desc_ == descriptors::encoded::Descriptor::STF_BUS_MASTER_ACCESS;
        const bool cur_is_memory_content = encoded_desc == descriptors::encoded::Descriptor::STF_INST_MEM_CONTENT;
        const bool cur_is_bus_content = encoded_desc == descriptors::encoded::Descriptor::STF_BUS_MASTER_CONTENT;

        stf_assert(!last_was_memory_access || cur_is_memory_content,
                   descriptors::encoded::Descriptor::STF_INST_MEM_CONTENT
                   << " must follow "
                   << descriptors::encoded::Descriptor::STF_INST_MEM_ACCESS);

        stf_assert(!last_was_bus_access || cur_is_bus_content,
                   descriptors::encoded::Descriptor::STF_BUS_MASTER_CONTENT
                   << " must follow "
                   << descriptors::encoded::Descriptor::STF_BUS_MASTER_ACCESS);

        stf_assert(encoded_desc >= last_desc_ ||
                   STFRecord::isInstructionRecord(last_desc_) ||
                   (wrote_inst_memory_access_content_pair_ && desc == descriptors::internal::Descriptor::STF_INST_MEM_ACCESS) ||
                   (wrote_bus_memory_access_content_pair_ && desc == descriptors::internal::Descriptor::STF_BUS_MASTER_ACCESS) ||
                   (wrote_event_record_group_ && desc == descriptors::internal::Descriptor::STF_EVENT) ||
                   ((wrote_page_table_walk_ || wrote_reg_) && desc == descriptors::internal::Descriptor::STF_INST_PC_TARGET) ||
                   (wrote_page_table_walk_ && desc == descriptors::internal::Descriptor::STF_INST_REG) ||
                   (desc == descriptors::internal::Descriptor::STF_COMMENT),
                   "Attempted out of order write. " << desc << " should come before " << last_desc_);
        switch(desc) {
            case descriptors::internal::Descriptor::STF_IDENTIFIER:
            case descriptors::internal::Descriptor::STF_VERSION:
                stf_assert(!headerStarted(), "Attempted to write more than one " << desc << " record!");
                break;
            case descriptors::internal::Descriptor::STF_COMMENT:
                stf_assert(headerStarted(), "Attempted to write a comment before STF_IDENTIFIER or VERSION records");
                break;
            case descriptors::internal::Descriptor::STF_ISA:
            case descriptors::internal::Descriptor::STF_TRACE_INFO:
            case descriptors::internal::Descriptor::STF_TRACE_INFO_FEATURE:
            case descriptors::internal::Descriptor::STF_PROCESS_ID_EXT:
            case descriptors::internal::Descriptor::STF_VLEN_CONFIG:
            case descriptors::internal::Descriptor::STF_END_HEADER:
                stf_assert(!headerFinalized(), "Attempted to write " << desc << " record outside of the header"); //FALLTHRU
            case descriptors::internal::Descriptor::STF_INST_IEM:
            case descriptors::internal::Descriptor::STF_FORCE_PC:
                stf_assert(headerStarted(), "Attempted to write " << desc << " before the header has started");
                break;
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
                stf_assert(headerFinalized(), "Attempted to write " << desc << " record before finalizing the header");
                break;
            case descriptors::internal::Descriptor::STF_RESERVED:
            case descriptors::internal::Descriptor::STF_RESERVED_END:
                stf_throw("Attempted to write reserved record: " << desc);
                break;
        }

        *stream_ << rec;

        wrote_inst_memory_access_content_pair_ = last_was_memory_access && cur_is_memory_content;
        wrote_bus_memory_access_content_pair_ = last_was_bus_access && cur_is_bus_content;

        const bool last_was_event = (last_desc_ == descriptors::encoded::Descriptor::STF_EVENT);
        wrote_event_record_group_ =
            ((encoded_desc == descriptors::encoded::Descriptor::STF_EVENT_PC_TARGET) && last_was_event);

        wrote_page_table_walk_ = (encoded_desc == descriptors::encoded::Descriptor::STF_PAGE_TABLE_WALK);
        wrote_reg_ = (encoded_desc == descriptors::encoded::Descriptor::STF_INST_REG);

        last_desc_ = encoded_desc;

        return *this;
    }
} // end namespace stf
