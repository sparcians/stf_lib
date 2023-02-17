#ifndef __STF_PTE_READER_HPP__
#define __STF_PTE_READER_HPP__

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "stf_compressed_chunked_base.hpp"
#include "stf_descriptor.hpp"
#include "stf_reader.hpp"
#include "stf_page_table.hpp"

namespace stf {
    /**
     * \class STFPTEReader
     * \brief Opens an STF and reads only records related to page translation. Reading takes place in a separate
     * thread to (hopefully) read and process page translation info before it is needed by the main STFInstReader.
     */
    class STFPTEReader {
        private:
            using IntDescriptor = descriptors::internal::Descriptor;

            std::thread reader_thread_; /**< Holds the reader thread */

            mutable std::mutex page_table_mutex_; /**< Protects the page table from simultaneous reads/writes from separate threads */
            mutable std::condition_variable sync_cv_; /**< Used by the main thread to wait until the reader thread has advanced far enough to handle a translation request */
            STFPageTable page_table_ /**< Tracks page translation info */;
            uint64_t last_valid_insts_ = 0; /**< Latest instruction index that we have translation info for */
            bool done_reading_ = true; /**< If true, the reader thread has completed */
            std::atomic_bool stop_reading_ = false; /**< Stops the reader thread if set to true */

            /**
             * Reads the page translation info from a trace in a separate thread
             */
            void readerThread_(const std::string& filename) {
                STFReader reader(filename, true);
                // Only bother reading the trace if it actually has PTEs
                if(reader.getTraceFeatures()->hasAnyFeatures(TRACE_FEATURES::STF_CONTAIN_PTE,
                                                             TRACE_FEATURES::STF_CONTAIN_PTE_ONLY,
                                                             TRACE_FEATURES::STF_CONTAIN_PTE_HW_AD)) {
                    {
                        std::lock_guard<std::mutex> guard(page_table_mutex_);
                        // Configure the page table for the trace IEM
                        page_table_.reset(reader.getInitialIEM());
                    }
                    STFRecord::UniqueHandle urec;

                    // Keep going unless the main thread tells us to stop
                    // Using std::memory_order_relaxed ensures we don't add any unnecessary barriers
                    while(STF_EXPECT_TRUE(!stop_reading_.load(std::memory_order_relaxed))) {
                        try {
                            reader >> urec;
                            if(STF_EXPECT_FALSE(urec->getId() == IntDescriptor::STF_PAGE_TABLE_WALK)) {
                                const auto& pte_rec = urec->as<PageTableWalkRecord>();
                                {
                                    std::lock_guard<std::mutex> guard(page_table_mutex_);
                                    page_table_.update(pte_rec);
                                    // Update the last read instruction index
                                    last_valid_insts_ = reader.numInstsRead();
                                }
                                // Notify the main thread that there's been an update
                                sync_cv_.notify_one();
                            }
                            else if(STF_EXPECT_FALSE(urec->getId() == IntDescriptor::STF_INST_REG)) {
                                const auto& reg_rec = urec->as<InstRegRecord>();
                                const auto operand_type = reg_rec.getOperandType();
                                const bool is_state = operand_type == Registers::STF_REG_OPERAND_TYPE::REG_STATE;
                                if(STF_EXPECT_FALSE((reg_rec.getReg() == Registers::STF_REG::STF_REG_CSR_SATP) &&
                                                    (is_state || (operand_type == Registers::STF_REG_OPERAND_TYPE::REG_DEST)))) {
                                    std::lock_guard<std::mutex> guard(page_table_mutex_);
                                    // reader.numInstsRead() == index of *previous* instruction
                                    // If this is a state record, we should set its effective index to that value
                                    // If this is a dest record, we should set its effective index to the instruction
                                    // *after* this one (i.e., numInstsRead() + 2)
                                    //
                                    // NOTE: If an instruction has both a dest and a state record attached, this index
                                    // math will ensure they don't interfere with each other
                                    page_table_.updateSatp(reg_rec, reader.numInstsRead() + 2*(!is_state));
                                }
                            }
                            else if(STF_EXPECT_FALSE(urec->getId() == IntDescriptor::STF_INST_OPCODE16 ||
                                                     urec->getId() == IntDescriptor::STF_INST_OPCODE32)) {
                                // Periodically update the last read instruction index
                                // We don't want to do this too often or else we risk spurious wakeups of the main thread,
                                // so we'll try to do it 10 times per chunk
                                static constexpr size_t INSTRUCTION_INDEX_UPDATE_INTERVAL = STFCompressedChunkedBase::DEFAULT_CHUNK_SIZE / 10;

                                if(const uint64_t insts_read = reader.numInstsRead();
                                   STF_EXPECT_FALSE(insts_read % INSTRUCTION_INDEX_UPDATE_INTERVAL == 0)) {
                                    {
                                        std::lock_guard<std::mutex> guard(page_table_mutex_);
                                        last_valid_insts_ = insts_read;
                                    }
                                    sync_cv_.notify_one();
                                }
                            }
                            else if(STF_EXPECT_FALSE(urec->getId() == IntDescriptor::STF_EVENT)) {
                                const auto& event_rec = urec->as<EventRecord>();
                                if(STF_EXPECT_FALSE(event_rec.isModeChange())) {
                                    {
                                        std::lock_guard<std::mutex> guard(page_table_mutex_);
                                        uint64_t index = reader.numInstsRead();

                                        // index == index of *previous* instruction
                                        // If index == 0, this is the first instruction in the trace, and we should insert that
                                        // mode at index 0.
                                        // Otherwise, increment the index by 2 so it points to the instruction *after* this one
                                        index += 2*(index != 0);
                                        page_table_.updateMode(static_cast<EXECUTION_MODE>(event_rec.getData().front()), index);
                                    }
                                }
                            }
                        }
                        catch(const EOFException&) {
                            break;
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> guard(page_table_mutex_);
                    // Let the main thread know it doesn't need to use the mutex anymore
                    done_reading_ = true;
                }
                sync_cv_.notify_one();
            }

        public:
            /**
             * Constructs an STFPTEReader
             */
            STFPTEReader() = default;

            /**
             * Constructs an STFPTEReader and opens the given filename
             */
            template<typename StrType>
            explicit STFPTEReader(const StrType& filename) {
                open(filename);
            }

            /**
             * Destructor
             */
            ~STFPTEReader() {
                close();
            }

            STFPTEReader(const STFPTEReader&) = delete;
            STFPTEReader& operator=(const STFPTEReader&) = delete;

            /**
             * Opens the given trace file
             */
            inline void open(const std::string_view filename) {
                done_reading_ = false;
                stop_reading_ = false;
                reader_thread_ = std::thread(&STFPTEReader::readerThread_, this, std::string(filename));
            }

            /**
             * Closes the reader
             */
            inline void close() {
                // Reader thread is still running and/or needs cleanup
                if(!done_reading_ || reader_thread_.joinable()) {
                    // Tell the thread to stop
                    stop_reading_ = true;
                    // Wait for it to finish and clean up
                    reader_thread_.join();
                }
            }

            /**
             * Translates the given VA using the page table setup that was valid at the given instruction index
             */
            __attribute__((always_inline))
            inline uint64_t translate(const uint64_t va, const uint64_t index) const {
                // If the reader thread has finished, there's no need to use the mutex anymore
                if(done_reading_) {
                    return page_table_.translate(va, index);
                }

                std::unique_lock lk(page_table_mutex_);

                // We don't know when a table walk will happen in the trace, so wait until
                // the reader thread has advanced past the current instruction index
                sync_cv_.wait(lk, [this, index](){ return index <= this->last_valid_insts_ || this->done_reading_; });

                // We know the reader has valid translation info for the current index, so grab the mutex and do
                // the translation
                const uint64_t pa = page_table_.translate(va, index);
                lk.unlock();

                return pa;
            }
    };
} // end namespace stf

#endif
