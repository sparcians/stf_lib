#ifndef __STF_OBJECT_ID_HPP__
#define __STF_OBJECT_ID_HPP__

#include <type_traits>

namespace stf {
    /**
     * \struct ObjectIdConverter
     *
     * Converts object IDs between trace and internal values
     */
    struct ObjectIdConverter {
        /**
         * Converts an enum value from a trace value to the internal value.
         *
         * Defaults to no conversion. Can be specialized for STFObjects that use different internal and external ID values (e.g. STFRecord)
         *
         * \param val Value to convert
         */
        template<typename TraceEnum>
        static inline constexpr auto fromTrace(const TraceEnum val) {
            static_assert(std::is_enum_v<TraceEnum>, "ObjectIdConverter can only be used with enum types");
            return val;
        }

        /**
         * Converts an enum value from an internal value to the trace value.
         *
         * Defaults to no conversion. Can be specialized for STFObjects that use different internal and external ID values (e.g. STFRecord)
         *
         * \param val Value to convert
         */
        template<typename InternalEnum>
        static inline constexpr auto toTrace(const InternalEnum val) {
            static_assert(std::is_enum_v<InternalEnum>, "ObjectIdConverter can only be used with enum types");
            return val;
        }
    };
} // end namespace stf

#endif
