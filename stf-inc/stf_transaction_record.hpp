#ifndef __STF_TRANSACTION_RECORD_HPP__
#define __STF_TRANSACTION_RECORD_HPP__

#include "stf_ifstream.hpp"
#include "stf_protocol_data.hpp"
#include "stf_record_interfaces.hpp"

namespace stf {
    class TransactionRecord : public TypeAwareSTFRecord<TransactionRecord> {
        public:
            enum class TransactionType : uint8_t {
                INVALID = 0,
                REQUEST = 1,
                RESPONSE = 2
            };

        private:
            uint64_t transaction_id_;
            uint64_t time_delta_;
            uint64_t address_;
            TransactionType transaction_type_;
            ProtocolData protocol_data_;
            SerializableVector<uint8_t, uint64_t> payload_data_;

        public:
            /**
             * Packs an STFIdentifierRecord into an STFOFstream
             * \param writer STFOFstream to use
             */
            inline void pack_impl(STFOFstream& writer) const {
                write_(writer,
                       transaction_id_,
                       time_delta_,
                       address_,
                       transaction_type_,
                       protocol_data_,
                       payload_data_);
            }

            /**
             * Unpacks an STFIdentifierRecord from an STFIFstream
             * \param reader STFIFstream to use
             */
            __attribute__((always_inline))
            inline void unpack_impl(STFIFstream& reader) {
                read_(reader,
                      transaction_id_,
                      time_delta_,
                      address_,
                      transaction_type_);
                reader >> protocol_data_;
                reader >> payload_data_;
            }

    };
} // end namespace stf

#endif
