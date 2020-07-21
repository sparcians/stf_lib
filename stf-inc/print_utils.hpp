#ifndef __PRINT_UTILS_HPP__
#define __PRINT_UTILS_HPP__

#include <iostream>
#include <string_view>
#include "format_utils.hpp"
#include "stf_exception.hpp"

namespace stf {
    /**
     * \struct print_utils
     *
     * Struct containing convenience functions for printing formatted output to std::cout
     */
    struct print_utils {
        /**
         * Prints the given value to stdout with the specified width, padding with the specified character
         * \param val value to format
         * \param width width to format to
         * \param pad_char padding character
         */
        template<typename T>
        static inline void printWidth(const T& val, const int width = 0, const char pad_char = ' ') {
            format_utils::formatWidth(std::cout, val, width, pad_char);
        }

        /**
         * Left-justifies the given value to stdout with the specified width, padding with the specified character
         * \param val value to format
         * \param width width to format to
         * \param pad_char padding character
         */
        template<typename T>
        static inline void printLeft(const T& val, const int width = 0, const char pad_char = ' ') {
            format_utils::formatLeft(std::cout, val, width, pad_char);
        }

        /**
         * Left-justifies the given value in decimal to stdout with the specified width, padding with the specified character
         * \param val value to format
         * \param width width to format to
         * \param pad_char padding character
         */
        template<typename T>
        static inline typename std::enable_if<std::is_integral<T>::value>::type
        printDecLeft(T val, int width = 0, char pad_char = ' ') {
            format_utils::formatDecLeft(std::cout, val, width, pad_char);
        }

        /**
         * Prints the given value in hex to stdout with the specified width, padding with the specified character
         * \param val value to format
         * \param width width to format to
         * \param pad_char padding character
         */
        template<typename T>
        static inline typename std::enable_if<std::is_integral<T>::value>::type
        printHex(T val, int width = format_utils::numHexDigits<T>(), char pad_char = '0') {
            format_utils::formatHex(std::cout, val, width, pad_char);
        }

        /**
         * Prints the given value in decimal to stdout with the specified width, padding with the specified character
         * \param val value to format
         * \param width width to format to
         * \param pad_char padding character
         */
        template<typename T>
        static inline typename std::enable_if<std::is_integral<T>::value>::type
        printDec(T val, int width = 0, char pad_char = '0') {
            format_utils::formatDec(std::cout, val, width, pad_char);
        }

        /**
         * Prints a label
         * \param val value to format
         */
        template<typename T>
        static inline void printLabel(const T& val) {
            format_utils::formatLabel(std::cout, val);
        }

        /**
         * Prints a virtual address
         * \param val value to format
         */
        template<typename T>
        static inline typename std::enable_if<std::is_integral<T>::value>::type
        printVA(const T val) {
            format_utils::formatVA(std::cout, val);
        }

        /**
         * Prints a physical address
         * \param val value to format
         */
        template<typename T>
        static inline typename std::enable_if<std::is_integral<T>::value>::type
        printPA(const T val) {
            format_utils::formatPA(std::cout, val);
        }

        /**
         * Prints a TID/PID/ASID
         * \param val value to format
         */
        template<typename T>
        static inline typename std::enable_if<std::is_integral<T>::value>::type
        printTID(const T val) {
            format_utils::formatTID(std::cout, val);
        }

        /**
         * Prints an operand label
         * \param label label to format
         */
        template<typename T>
        static inline void printOperandLabel(const T& label)
        {
            format_utils::formatOperandLabel(std::cout, label);
        }

        /**
         * Prints the specified number of spaces
         * \param num_spaces number of spaces to print
         */
        static inline void printSpaces(const size_t num_spaces) {
            format_utils::formatSpaces(std::cout, num_spaces);
        }

        /**
         * Prints a floating point value with the specified width and precision
         * \param val value to print
         * \param width width to print to
         * \param precision number of digits to include after decimal point - omit for default precision
         */
        template<typename T>
        static inline typename std::enable_if<std::is_floating_point<T>::value>::type
        printFloat(const T val, const int width = 0, const int precision = -1) {
            format_utils::formatFloat(std::cout, val, width, precision);
        }

        /**
         * Prints a floating point value as a percentage with the specified width and precision
         * \param val value to print
         * \param width width to print to
         * \param precision number of digits to include after decimal point - omit for default precision
         */
        template<typename T>
        static inline typename std::enable_if<std::is_floating_point<T>::value>::type
        printPercent(const T val, const int width = 0, const int precision = -1) {
            format_utils::formatPercent(std::cout, val, width, precision);
        }

        /**
         * Prints an opcode along with its disassembly
         * \param dis disassembler to use
         * \param inst_set instruction set to use
         * \param opcode instruction opcode
         * \param pc instruction PC
         */
        template<typename DisassemblerT, typename InstSetT>
        static inline void printOpcodeWithDisassembly(DisassemblerT& dis,
                                                      const InstSetT inst_set,
                                                      const uint32_t opcode,
                                                      const uint64_t pc)
        {
            static constexpr int OPCODE_PADDING = format_utils::OPCODE_FIELD_WIDTH - format_utils::OPCODE_WIDTH - 1;

            dis.printOpcode(inst_set, opcode);

            printSpaces(OPCODE_PADDING); // pad out the rest of the opcode field with spaces

            dis.printDisassembly(pc, inst_set, opcode);
            // if (show_annotation)
            // {
            //     // Retrieve symbol information from symbol table hash map
            //     symInfo = annoSt->getSymbol(dism_pc);
            //     if (match_symbol_opcode)
            //     {
            //         if(symInfo.opcode != inst.opcode())
            //             std::cout << " | " << " [ " << symInfo.libName << ", " << symInfo.symName << ", OPCODE_MISMATCH: " << std::hex  << symInfo.opcode << " ] ";
            //         else
            //             std::cout << " | " << " [ " << symInfo.libName << ", " << symInfo.symName << ", OPCODE_CROSSCHECKED"  << " ] ";
            //     }
            //     else
            //         std::cout << " | " << " [ " << symInfo.libName << ", " << symInfo.symName << " ] ";
            // }
            std::cout << std::endl;
        }
    };
} // end namespace stf

#endif
