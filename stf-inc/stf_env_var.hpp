#ifndef __STF_ENV_VAR_HPP__
#define __STF_ENV_VAR_HPP__

#include <cstdlib>
#include <cstring>
#include <set>
#include <string>

#include "stf_exception.hpp"

namespace stf {
    /**
     * \class STFEnvVar
     * Provides a convenient interface to read environment variables
     */
    class STFEnvVar {
        private:
            const std::string var_name_;
            const std::set<std::string> allowed_values_;
            const std::string default_value_;

        public:
            /**
             * Constructs an STFEnvVar
             * \param var_name Name of the environment variable
             * \param allowed_values List of allowed values
             */
            STFEnvVar(const std::string& var_name,
                      std::initializer_list<std::string> allowed_values) :
                STFEnvVar(var_name, allowed_values, "")
            {
            }

            /**
             * Constructs an STFEnvVar
             * \param var_name Name of the environment variable
             * \param allowed_values List of allowed values
             * \param default_value Default value that should be returned if the variable is unset
             */
            STFEnvVar(const std::string& var_name,
                      std::initializer_list<std::string> allowed_values,
                      const std::string& default_value) :
                var_name_(var_name),
                allowed_values_(allowed_values),
                default_value_(default_value)
            {
            }

            /**
             * Gets the value of the variable
             * \returns Value of the variable, or the default value if it is unset
             */
            inline const std::string& get() const {
                const char* val = std::getenv(var_name_.c_str());
                if(!val) {
                    return default_value_;
                }

                const auto it = allowed_values_.find(val);
                stf_assert(it != allowed_values_.end(),
                           "Invalid value specified for "
                           << var_name_
                           << ": "
                           << val
                           << ". Allowed values are: "
                           << allowed_values_);

                return *it;
            }
    };

    /**
     * \class STFBooleanEnvVar
     * Interface for reading environment variables that should be interpreted as boolean values
     */
    class STFBooleanEnvVar : public STFEnvVar {
        public:
            /**
             * Constructs an STFBooleanEnvVar
             * \param var_name Name of the environment variable
             * \param default_value Default value that should be returned if the variable is unset
             */
            STFBooleanEnvVar(const std::string& var_name,
                             const bool default_value = false) :
                STFEnvVar(var_name,
                          {"0", "false", "1", "true"},
                          default_value ? "true": "false")
            {
            }

            /**
             * Gets the value of the variable
             * \returns Value of the variable, or the default value if it is unset
             */
            inline bool get() const {
                const auto& string_val = STFEnvVar::get();
                if((string_val == "true") || (string_val == "1")) {
                    return true;
                }

                return false;
            }

            /**
             * bool conversion operator
             * \returns Value of the variable, or the default value if it is unset
             */
            inline operator bool() const {
                return get();
            }
    };
} // end namespace stf

#endif
