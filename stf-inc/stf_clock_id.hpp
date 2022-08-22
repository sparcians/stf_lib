#ifndef __STF_CLOCK_ID_HPP__
#define __STF_CLOCK_ID_HPP__

#include <cstdint>
#include <iostream>

#include <boost/container/flat_map.hpp>

#include "stf_exception.hpp"

namespace stf {
    /**
     * \typedef ClockId
     * Type used to represent clock IDs in transaction traces
     */
    using ClockId = uint16_t;
    static inline constexpr ClockId INVALID_CLOCK_ID = 0;

    class ClockRegistry {
        private:
            ClockId default_clock_;
            boost::container::flat_map<ClockId, std::string> clocks_;

            ClockRegistry() = default;

            static inline ClockRegistry& get_() {
                static ClockRegistry registry;
                return registry;
            }

            inline void registerClock_(const ClockId clock_id, const std::string& name) {
                stf_assert(clock_id != INVALID_CLOCK_ID, "Attempted to register invalid clock ID");

                const bool first_clock = clocks_.empty();
                const auto result = clocks_.try_emplace(clock_id, name);
                // If there's an ID collision, make sure the names match. Otherwise it's an error.
                stf_assert(result.second || result.first->second == name,
                           "Attempted to re-register clock " << clock_id << ":" << name << ". "
                           "Existing registration was " << result.first->first << ":" << result.first->second);
                if(STF_EXPECT_FALSE(first_clock)) {
                    default_clock_ = clock_id;
                }
            }

            inline ClockId getDefaultClock_() const {
                stf_assert(!clocks_.empty(), "Attempted to get default clock before registering any clocks!");
                return default_clock_;
            }

            inline const std::string& getClockName_(const ClockId clock_id) const {
                return clocks_.at(clock_id);
            }

        public:
            static inline void registerClock(const ClockId clock_id, const std::string& name) {
                get_().registerClock_(clock_id, name);
            }

            static inline void registerClock(const ClockId clock_id, const std::string_view name) {
                get_().registerClock_(clock_id, std::string(name));
            }

            static inline ClockId getDefaultClock() {
                return get_().getDefaultClock_();
            }

            static inline const std::string& getClockName(const ClockId clock_id) {
                return get_().getClockName_(clock_id);
            }
    };
} // end namespace stf

#endif
