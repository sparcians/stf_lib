
// <STF_Reader> -*- HPP -*-

/**
 * \brief This file defines APIs for reading STF files.
 */

#ifndef __STF_READER_HPP__
#define __STF_READER_HPP__

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

#include "stf_enums.hpp"
#include "stf_exception.hpp"
#include "stf_reader_base.hpp"

namespace stf {
    class ISARecord;
    class InstIEMRecord;
    class ForcePCRecord;
    class ProcessIDExtRecord;
    class VLenConfigRecord;
    class STFWriter;

    /**
     * \class STFReader
     *
     * Holds internal state of an STF reader instance.
     */
    class STFReader : public STFReaderBase {
        private:
            STFRecord::ConstHandle<ISARecord> isa_;
            STFRecord::ConstHandle<InstIEMRecord> initial_iem_;
            STFRecord::ConstHandle<ForcePCRecord> initial_pc_;
            STFRecord::ConstHandle<ProcessIDExtRecord> initial_process_id_;
            STFRecord::ConstHandle<VLenConfigRecord> vlen_config_;

            /**
             * Reads the STF header
             */
            void readHeader_() final;

            /**
             * Returns whether the header was valid
             */
            void validateHeader_() const final;

        public:
            STFReader() = default;

            /**
             * Constructs an STFReader and opens the specified file
             * \param filename file to open
             * \param force_single_threaded_stream If true, forces single threaded mode
             */
            explicit STFReader(const std::string_view filename, const bool force_single_threaded_stream = false) {
                open(filename, force_single_threaded_stream);
            }

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
             * Gets the initial TGID
             */
            uint32_t getInitialTGID() const;

            /**
             * Gets the initial TID
             */
            uint32_t getInitialTID() const;

            /**
             * Gets the initial ASID
             */
            uint32_t getInitialASID() const;

            /**
             * Closes the file
             */
            int close() override;

            /**
             * Returns the number of instructions read so far
             */
            inline size_t numInstsRead() const {
                return getNumMarkerRecords_();
            }

            /**
             * Copies the header to the specifed STFWriter
             * \param stf_writer STFWriter to use
             */
            void copyHeader(STFWriter& stf_writer) const;

            /**
             * Gets the current PC
             */
            inline uint64_t getPC() const {
                return stream_->getPC();
            }

            /**
             * Dumps the header to the specified std::ostream
             * \param os ostream to use
             */
            void dumpHeader(std::ostream& os) const final;

            /**
             * Gets the vlen parameter for the trace
             */
            inline vlen_t getVLen() const {
                return stream_->getVLen();
            }
    };
} // end namespace stf
// __STF_READER_HPP__
#endif
