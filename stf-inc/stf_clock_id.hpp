#ifndef __STF_CLOCK_ID_HPP__
#define __STF_CLOCK_ID_HPP__

#include <cstdint>
#include <iostream>
#include <vector>

#include "boost_wrappers/flat_map.hpp"

#include "stf_exception.hpp"

namespace stf {
    /**
     * \typedef ClockId
     * Type used to represent clock IDs in transaction traces
     */
    using ClockId = uint8_t;
    static inline constexpr ClockId INVALID_CLOCK_ID = 0; /**< Value used to indicate an invalid clock ID */

    /**
     * \class ClockRegistry
     *
     * Singleton class that records clock ID -> name mappings and the default clock ID.
     * The default clock ID is the first clock ID that appears in the trace header.
     */
    class ClockRegistry {
        private:
            ClockId default_clock_;

            using MapType = boost::container::flat_map<ClockId, std::string>;
            MapType clocks_;

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

            using DumpType = std::vector<MapType::value_type>;
            inline DumpType dumpClocks_() const {
                DumpType clocks;
                const auto default_clock = clocks_.find(getDefaultClock_());
                stf_assert(default_clock != clocks_.end(),
                           "Default clock could not be found in clock registry");
                clocks.emplace_back(*default_clock);
                for(auto it = clocks_.begin(); it != clocks_.end(); ++it) {
                    if(STF_EXPECT_TRUE(it != default_clock)) {
                        clocks.emplace_back(*it);
                    }
                }

                return clocks;
            }

        public:
            /**
             * Registers a new clock
             * \param clock_id ID of the clock
             * \param name Name of the clock
             */
            static inline void registerClock(const ClockId clock_id, const std::string& name) {
                get_().registerClock_(clock_id, name);
            }

            /**
             * Registers a new clock
             * \param clock_id ID of the clock
             * \param name Name of the clock
             */
            static inline void registerClock(const ClockId clock_id, const std::string_view name) {
                get_().registerClock_(clock_id, std::string(name));
            }

            /**
             * Gets the default clock ID
             */
            static inline ClockId getDefaultClock() {
                return get_().getDefaultClock_();
            }

            /**
             * Gets the name associated with a clock ID
             * \param clock_id clock ID to look up
             */
            static inline const std::string& getClockName(const ClockId clock_id) {
                return get_().getClockName_(clock_id);
            }

            /**
             * Dumps clocks into a vector with the default clock in the first element
             */
            static inline DumpType dumpClocks() {
                return get_().dumpClocks_();
            }
    };
} // end namespace stf

#endif
