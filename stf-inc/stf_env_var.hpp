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
        protected:
            const std::string val_; /**< Value of the environment variable */

            /**
             * Gets the value of the environment variable, returning default_value if it is not defined
             * \param var_name Environment variable name
             * \param default_value Default value if the variable is undefined
             */
            static inline std::string getVal_(const std::string& var_name, const std::string& default_value) {
                const char* val = std::getenv(var_name.c_str());
                if(!val) {
                    return default_value;
                }

                return val;
            }

        public:
            /**
             * Constructs an STFEnvVar
             * \param var_name Name of the environment variable
             */
            explicit STFEnvVar(const std::string& var_name) :
                STFEnvVar(var_name, "")
            {
            }

            /**
             * Constructs an STFEnvVar
             * \param var_name Name of the environment variable
             * \param default_value Default value that should be returned if the variable is unset
             */
            STFEnvVar(const std::string& var_name,
                      const std::string& default_value) :
                val_(getVal_(var_name, default_value))
            {
            }

            /**
             * Gets the value of the variable
             * \returns Value of the variable, or the default value if it is unset
             */
            inline const std::string& get() const {
                return val_;
            }
    };

    /**
     * \class STFValidatedEnvVar
     * Validates environment variable values against a predefined list of allowed values
     */
    class STFValidatedEnvVar : public STFEnvVar {
        private:
            const std::set<std::string> allowed_values_;

        public:
            /**
             * Constructs an STFValidatedEnvVar
             * \param var_name Name of the environment variable
             * \param allowed_values List of allowed values
             */
            STFValidatedEnvVar(const std::string& var_name,
                      std::initializer_list<std::string> allowed_values) :
                STFValidatedEnvVar(var_name, allowed_values, "")
            {
            }

            /**
             * Constructs an STFValidatedEnvVar
             * \param var_name Name of the environment variable
             * \param allowed_values List of allowed values
             * \param default_value Default value that should be returned if the variable is unset
             */
            STFValidatedEnvVar(const std::string& var_name,
                      std::initializer_list<std::string> allowed_values,
                      const std::string& default_value) :
                STFEnvVar(var_name, default_value),
                allowed_values_(allowed_values)
            {
                const auto it = allowed_values_.find(val_);
                stf_assert(it != allowed_values_.end(),
                           "Invalid value specified for "
                           << var_name
                           << ": "
                           << val_
                           << ". Allowed values are: "
                           << allowed_values_);
            }
    };

    /**
     * \class STFBooleanEnvVar
     * Interface for reading environment variables that should be interpreted as boolean values
     */
    class STFBooleanEnvVar : public STFValidatedEnvVar {
        private:
            const bool bool_val_;

        public:
            /**
             * Constructs an STFBooleanEnvVar
             * \param var_name Name of the environment variable
             * \param default_value Default value that should be returned if the variable is unset
             */
            explicit STFBooleanEnvVar(const std::string& var_name,
                             const bool default_value = false) :
                STFValidatedEnvVar(var_name,
                                   {"0", "false", "1", "true"},
                                   default_value ? "true": "false"),
                bool_val_((val_ == "true") || (val_ == "1"))
            {
            }

            /**
             * Gets the value of the variable
             * \returns Value of the variable, or the default value if it is unset
             */
            inline bool get() const {
                return bool_val_;
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
