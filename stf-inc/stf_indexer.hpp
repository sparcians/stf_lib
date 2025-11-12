#ifndef __STF_INDEXER_HPP__
#define __STF_INDEXER_HPP__

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <string_view>
#include <thread>

#include "stf_exception.hpp"
#include "stf_record.hpp"
#include "stf_reader_base.hpp"
#include "stf_valid_value.hpp"

namespace stf {
    /**
     * \class STFIndexer
     * \brief Generates an index of a trace for the given reader type to provide random-access seeking
     */
    template<typename ReaderType, size_t granularity = 1024>
    class STFIndexer {
        static_assert(std::is_base_of_v<STFReaderBase, ReaderType>,
                      "ReaderType must inherit from STFReaderBase");

        private:
            using IndexMap = std::map<size_t, size_t>;

        public:
            /**
             * \typedef const_iterator
             * \brief Iterator type
             */
            using const_iterator = IndexMap::const_iterator;

        private:
            std::thread reader_thread_;
            mutable std::mutex index_map_mutex_;
            mutable std::condition_variable sync_cv_;

            IndexMap index_map_;
            stf::ValidValue<size_t> last_index_;
            bool scan_done_ = false;
            std::atomic_bool stop_reading_ = false; /**< Stops the reader thread if set to true */

            void scan_(const std::string& filename) {
                stf_assert(!scan_done_);

                ReaderType reader(filename);
                stf::STFRecord::UniqueHandle rec;
                size_t cur_offset = reader.getCurrentOffset();

                while(STF_EXPECT_TRUE(!stop_reading_.load(std::memory_order_relaxed))) {
                    try {
                        reader >> rec;

                        if(rec->isMarkerRecord()) {
                            if(const auto index = reader.numMarkerRecordsRead() - 1; index % granularity == 0) {
                                {
                                    std::lock_guard<std::mutex> guard(index_map_mutex_);
                                    index_map_.emplace(index, cur_offset);
                                    last_index_ = index;
                                }
                                sync_cv_.notify_one();
                                cur_offset = reader.getCurrentOffset();
                            }
                        }
                    }
                    catch(const stf::EOFException&) {
                        break;
                    }
                }

                {
                    std::lock_guard<std::mutex> guard(index_map_mutex_);
                    scan_done_ = true;
                }

                sync_cv_.notify_one();
            }

            inline const_iterator findNearestEntryUnsafe_(const size_t index) const {
                auto it = index_map_.lower_bound(index);

                if(it == index_map_.end() || it->first > index) {
                    if(STF_EXPECT_FALSE(it == index_map_.begin())) {
                        return end();
                    }

                    --it;
                }

                return it;
            }

        public:
            STFIndexer() = default;

            /**
             * Constructs an STFIndexer
             * \param trace Trace to open and index
             */
            explicit STFIndexer(const std::string_view trace) {
                open(trace);
            }

            /**
             * \brief Opens and begins indexing the given trace
             * \param trace Trace to index
             */
            inline void open(const std::string_view trace) {
                scan_done_ = false;
                stop_reading_ = false;
                reader_thread_ = std::thread(&STFIndexer::scan_, this, std::string(trace));
            }

            /**
             * \brief Closes the indexer, aborting any in-flight indexing
             */
            void close() {
                if(!scan_done_ || reader_thread_.joinable()) {
                    stop_reading_ = true;
                    reader_thread_.join();
                }
            }

            ~STFIndexer() {
                close();
            }

            /**
             * \brief Returns an iterator to the end of the internal index map
             */
            const_iterator end() const { return index_map_.end(); }

            /**
             * \brief Returns an iterator to the entry at or before the given index
             * \param index Index to look up
             */
            inline const_iterator findNearestEntry(const size_t index) const {
                if(scan_done_) {
                    return findNearestEntryUnsafe_(index);
                }

                std::unique_lock lk(index_map_mutex_);

                sync_cv_.wait(lk, [this, index](){
                    return (last_index_.valid() && index <= last_index_.get()) || scan_done_;
                });

                const auto it = findNearestEntryUnsafe_(index);

                lk.unlock();

                return it;
            }

            /**
             * \brief Waits for the index scan to complete
             */
            void waitForScanComplete() {
                if(!scan_done_) {
                    stf_assert(reader_thread_.joinable(), "Atempted to call waitForScanComplete without scanning a trace");
                    reader_thread_.join();
                }
            }

            /**
             * \brief Returns true if the index scan is finished
             */
            bool scanComplete() const { return scan_done_; }

            /**
             * \brief Returns how many items have been scanned so far
             */
            size_t numItemsScanned() const { return last_index_.valid() ? last_index_.get() + 1 : 0; }
    };
} // end namespace stf

#endif
