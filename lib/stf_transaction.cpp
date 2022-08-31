#include "format_utils.hpp"
#include "stf_transaction.hpp"

namespace stf {
    std::ostream& operator<<(std::ostream& os, const STFTransaction& transaction) {
        format_utils::formatLabel(os, "TXNID");
        format_utils::formatDec(os, transaction.getTransactionId());
        os << std::endl;
        format_utils::formatLabel(os, "CLOCK");
        os << ClockRegistry::getClockName(transaction.getClockId()) << std::endl;
        format_utils::formatLabel(os, "DELTA");
        format_utils::formatDec(os, transaction.getCycleDelta());
        os << std::endl;

        if(const auto& dependencies = transaction.getDependencies(); !dependencies.empty()) {
            format_utils::formatLabel(os, "DEPENDENCIES");
            os << std::endl;
            for(const auto& dep: dependencies) {
                os << dep << std::endl;
            }
        }

        os << transaction.getProtocol() << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const STFTransaction::Dependency& dep) {
        dep.rec_->format_impl(os);
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const STFTransaction::Protocol& protocol) {
        protocol.protocol_->format(os);
        return os;
    }
} // end namespace stf
