#ifndef __STF_PROTOCOL_DATA_HPP__
#define __STF_PROTOCOL_DATA_HPP__

#include "stf_ofstream.hpp"
#include "stf_protocol_id.hpp"

namespace stf {
    class ProtocolData {
        protected:
            const ProtocolId protocol_id_;

        public:
            ProtocolData(const ProtocolId protocol_id) :
                protocol_id_(protocol_id)
            {
            }

            virtual inline ~ProtocolData() = default;

            virtual void pack(STFOFstream& writer) const = 0;
    };

    template<typename T>
    class TypeAwareProtocolData : public ProtocolData {
        private:
            inline void pack(STFOFstream& writer) const final {
                static_cast<const T*>(this)->pack_impl(writer);
            }

        public:
            static ProtocolId getProtocolId();

        protected:
            TypeAwareProtocolData() :
                ProtocolData(getProtocolId())
            {
            }

        public:
            inline ~TypeAwareProtocolData() override {
                static_assert(std::is_base_of<TypeAwareProtocolData<T>, T>::value,
                              "Template parameter passed to TypeAwareProtocolData must inherit from TypeAwareProtocolData");
            }

    };
} // end namespace stf

#endif
