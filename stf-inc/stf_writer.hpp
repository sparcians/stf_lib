
// <STF_Writer> -*- HPP -*-

/**
 * \brief This file defines an API for reading STF files.
 */

#ifndef __STF_WRITER_HPP__
#define __STF_WRITER_HPP__

#include <string_view>

#include "stf_enums.hpp"
#include "stf_writer_base.hpp"

namespace stf {
    class VersionRecord;
    class ISARecord;
    class InstIEMRecord;
    class ForcePCRecord;
    class VLenConfigRecord;

    /**
     * \class STFWriter
     *
     * Holds the internal state of a STF [trace] writer instance.
     */
    class STFWriter : public STFWriterBase {
        private:
            descriptors::encoded::Descriptor last_desc_ = descriptors::encoded::Descriptor::STF_RESERVED; /**< Last record type written */

            STFRecord::Handle<ISARecord> isa_;
            bool isa_written_ = false;

            STFRecord::Handle<InstIEMRecord> initial_iem_;
            bool initial_iem_written_ = false;

            STFRecord::Handle<ForcePCRecord> initial_pc_;
            bool initial_pc_written_ = false;

            STFRecord::Handle<VLenConfigRecord> vlen_config_;
            bool vlen_config_written_ = false;

            bool wrote_inst_memory_access_content_pair_ = false;
            bool wrote_bus_memory_access_content_pair_ = false;
            bool wrote_event_record_group_ = false;
            bool wrote_page_table_walk_ = false;
            bool wrote_reg_ = false;
            bool force_32bit_events_ = false;

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
                               size_t chunk_size = DEFAULT_CHUNK_SIZE) :
                STFWriterBase(filename, compression_level, chunk_size)
            {
            }

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
             * Removes trace feature from header
             * \param trace_feature feature to remove
             */
            inline void disableTraceFeature(const TRACE_FEATURES trace_feature) final {
                if(trace_feature == TRACE_FEATURES::STF_CONTAIN_EVENT64) {
                    force_32bit_events_ = true;
                }

                STFWriterBase::disableTraceFeature(trace_feature);
            }

            /**
             * Sets the vlen parameter
             * \param vlen vlen value to set
             */
            void setVLen(vlen_t vlen);

            void flushHeader() final;

            void finalizeHeader() final;

            /**
             * Closes the file
             */
            int close() final;

            /**
             * Returns the number of instructions read so far
             */
            inline size_t numInstsWritten() const {
                return getNumMarkerRecords_();
            }

            /**
             * Writes an STFRecord
             * \param rec Record to write
             */
            STFWriter& operator<<(const STFRecord& rec) final;
        };
} // end namespace stf
// __STF_WRITER_HPP__
#endif
