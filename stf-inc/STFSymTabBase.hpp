
// <STFSymTabBase> -*- HPP -*-

#ifndef __STFSymTabBase_HPP__
#define __STFSymTabBase_HPP__

#include <iostream>
#include <unordered_map>
#include <string>

namespace stf {
    /**
     * \class STFSymTabBase
     *
     * Base class for STF symbol table
     *
     */
    class STFSymTabBase {

     protected:
        /**
         * \struct symType
         * Symbol info structure
         */
        struct symType
        {
           std::string symName;   /**< Symbol i.e. function name*/
           std::string libName;   /**< Symbol ELF (lib/bin) name. If libName is post-fixed by <Untrusted-OPCODE_MISMATCH>, then be WARNED that it is untrustworthy entry in YAML i.e. IMEM and ELF did not agree on this symbol's opcode*/
           uint64_t opcode;       /**< Symbol opcode (integer, why? so that cross-compare against instruction opcode from trace is cheap)*/
        };

       std::unordered_map<uint64_t, symType> st_map; /**< symbol table map */
       std::unordered_map<uint64_t, symType>::iterator st_map_iterator; /**< iterator into the symbol table map */
       /** Dummy Unknown Symbol. Used to return missing symbol
        * i.e. symbol is missing from YAML symbol table provided
        */
       symType sym_unknown = {"Unknown.10", "Unknown.10", 0};

     public:
       STFSymTabBase() = default;
       virtual ~STFSymTabBase() = default;
       bool parse_yamlSt(const char* fname_symTab_yaml);    /**< Parse a YAML symbol table file entries into current hash map*/
       void insertSymbol(uint64_t key, symType new_symbol); /**< Add a new symbol to current symbol table hash*/
       symType getSymbol(uint64_t key);                     /**< Given a virtual addr key, return the symbol info*/
       uint64_t get_st_size();                              /**< return symbol count in current symbol table*/
    };
} // end namespace stf

// __STFSymTabBase_HPP__
#endif
