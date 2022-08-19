#ifndef __STF_TRANSACTION_RECORD_READER_HPP__
#define __STF_TRANSACTION_RECORD_READER_HPP__

#include <string_view>

#include "stf_reader_base.hpp"

namespace stf {
    class STFTransactionWriter;

    /**
     * \class STFTransactionRecordReader
     * Transaction trace reader
     */
    class STFTransactionRecordReader : public STFReaderBase {
        private:
            protocols::ProtocolId expected_protocol_ = protocols::ProtocolId::__RESERVED_END;

            /**
             * Reads the STF header
             */
            void readHeader_() final;

        public:
            STFTransactionRecordReader() = default;

            /**
             * Constructs an STFReader and opens the specified file
             * \param filename file to open
             * \param expected_protocol Protocol ID we expect the trace to contain
             * \param force_single_threaded_stream If true, forces single threaded mode
             */
            explicit STFTransactionRecordReader(const std::string_view filename,
                                                const protocols::ProtocolId expected_protocol = protocols::ProtocolId::__RESERVED_END,
                                                const bool force_single_threaded_stream = false) :
                expected_protocol_(expected_protocol)
            {
                open(filename, force_single_threaded_stream);
            }

            /**
             * Sets the expected protocol type for this trace
             * \param expected_protocol Protocol ID we expect the trace to contain
             */
            inline void setExpectedProtocol(const protocols::ProtocolId expected_protocol) {
                expected_protocol_ = expected_protocol;
            }

            /**
             * Returns the number of instructions read so far
             */
            inline size_t numTransactionsRead() const {
                return getNumMarkerRecords_();
            }

            /**
             * Copies the header to the specifed STFTransactionWriter
             * \param stf_writer STFTransactionWriter to use
             */
            void copyHeader(STFTransactionWriter& stf_writer) const;

            /**
             * Dumps the header to the specified std::ostream
             * \param os ostream to use
             */
            void dumpHeader(std::ostream& os) const final;

            /**
             * Gets the protocol ID for the trace
             */
            inline protocols::ProtocolId getProtocolId() const {
                return stream_->getProtocolId();
            }
    };
} // end namespace stf

#endif
