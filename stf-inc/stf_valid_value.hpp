#ifndef __STF_VALID_VALUE_HPP__
#define __STF_VALID_VALUE_HPP__

#include <utility>
#include "stf_exception.hpp"

namespace stf {
    /**
     * \class ValidValue
     *
     * Object that wraps a value with a valid flag
     */
    template<typename T>
    class ValidValue {
        private:
            T val_ = T(); /**< Value */
            bool valid_ = false; /**< Valid flag */

        public:
            ValidValue() = default;

            /**
             * Constructs a ValidValue with the specified value
             * \param val Value to initialize the object with
             */
            explicit ValidValue(const T val) :
                val_(std::move(val)),
                valid_(true)
            {
            }

            /**
             * Assignment operator
             * \param val Value to set
             */
            ValidValue& operator=(const T val) {
                val_ = std::move(val);
                valid_ = true;
                return *this;
            }

            /**
             * Invalidates the value
             */
            void invalidate() {
                valid_ = false;
            }

            /**
             * Returns whether the value is valid
             */
            bool valid() const {
                return valid_;
            }

            /**
             * Gets the underlying value
             */
            explicit operator T() const {
                return get();
            }

            /**
             * Gets the underlying value (non-ref version)
             */
            template<typename U = T>
            typename std::enable_if<std::is_scalar<U>::value, U>::type
            get() const {
                stf_assert(valid_, "Attempted to access invalid ValidValue");
                return val_;
            }

            /**
             * Gets the underlying value (const ref version)
             */
            template<typename U = T>
            typename std::enable_if<!std::is_scalar<U>::value, const U&>::type
            get() const {
                stf_assert(valid_, "Attempted to access invalid ValidValue");
                return val_;
            }
    };

} // end namespace stf

#endif
