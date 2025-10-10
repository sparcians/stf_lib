#ifndef __STF_TRANSACTION_HPP__
#define __STF_TRANSACTION_HPP__

#include <ostream>

#include "stf_record_map.hpp"
#include "stf_record_types.hpp"
#include "stf_item.hpp"
#include "stf_transaction_writer.hpp"

namespace stf {
    template<bool Indexed>
    class STFTransactionReaderBase;

    namespace delegates {
        class STFTransactionDelegate;
    } // end namespace delegates

    /**
     * \class STFTransaction
     * Convenient wrapper class for transaction and dependency records. Returned from STFTransactionReaderBase iterator.
     */
    class STFTransaction : public STFItem {
        public:
            /**
             * \class Dependency
             * Wraps a dependency record
             */
            class Dependency {
                private:
                    const TransactionDependencyRecord* const rec_;

                public:
                    /**
                     * Constructs a Dependency from an existing TransactionDependencyRecord
                     * \param rec Pointer to underlying record for this dependency
                     */
                    explicit Dependency(const TransactionDependencyRecord* const rec) :
                        rec_(rec)
                    {
                    }

                    /**
                     * Gets the ID of the transaction this dependency points to
                     */
                    inline auto getDependencyId() const {
                        return rec_->getDependencyId();
                    }

                    /**
                     * Gets the cycle delta between when the dependent transaction completes
                     * and when this dependency can be considered fulfilled
                     */
                    inline auto getCycleDelta() const {
                        return rec_->getCycleDelta();
                    }

                    /**
                     * Gets the clock domain ID for this dependency
                     */
                    inline auto getClockId() const {
                        return rec_->getClockId();
                    }

                    /**
                     * std::ostream operator<< override for Dependency objects
                     * \param os std::ostream to use
                     * \param dep Dependency to format
                     */
                    friend std::ostream& operator<<(std::ostream& os, const Dependency& dep);
            };

            /**
             * \class Protocol
             * Wraps a protocol data record
             */
            class Protocol {
                private:
                    const protocols::ProtocolData* protocol_ = nullptr;

                public:
                    Protocol() = default;

                    /**
                     * Constructs a Protocol from a TransactionRecord
                     * \param rec TransactionRecord containing the protocol data this object will wrap
                     */
                    explicit Protocol(const TransactionRecord& rec) :
                        Protocol(rec.getProtocolData().get())
                    {
                    }

                    /**
                     * Constructs a Protocol from a ProtocolData object
                     * \param protocol Pointer to underlying ProtocolData object that will be wrapped
                     */
                    explicit Protocol(const protocols::ProtocolData* const protocol) :
                        protocol_(protocol)
                    {
                    }

                    /**
                     * Points this object to a different ProtocolData object
                     * \param rec TransactionRecord containing the new ProtocolData object
                     */
                    inline void reset(const TransactionRecord& rec) {
                        reset(rec.getProtocolData().get());
                    }

                    /**
                     * Points this object to a different ProtocolData object
                     * \param protocol Pointer to the new ProtocolData object
                     */
                    inline void reset(const protocols::ProtocolData* const protocol) {
                        protocol_ = protocol;
                    }

                    /**
                     * Casts the protocol data to the desired type
                     */
                    template<typename T>
                    inline const auto& as() const {
                        return protocol_->as<T>();
                    }

                    /**
                     * Gets the protocol ID value
                     */
                    inline auto getId() const {
                        return protocol_->getId();
                    }

                    /**
                     * std::ostream operator<< override for Protocol objects
                     * \param os std::ostream to use
                     * \param protocol Protocol to format
                     */
                    friend std::ostream& operator<<(std::ostream& os, const Protocol& protocol);
            };

        private:
            friend class delegates::STFTransactionDelegate;
            RecordMap orig_records_; /**< orig_records_ contains all the STFRecords for
                                      * this transaction, in the order they were read
                                      * from the trace
                                      */

            uint64_t transaction_id_ = 0;
            uint64_t cycle_delta_ = 0;
            ClockId clock_id_ = INVALID_CLOCK_ID;
            Protocol protocol_;
            const TransactionRecord::Metadata* metadata_ = nullptr;

            using DependencyVector = boost::container::small_vector<Dependency, 1>;
            DependencyVector dependencies_;

            __attribute__((always_inline))
            inline const STFRecord* appendOrigRecord_(STFRecord::UniqueHandle&& urec) {
                return orig_records_.emplace(std::move(urec));
            }

            __attribute__((always_inline))
            // cppcheck-suppress duplInheritedMember
            inline void reset_() {
                STFItem::reset_();
                orig_records_.clear();
                transaction_id_ = 0;
                cycle_delta_ = 0;
                clock_id_ = INVALID_CLOCK_ID;
                protocol_.reset(nullptr);
                metadata_ = nullptr;
                dependencies_.clear();
            }

            __attribute__((always_inline))
            inline void setTransactionInfo_(const STFRecord* const rec) {
                const auto& transaction_info = rec->as<TransactionRecord>();
                transaction_id_ = transaction_info.getTransactionId();
                cycle_delta_ = transaction_info.getCycleDelta();
                clock_id_ = transaction_info.getClockId();
                protocol_.reset(transaction_info);
                metadata_ = &transaction_info.getMetadata();
            }

            __attribute__((always_inline))
            inline void appendDependency_(const STFRecord* const rec) {
                dependencies_.emplace_back(rec->as_ptr<TransactionDependencyRecord>());
            }

        public:
            /**
             * std::ostream operator<< override for STFTransaction objects
             * \param os std::ostream to use
             * \param transaction STFTransaction to format
             */
            friend std::ostream& operator<<(std::ostream& os, const STFTransaction& transaction);

            /**
             * Gets the transaction ID
             */
            inline uint64_t getTransactionId() const {
                return transaction_id_;
            }

            /**
             * Gets the cycle delta
             */
            inline uint64_t getCycleDelta() const {
                return cycle_delta_;
            }

            /**
             * Gets the clock domain ID
             */
            inline ClockId getClockId() const {
                return clock_id_;
            }

            /**
             * Gets the protocol data
             */
            inline const Protocol& getProtocol() const {
                return protocol_;
            }

            /**
             * Gets any dependencies associated with this transaction
             */
            inline const auto& getDependencies() const {
                return dependencies_;
            }

            /**
             * Gets the vector of CommentRecords
             */
            inline const auto& getComments() const {
                return orig_records_.at(descriptors::internal::Descriptor::STF_COMMENT);
            }

            /**
             * Gets transaction metadata
             */
            inline const auto& getMetadata() const {
                return *metadata_;
            }

            /**
             * \brief Write all records in this transaction to STFTransactionWriter
             */
            inline void write(STFTransactionWriter& stf_writer) const {
                for (const auto& vec_pair: orig_records_.sorted()) {
                    for(const auto& record: vec_pair.second) {
                        stf_writer << *record;
                    }
                }
            }
    };

    namespace delegates {
        /**
         * \class STFTransactionDelegate
         * Delegate class used to hide any non-const methods from non-reader classes
         */
        class STFTransactionDelegate : public STFItemDelegate {
            private:
                __attribute__((always_inline))
                static inline const STFRecord* appendOrigRecord_(STFTransaction& transaction,
                                                                 STFRecord::UniqueHandle&& urec) {
                    return transaction.appendOrigRecord_(std::move(urec));
                }

                __attribute__((always_inline))
                static inline void reset_(STFTransaction& transaction) {
                    transaction.reset_();
                }

                __attribute__((always_inline))
                static inline void setTransactionInfo_(STFTransaction& transaction,
                                                       const STFRecord* const urec) {
                    transaction.setTransactionInfo_(urec);
                }

                __attribute__((always_inline))
                static inline void appendDependency_(STFTransaction& transaction,
                                                     const STFRecord* const urec) {
                    transaction.appendDependency_(urec);
                }

                template<bool Indexed>
                friend class stf::STFTransactionReaderBase;
        };
    } // end namespace delegates
} // end namespace stf

#endif
