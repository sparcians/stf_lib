
// <STFSymTab> -*- HPP -*-

#ifndef __STFSymTab_HPP__
#define __STFSymTab_HPP__

#include "STFSymTabBase.hpp"

namespace stf {
    /**
     * \class STFSymTab
     *
     * Class that holds an STF symbol table
     *
     */
    class STFSymTab : public STFSymTabBase {

     public:
       STFSymTab() = default;
       ~STFSymTab() override = default;
       bool parse_yamlSt(const char* fname_symTab_yaml);    /**< Parse a YAML symbol table file entries into current hash map*/
       void insertSymbol(uint64_t key, symType new_symbol); /**< Add a new symbol to current symbol table hash*/
       symType getSymbol(uint64_t key);                     /**< Given a virtual addr key, return the symbol info*/
       uint64_t get_st_size();                              /**< return symbol count in current symbol table*/
    };

    // STFSymTab class method implementation specific
    // YAML Symbol Table Parsing Events (parser_event_count % YAML_SYM_TAB_EVENTS_MAX)
    static constexpr unsigned int YAML_SYM_TAB_EVENT_MAX     = 4;
    static constexpr unsigned int YAML_SYM_TAB_EVENT_VADDR   = (1 % YAML_SYM_TAB_EVENT_MAX);
    static constexpr unsigned int YAML_SYM_TAB_EVENT_ELFNAME = (2 % YAML_SYM_TAB_EVENT_MAX);
    static constexpr unsigned int YAML_SYM_TAB_EVENT_SYMNAME = (3 % YAML_SYM_TAB_EVENT_MAX);
    static constexpr unsigned int YAML_SYM_TAB_EVENT_OPCODE  = (4 % YAML_SYM_TAB_EVENT_MAX);
} // end namespace stf

// __STFSymTab_HPP__
#endif
