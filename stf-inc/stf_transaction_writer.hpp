#ifndef __STF_TRANSACTION_WRITER_HPP__
#define __STF_TRANSACTION_WRITER_HPP__

#include <string_view>

#include "stf_clock_id.hpp"
#include "stf_protocol_id.hpp"
#include "stf_writer_base.hpp"

namespace stf {
    class ProtocolIdRecord;
    class ClockIdRecord;

    /**
     * \class STFTransactionWriter
     *
     * Writes timed transaction traces
     */
    class STFTransactionWriter : public STFWriterBase {
        private:
            STFRecord::Handle<ProtocolIdRecord> protocol_id_;
            bool protocol_id_written_ = false;
            std::vector<ClockIdRecord> clock_ids_;
            bool clock_ids_written_ = false;

        public:
            STFTransactionWriter() = default;

            /**
             * Constructor
             *
             * \param filename The trace file name
             * \param compression_level Compression level to use (-1 for default).
             * \param chunk_size Chunk size to use (Defaults to DEFAULT_CHUNK_SIZE)
             */
            explicit STFTransactionWriter(std::string_view filename,
                                          int compression_level = -1,
                                          size_t chunk_size = DEFAULT_CHUNK_SIZE) :
                STFWriterBase(filename, compression_level, chunk_size)
            {
            }

            /**
             * Sets the protocol ID parameter
             * \param protocol_id protocol ID value to set
             */
            void setProtocolId(protocols::ProtocolId protocol_id);

            /**
             * Adds a clock to the trace. The first clock added will be used as the default clock domain.
             * \param clock_id Clock domain ID
             * \param name Name of the clock domain
             */
            void addClock(ClockId clock_id, std::string_view name);

            void flushHeader() final;

            void finalizeHeader() final;

            /**
             * Closes the file
             */
            int close() final;

            /**
             * Returns the number of instructions read so far
             */
            inline size_t numTransactionsWritten() const {
                return getNumMarkerRecords_();
            }

            /**
             * Writes an STFRecord
             * \param rec Record to write
             */
            STFTransactionWriter& operator<<(const STFRecord& rec) final;
    };
} // end namespace stf

#endif
