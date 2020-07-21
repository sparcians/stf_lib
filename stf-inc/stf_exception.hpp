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

            /// Destroy!
            ~STFException() noexcept override = default;

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
            STFException & operator<<(const T & msg) {
                std::stringstream str;
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
 * \def STF_EXPECT_TRUE
 * Convenience macro to mark a conditional as likely-true
 */
#define STF_EXPECT_TRUE(x) __builtin_expect(x, 1)

/**
 * \def STF_EXPECT_FALSE
 * Convenience macro to mark a conditional as likely-false
 */
#define STF_EXPECT_FALSE(x) __builtin_expect(x, 0)

/**
 * \def ADD_FILE_INFORMATION
 * Convenience macro to add file information to an exception
 */
#define ADD_FILE_INFORMATION(ex, file, line)                \
    ex << ": in file: '" << file << "', on line: " << std::dec << line;

/**
 * \def STF_THROW_EXCEPTION
 * For internal use only
 */
#define STF_THROW_EXCEPTION(reason, file, line)              \
    stf::STFException ex(reason);                            \
    ADD_FILE_INFORMATION(ex, file, line)                     \
    throw ex;

/**
 * \def stf_assert1
 * For internal use only
 */
#define stf_assert1(e) \
    if(__builtin_expect(!(e), 0)) { STF_THROW_EXCEPTION(#e, __FILE__, __LINE__) }

/**
 * \def stf_assert2
 * For internal use only
 */
#define stf_assert2(e, insertions)                                                 \
    if(__builtin_expect(!(e), 0)) { stf::STFException ex(std::string(#e) + ": " ); \
                                    ex << insertions;                              \
                                    ADD_FILE_INFORMATION(ex, __FILE__, __LINE__);  \
                                    throw ex; }

/**
 * \def VA_NARGS_IMPL
 * For internal use only
 */
#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
/**
 * \def VA_NARGS
 * For internal use only
 */
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
/**
 * \def stf_assert_impl2
 * For internal use only
 */
#define stf_assert_impl2(count, ...) stf_assert##count(__VA_ARGS__)
/**
 * \def stf_assert_impl
 * For internal use only
 */
#define stf_assert_impl(count, ...)  stf_assert_impl2(count, __VA_ARGS__)
/**
 * \def stf_assert
 * Throws an stf::STFException if the provided condition is false
 */
#define stf_assert(...) stf_assert_impl(VA_NARGS(__VA_ARGS__), __VA_ARGS__)

/**
 * \def stf_throw
 * Throws an stf::STFException with the provided message (supports << operator on message)
 */
#define stf_throw(message) \
    { \
        std::stringstream msg; \
        msg << message; \
        stf::STFException ex(std::string("abort: ") + msg.str()); \
        ADD_FILE_INFORMATION(ex, __FILE__, __LINE__); \
        throw ex; \
    }

/**
 * \def invalid_descriptor_throw
 * Throws an stf::InvalidDescriptorException with the provided message (supports << operator on message)
 */
#define invalid_descriptor_throw(message) \
    { \
        std::stringstream msg; \
        msg << message; \
        stf::InvalidDescriptorException ex(std::string("abort: ") + msg.str()); \
        ADD_FILE_INFORMATION(ex, __FILE__, __LINE__); \
        throw ex; \
    }

#endif //__STF_EXCEPTION_HPP__
