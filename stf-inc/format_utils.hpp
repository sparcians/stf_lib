#ifndef __FORMAT_UTILS_HPP__
#define __FORMAT_UTILS_HPP__

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <stack>
#include <string>
#include <type_traits>

#include "stf_exception.hpp"
#include "util.hpp"

namespace stf {
    /**
     * \class format_utils
     *
     * Defines a set of useful functions for formatting strings/output with padding and alignment
     *
     */
    class format_utils {
        private:
            static bool show_phys_; /**< If true, show physical addresses */
            /**
             * Formats the given value to the specified width, padding with the specified character
             * \note Does not preserve flags!
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline void formatWidth_(OStream&& os, const T& val, const int width, const char pad_char = ' ') {
                os << std::setw(width) << std::setfill(pad_char) << val;
            }

            /**
             * Left-justifies the given value to the specified width, padding with the specified character
             * \note Does not preserve flags!
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline void formatLeft_(OStream&& os, const T& val, const int width = 0, const char pad_char = ' ') {
                os << std::left;
                formatWidth_(os, val, width, pad_char);
                os << std::right;
            }

        public:
            static constexpr int LABEL_WIDTH = 20; /**< Width of the leftmost (label) column*/
            static constexpr int VA_WIDTH = 16; /**< Width of a virtual address, in hex*/
            static constexpr int PA_WIDTH = 10; /**< Width of a physical address, in hex*/
            static constexpr int DATA_WIDTH = 16; /**< Width of raw data, in hex*/
            static constexpr int OPCODE_WIDTH = 8; /**< Width of an opcode, in hex*/
            static constexpr int OPCODE_FIELD_WIDTH = 21; /**< Total width of the opcode field*/
            static constexpr int MEM_ACCESS_FIELD_WIDTH = 13; /**< Total width of the MEM_ATTR field*/
            static constexpr int OPERAND_LABEL_LEFT_PADDING = LABEL_WIDTH + VA_WIDTH + 1; /**< Left padding used for operand label*/
            static constexpr int OPERAND_LABEL_RIGHT_PADDING = 4; /**< Right padding used for operand label*/
            static constexpr int OPERAND_LABEL_WIDTH = OPERAND_LABEL_LEFT_PADDING + OPERAND_LABEL_RIGHT_PADDING; /**< Total width of operand field*/
            static constexpr int TID_WIDTH = 8; /**< Width of a TID/PID/ASID, in hex*/
            static constexpr int REGISTER_NAME_WIDTH = 22; /**< Width of the register name field*/
            static constexpr int EVENT_WIDTH = 22; /**< Width of an event code string representation */
            static constexpr int EVENT_DATA_WIDTH = 8; /**< Width of event data, in hex*/
            static constexpr int ESCAPE_WIDTH = 4; /**< Width of an escape record, in decimal*/
            static constexpr int PTE_ACCESS_INDEX_WIDTH = 16; /**< Width of a PTE access index, in decimal*/
            static constexpr int PTE_DESC_LABEL_WIDTH = LABEL_WIDTH + 4; /**< Width of a PTE descriptor label*/

            /**
             * \class FlagSaver
             *
             * Object that uses RAII to save and restore the format flags for an object that implements
             * saveFlags/restoreFlags methods (e.g. OutputFileStream)
             */
            template<typename OStream,
                     typename charT = char,
                     class traits = std::char_traits<charT>,
                     bool = std::is_base_of<std::basic_ios<charT, traits>, OStream>::value>
            class FlagSaver {
                private:
                    OStream& os_;

                public:
                    /**
                     * Constructs a FlagSaver
                     * \param os Stream whose flags will be saved
                     */
                    explicit FlagSaver(OStream& os) :
                        os_(os)
                    {
                        os_.saveFlags();
                    }

                    ~FlagSaver() {
                        os_.restoreFlags();
                    }
            };

            /**
             * \class FlagSaver<OStream, charT, traits, true>
             *
             * Object that uses RAII to save and restore the format flags for an object inheriting
             * from std::basic_ios (e.g. std::ostream)
             */
            template<typename OStream, typename charT, typename traits>
            class FlagSaver<OStream, charT, traits, true> {
                private:
                    OStream& os_;
                    std::stack<std::ios> flags_;

                    /**
                     * Restores flags. Only call if flags_ is not empty.
                     */
                    inline void restoreFlags_() {
                        os_.copyfmt(flags_.top());
                        flags_.pop();
                    }

                public:
                    /**
                     * Constructs a FlagSaver
                     * \param os Stream whose flags will be saved
                     */
                    explicit FlagSaver(OStream& os) :
                        os_(os),
                        flags_()
                    {
                        saveFlags();
                    }

                    ~FlagSaver() {
                        while(!flags_.empty()) {
                            restoreFlags_();
                        }
                    }

                    /**
                     * Manually save flags
                     */
                    inline void saveFlags() {
                        flags_.emplace(nullptr);
                        flags_.top().copyfmt(os_);
                    }

                    /**
                     * Manually restore flags. Does nothing if no flags have been saved.
                     */
                    inline void restoreFlags() {
                        if(!flags_.empty()) {
                            restoreFlags_();
                        }
                    }
            };

            /**
             * Gets the number of hex digits required to represent the given type
             */
            template<typename T>
            static constexpr int numHexDigits()
            {
                return numHexDigits(sizeof(T));
            }

            /**
             * Gets the number of hex digits required to represent the given width
             */
            static constexpr int numHexDigits(const size_t length)
            {
                constexpr int HEX_DIGITS_PER_BYTE = 2; // Number of digits in the hex representation of a byte
                return static_cast<int>(HEX_DIGITS_PER_BYTE * length);
            }

            /**
             * Formats the given value to the specified width, padding with the specified character
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline void formatWidth(OStream&& os, const T& val, const int width, const char pad_char = ' ') {
                FlagSaver flags(os);
                formatWidth_(os, val, width, pad_char);
            }

            /**
             * Left-justifies the given value to the specified width, padding with the specified character
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline void formatLeft(OStream&& os, const T& val, const int width = 0, const char pad_char = ' ') {
                FlagSaver flags(os);
                formatLeft_(os, val, width, pad_char);
            }

            /**
             * Left-justifies the given value in decimal to the specified width, padding with the specified character
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatDecLeft(OStream&& os, const T val, const int width = 0, const char pad_char = ' ') {
                FlagSaver flags(os);
                os << std::dec;
                formatLeft_(os, val, width, pad_char);
            }

            /**
             * Formats the given value in hex to the specified width, padding with the specified character
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatHex(OStream&& os, const T val, const int width = numHexDigits<T>(), const char pad_char = '0') {
                FlagSaver flags(os);
                os << std::hex;
                formatWidth_(os, val, width, pad_char);
            }

            /**
             * Formats the given uint8_t in hex to the specified width, padding with the specified character
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream>
            static inline void formatHex(OStream&& os,
                                         const uint8_t val,
                                         const int width,
                                         const char pad_char) {
                formatHex(os, static_cast<uint16_t>(val), width, pad_char);
            }

            /**
             * Formats the given value in decimal to the specified width, padding with the specified character
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param pad_char padding character
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatDec(OStream&& os, const T val, const int width = 0, const char pad_char = '0') {
                FlagSaver flags(os);
                os << std::dec;
                formatWidth_(os, val, width, pad_char);
            }

            /**
             * Formats a label
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline void formatLabel(OStream&& os, const T& val) {
                formatLeft(os, val, LABEL_WIDTH);
            }

            /**
             * Formats an operand label
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline void formatOperandLabel(OStream&& os, const T& val) {
                formatWidth(os, val, OPERAND_LABEL_WIDTH);
            }

            /**
             * Formats a register name
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline void formatRegisterName(OStream&& os, const T& val) {
                formatLeft(os, val, REGISTER_NAME_WIDTH);
            }

            /**
             * Formats a virtual address
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatVA(OStream&& os, const T val) {
                formatHex(os, val, VA_WIDTH);
            }

            /**
             * Formats a physical address
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatPA(OStream&& os, const T val) {
                formatHex(os, val, PA_WIDTH);
            }

            /**
             * Formats a raw data value
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatData(OStream&& os, const T val) {
                formatHex(os, val, DATA_WIDTH);
            }

            /**
             * Formats an opcode
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatOpcode(OStream&& os, const T val) {
                formatHex(os, val, OPCODE_WIDTH);
            }

            /**
             * Formats a PID/TID/ASID
             * \param os ostream to write result to
             * \param val value to format
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_integral<T>::value>::type
            formatTID(OStream&& os, const T val) {
                formatHex(os, val, TID_WIDTH);
            }

            /**
             * Generates the specified number of spaces
             * \param os ostream to write result to
             * \param num_spaces number of spaces to generate
             */
            template<typename OStream>
            static inline void formatSpaces(OStream&& os, const size_t num_spaces) {
                os << std::string(num_spaces, ' '); // Faster than std::setw
            }

            /**
             * Sets whether physical addresses should be included
             * \param show_phys If true, physical addresses will be shown
             */
            static void setShowPhys(bool show_phys) {
                show_phys_ = show_phys;
            }

            /**
             * Returns whether physical addresses should be included
             */
            static bool showPhys() {
                return show_phys_;
            }

            /**
             * Formats a floating point value with the specified width and precision
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param precision number of digits to include after decimal point - omit for default precision
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_floating_point<T>::value>::type
            formatFloat(OStream&& os, const T val, const int width = 0, const int precision = -1) {
                FlagSaver flags(os);
                if(precision >= 0) {
                    os << std::setprecision(precision);
                }
                os << std::fixed << std::setw(width) << val;
            }

            /**
             * Formats a floating point value as a percentage with the specified width and precision
             * \param os ostream to write result to
             * \param val value to format
             * \param width width to format to
             * \param precision number of digits to include after decimal point - omit for default precision
             */
            template<typename OStream, typename T>
            static inline typename std::enable_if<std::is_floating_point<T>::value>::type
            formatPercent(OStream&& os, const T val, const int width = 0, const int precision = -1) {
                formatFloat(os, 100.0*val, width, precision);
                os << '%';
            }

            /**
             * Formats an event type, padding with spaces if necessary. Intended for use with EventRecord::TYPE.
             * \param os ostream to write result to
             * \param event_type Event type to format
             */
            template<typename OStream, typename EventT>
            static inline void formatEvent(OStream&& os, const EventT event_type) {
                FlagSaver flags(os);
                os << std::left << std::setw(EVENT_WIDTH) << std::setfill(' ') << event_type;
            }

            /**
             * Formats a vector type, padding with spaces if necessary. Intended for use with vector InstRegRecords.
             * \param os ostream to write result to
             * \param vec Vector to format
             * \param vlen Vlen to use to format vector data
             * \param indent Number of characters to indent the line
             * \param indent_first_line If false, first line will not be indented
             */
            template<typename OStream, typename VectorT>
            static inline void formatVector(OStream&& os,
                                            const VectorT& vec,
                                            const size_t vlen,
                                            const size_t indent = 0,
                                            const bool indent_first_line = true) {
                static constexpr size_t VECTOR_ELEMENT_WIDTH = byte_utils::bitSize<typename VectorT::value_type>();
                FlagSaver flags(os);
                bool first_line = true;
                if(vlen < VECTOR_ELEMENT_WIDTH) {
                    stf_assert(VECTOR_ELEMENT_WIDTH % vlen == 0,
                               "The vlen parameter ("
                               << vlen
                               << ") must evenly divide the vector element size ("
                               << VECTOR_ELEMENT_WIDTH
                               << ")");

                    const typename VectorT::value_type mask = (1 << vlen) - 1;
                    const int vlen_digits = numHexDigits(vlen);

                    for(auto it = vec.rbegin(); it != vec.rend(); ++it) {
                        for(auto i = static_cast<ssize_t>(VECTOR_ELEMENT_WIDTH - vlen); i >= 0; i -= vlen) {
                            if(!first_line) {
                                os << std::endl;
                            }
                            if(!first_line || indent_first_line) {
                                formatSpaces(os, indent);
                            }
                            formatHex(os, (*it >> i) & mask, vlen_digits);
                            if(first_line) {
                                first_line = false;
                            }
                        }
                    }
                }
                else if(vlen > VECTOR_ELEMENT_WIDTH) {
                    stf_assert(vlen % VECTOR_ELEMENT_WIDTH == 0,
                               "The vector element size ("
                               << VECTOR_ELEMENT_WIDTH
                               << ") must evenly divide the vlen parameter ("
                               << vlen
                               << ")");
                    const size_t elements_per_vlen = vlen / VECTOR_ELEMENT_WIDTH;
                    stf_assert(vec.size() % elements_per_vlen == 0,
                               "The number of vector elements must be a multiple of the number of elements per vlen");
                    auto it = vec.rbegin();
                    while(it != vec.rend()) {
                        if(!first_line) {
                            os << std::endl;
                        }
                        if(!first_line || indent_first_line) {
                            formatSpaces(os, indent);
                        }
                        for(size_t i = 0; i < elements_per_vlen; ++i) {
                            formatHex(os, *it);
                            ++it;
                        }
                        if(first_line) {
                            first_line = false;
                        }
                    }
                }
                else {
                    for(auto it = vec.rbegin(); it != vec.rend(); ++it) {
                        if(!first_line) {
                            os << std::endl;
                        }
                        if(!first_line || indent_first_line) {
                            formatSpaces(os, indent);
                        }
                        formatSpaces(os, indent);
                        formatHex(os, *it);
                        if(first_line) {
                            first_line = false;
                        }
                    }
                }
            }

    };
} // end namespace stf

#endif
