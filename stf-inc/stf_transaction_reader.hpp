#ifndef __STF_TRANSACTION_READER_HPP__
#define __STF_TRANSACTION_READER_HPP__

#include "stf_buffered_reader.hpp"
#include "stf_filter_types.hpp"
#include "stf_transaction.hpp"
#include "stf_transaction_record_reader.hpp"

namespace stf {
    /**
     * \class STFTransactionReader
     * Buffered transaction record reader. Returns STFTransaction objects.
     */
    class STFTransactionReader final : public STFBufferedReader<STFTransaction, DummyFilter, STFTransactionReader, STFTransactionRecordReader> {
        private:
            using ParentReader = STFBufferedReader<STFTransaction, DummyFilter, STFTransactionReader, STFTransactionRecordReader>;
            friend ParentReader;
            using IntDescriptor = typename ParentReader::IntDescriptor;

            __attribute__((always_inline))
            inline const STFRecord* handleNewRecord_(STFTransaction& transaction, STFRecord::UniqueHandle&& urec) {
                return delegates::STFTransactionDelegate::appendOrigRecord_(transaction, std::move(urec));
            }

            // read STF records to construction a STFInst instance
            __attribute__((hot, always_inline))
            inline void readNext_(STFTransaction& transaction) {
                delegates::STFTransactionDelegate::reset_(transaction);
                while(true) {
                    const auto rec = readRecord_(transaction);
                    const auto id = rec->getId();

                    if(STF_EXPECT_TRUE(id == IntDescriptor::STF_TRANSACTION)) {
                        delegates::STFTransactionDelegate::setTransactionInfo_(transaction, rec);
                        initItemIndex_(transaction);
                        break;
                    }
                    else if(STF_EXPECT_TRUE(id == IntDescriptor::STF_TRANSACTION_DEPENDENCY)) {
                        delegates::STFTransactionDelegate::appendDependency_(transaction, rec);
                    }
                    else {
                        switch(id) {
                            // These descriptors don't need any special handling
                            case IntDescriptor::STF_COMMENT:
                                break;

                            // These descriptors should only be in instruction traces
                            case IntDescriptor::STF_INST_REG:
                            case IntDescriptor::STF_INST_OPCODE16:
                            case IntDescriptor::STF_INST_OPCODE32:
                            case IntDescriptor::STF_INST_MEM_ACCESS:
                            case IntDescriptor::STF_INST_MEM_CONTENT:
                            case IntDescriptor::STF_INST_MICROOP:
                            case IntDescriptor::STF_INST_READY_REG:
                            case IntDescriptor::STF_PAGE_TABLE_WALK:
                            case IntDescriptor::STF_BUS_MASTER_ACCESS:
                            case IntDescriptor::STF_BUS_MASTER_CONTENT:
                            case IntDescriptor::STF_ISA:
                            case IntDescriptor::STF_VLEN_CONFIG:
                            case IntDescriptor::STF_INST_IEM:
                            case IntDescriptor::STF_PROCESS_ID_EXT:
                            case IntDescriptor::STF_FORCE_PC:
                            case IntDescriptor::STF_EVENT_PC_TARGET:
                            case IntDescriptor::STF_EVENT:
                            case IntDescriptor::STF_INST_PC_TARGET:
                                stf_throw("Saw an instruction record " << id << " in a transaction trace");

                            case IntDescriptor::STF_TRACE_INFO:
                            case IntDescriptor::STF_IDENTIFIER:
                            case IntDescriptor::STF_VERSION:
                            case IntDescriptor::STF_TRACE_INFO_FEATURE:
                            case IntDescriptor::STF_PROTOCOL_ID:
                            case IntDescriptor::STF_CLOCK_ID:
                            case IntDescriptor::STF_END_HEADER:
                                stf_throw("Saw an unexpected record outside of the header: " << id);

                            // These descriptors *will* never be seen here
                            case IntDescriptor::STF_TRANSACTION: // handled earlier
                            case IntDescriptor::STF_TRANSACTION_DEPENDENCY: // handled earlier
                            case IntDescriptor::STF_RESERVED: // cannot be constructed
                            case IntDescriptor::__RESERVED_END: // cannot be constructed
                                __builtin_unreachable();
                        };
                    }
                }
            }

            __attribute__((hot, always_inline))
            inline size_t rawNumRead_() const {
                return rawNumTransactionsRead();
            }

        public:
            /**
             * \class iterator
             * \brief Instruction stream iterator
             */
            class iterator : public ParentReader::base_iterator {
                public:
                    iterator() = default;

                    /**
                     * Iterator constructor
                     * \param sir Parent STFInstReaderBase
                     * \param end If true, this is an end iterator
                     */
                    explicit iterator(STFTransactionReader *sir, const bool end = false) :
                        ParentReader::base_iterator(sir, end)
                    {
                    }

                    /**
                     * Returns whether this is the last instruction in the trace
                     */
                    inline bool isLastTransaction() const {
                        return ParentReader::base_iterator::isLastItem_();
                    }
            };

            /**
             * Constructs an STFTransactionReader
             * \param filename Trace file to open
             * \param expected_protocol Protocol ID the trace is expected to contain
             * \param buffer_size Number of transactions that will be buffered
             * \param force_single_threaded_stream If true, use a single threaded reader
             */
            template<typename StrType>
            explicit STFTransactionReader(const StrType& filename,
                                          const protocols::ProtocolId expected_protocol = protocols::ProtocolId::__RESERVED_END,
                                          const size_t buffer_size = DEFAULT_BUFFER_SIZE_,
                                          const bool force_single_threaded_stream = false) :
                ParentReader(buffer_size)
            {
                setExpectedProtocol(expected_protocol);
                open(filename, force_single_threaded_stream);
            }

            /**
             * Returns the number of instructions read so far without filtering
             */
            __attribute__((always_inline))
            inline size_t rawNumTransactionsRead() const {
                return STFTransactionRecordReader::numTransactionsRead();
            }
    };
} // end namespace stf

#endif
