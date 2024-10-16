// <STFException> -*- HPP -*-

/**
 * \file   stf_exception.hpp
 *
 * \brief  Exception class for all of STF.
 *
 *
 */

#ifndef __STF_EXCEPTION_HPP__
#define __STF_EXCEPTION_HPP__

#include <exception>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "type_utils.hpp"

namespace stf
{
    /**
     * \class STFException
     *
     * Used to construct and throw a standard C++ exception.  Inherits
     * from std::exception.
     */
    class STFException : public std::exception
    {
        public:
            /**
             * \class stringstream
             * Class used to construct STFException messages
             */
            class stringstream {
                private:
                    std::ostringstream ss_;

                public:
                    stringstream() = default;

                    /**
                     * Constructs a stringstream, initializing it with the specified string
                     * \param str String value to initialize stringstream with
                     */
                    explicit stringstream(const std::string& str) :
                        ss_(str, std::ostringstream::ate)
                    {
                    }

                    /**
                     * operator<< for string types
                     * \param msg String message
                     */
                    template<class T>
                    std::enable_if_t<std::disjunction_v<std::is_same<T, std::string>,
                                                                 std::is_same<T, std::string_view>,
                                                                 std::negation<type_utils::is_iterable<T>>>, stringstream&>
                    operator<<(const T & msg) {
                        ss_ << msg;
                        return *this;
                    }

                    /**
                     * operator<< for non-string iterable collections
                     * \param msg_vec Collection to format
                     */
                    template<typename Vector>
                    std::enable_if_t<std::conjunction_v<std::negation<std::is_same<Vector, std::string>>,
                                                                 std::negation<std::is_same<Vector, std::string_view>>,
                                                                 type_utils::is_iterable<Vector>>, stringstream&>
                    operator<<(const Vector& msg_vec) {
                        bool first = true;
                        *this << "[";
                        for(const auto& msg: msg_vec) {
                            if(!first) {
                                *this << ",";
                            }
                            else {
                                first = false;
                            }
                            *this << msg;
                        }
                        *this << "]";
                        return *this;
                    }

                    /**
                     * Returns the contained string
                     */
                    std::string str() const {
                        return ss_.str();
                    }
            };


            /**
             * \brief Construct a STFException object with empty reason
             *
             * Reason can be populated later with the insertion operator.
             */
            STFException() :
                reason_("")
            { }

            /**
             * \brief Construct a STFException object
             * \param reason The reason for the exception
             */
            explicit STFException(std::string reason) :
                reason_(std::move(reason))
            { }

            STFException(const STFException&) = default;
            STFException(STFException&&) = default;

            /// Destroy!
            inline ~STFException() noexcept override = default;

            /**
             * \brief Overload from std::exception
             * \return Const char * of the exception reason
             */
            const char* what() const noexcept override {
                return reason_.c_str();
            }

            /**
             * \brief Append additional information to the message.
             * \param msg The addition info
             * \return This exception object
             *
             * Usage:
             * \code
             * int bad_company = 4;
             * stf::STFException e("Oh uh");
             * e << ": this is bad: " << bad_company;
             * \endcode
             * or you can do this:
             * \code
             * int bad_company = 4;
             * throw stf::STFException e("Oh uh") << ": this is bad: " << bad_company;
             * \endcode
             * but it's not as pretty.
             */
            template<class T>
            STFException& operator<<(const T & msg) {
                stringstream str;
                str << msg;
                reason_ += str.str();
                return *this;
            }

        private:
            // You cannot use a stringstream here as it does not allow
            // copies.  Must use a string.
            std::string reason_;
    };

    /**
     * \class InvalidDescriptorException
     *
     * Exception thrown when we attempt to construct an STFRecord with an invalid descriptor
     */
    class InvalidDescriptorException : public STFException {
        public:
            InvalidDescriptorException() = default;

            /**
             * \brief Construct an InvalidDescriptorException object
             * \param reason The reason for the exception
             */
            explicit InvalidDescriptorException(const std::string& reason) :
                STFException(reason)
            {
            }
    };

    /**
     * \class EOFException
     *
     * Exception thrown to indicate we have reached the end of an STF file.  Inherits
     * from std::exception.
     */
    class EOFException : public std::exception {
    };


} //end namespace stf

/**
 * \def STF_EXPECT
 * Convenience macro to mark the likely direction of a conditional
 */
#define STF_EXPECT(x, dir) __builtin_expect(!!(x), (dir))

/**
 * \def STF_EXPECT_TRUE
 * Convenience macro to mark a conditional as likely-true
 */
#define STF_EXPECT_TRUE(x) STF_EXPECT(x, true)

/**
 * \def STF_EXPECT_FALSE
 * Convenience macro to mark a conditional as likely-false
 */
#define STF_EXPECT_FALSE(x) STF_EXPECT(x, false)

/**
 * \def ADD_FILE_INFORMATION
 * Convenience macro to add file information to an exception
 */
#define ADD_FILE_INFORMATION(ex, file, line) \
    ex << ": in file: '" << file << "', on line: " << std::dec << line;

/**
 * \def STF_THROW_EXCEPTION
 * For internal use only
 */
#define STF_THROW_EXCEPTION(reason, file, line) \
    stf::STFException ex(reason);               \
    ADD_FILE_INFORMATION(ex, file, line)        \
    throw ex;

/**
 * \def stf_throw_impl
 * For internal use only
 */
#define stf_throw_impl(prefix, message)                 \
    {                                                   \
        stf::STFException::stringstream msg(prefix);    \
        msg << ": " << message;                         \
        stf::STFException ex(msg.str());                \
        ADD_FILE_INFORMATION(ex, __FILE__, __LINE__);   \
        throw ex;                                       \
    }

/**
 * \def stf_assert1
 * For internal use only
 */
#define stf_assert1(e) \
    if(__builtin_expect(!(e), 0)) { STF_THROW_EXCEPTION(BOOST_PP_STRINGIZE(e), __FILE__, __LINE__) }

/**
 * \def stf_assert2
 * For internal use only
 */
#define stf_assert2(e, insertions) \
    if(__builtin_expect(!(e), 0)) { stf_throw_impl(BOOST_PP_STRINGIZE(e), insertions) }

/**
 * \def stf_assert
 * Throws an stf::STFException if the provided condition is false
 */
#define stf_assert(...) BOOST_PP_OVERLOAD(stf_assert, __VA_ARGS__)(__VA_ARGS__)

/**
 * \def stf_throw
 * Throws an stf::STFException with the provided message (supports << operator on message)
 */
#define stf_throw(message) stf_throw_impl("abort", message)

/**
 * \def invalid_descriptor_throw
 * Throws an stf::InvalidDescriptorException with the provided message (supports << operator on message)
 */
#define invalid_descriptor_throw(message)                                       \
    {                                                                           \
        std::stringstream msg;                                                  \
        msg << message;                                                         \
        stf::InvalidDescriptorException ex(std::string("abort: ") + msg.str()); \
        ADD_FILE_INFORMATION(ex, __FILE__, __LINE__);                           \
        throw ex;                                                               \
    }

#endif //__STF_EXCEPTION_HPP__
