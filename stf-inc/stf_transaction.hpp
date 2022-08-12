#ifndef __STF_TRANSACTION_HPP__
#define __STF_TRANSACTION_HPP__

#include <ostream>

#include "stf_record_map.hpp"
#include "stf_record_types.hpp"
#include "stf_item.hpp"

namespace stf {
    class STFTransactionReader;

    namespace delegates {
        class STFTransactionDelegate;
    } // end namespace delegates

    /**
     * \class STFTransaction
     * Convenient wrapper class for transaction and dependency records. Returned from STFTransactionReader iterator.
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
                     * Gets the time delta between when the dependent transaction completes
                     * and when this dependency can be considered fulfilled
                     */
                    inline auto getTimeDelta() const {
                        return rec_->getTimeDelta();
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
            uint64_t time_delta_ = 0;
            Protocol protocol_;

            using DependencyVector = boost::container::small_vector<Dependency, 1>;
            DependencyVector dependencies_;

            __attribute__((always_inline))
            inline const STFRecord* appendOrigRecord_(STFRecord::UniqueHandle&& urec) {
                return orig_records_.emplace(std::move(urec));
            }

            __attribute__((always_inline))
            inline void reset_() {
                transaction_id_ = 0;
                time_delta_ = 0;
                protocol_.reset(nullptr);
                dependencies_.clear();
            }

            __attribute__((always_inline))
            inline void setTransactionInfo_(const STFRecord* const rec) {
                const auto& transaction_info = rec->as<TransactionRecord>();
                transaction_id_ = transaction_info.getTransactionId();
                time_delta_ = transaction_info.getTimeDelta();
                protocol_.reset(transaction_info);
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
             * Gets the time delta
             */
            inline uint64_t getTimeDelta() const {
                return time_delta_;
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

                friend class stf::STFTransactionReader;
        };
    } // end namespace delegates
} // end namespace stf

#endif
