#ifndef __STF_ISA_UTILS_HPP__
#define __STF_ISA_UTILS_HPP__

#include <string>

namespace stf {
    /**
     * \class RISCVISAStringParser
     * Provides *very* rudimentary RISC-V ISA string parsing
     * Right now this can only determine if an extension appears in an ISA string
     */
    class RISCVISAStringParser {
        private:
            static inline bool findMulticharExtension_(const std::string& isa_string, const std::string& extension, size_t start_pos = 0) {
                const auto search_str = std::string(1, '_') + extension;

                const auto find_next = [&isa_string, &search_str, &start_pos]() {
                    return isa_string.find(search_str, start_pos);
                };

                for(auto pos = find_next(); pos != std::string::npos; pos = find_next()) {
                    // We only reach this point if the extension's string appears somewhere in the ISA string
                    const auto search_str_end = pos + search_str.size();

                    // For a multi-character extension ext, it will appear in one of the following forms:
                    // 1. _ext (at the end of the ISA string)
                    // 2. _ext_ (somewhere in the middle of the ISA string)
                    // 3. _ext[0-9] (anywhere in the string. The number indicates the extension version)

                    // First check for case 1
                    if(search_str_end == isa_string.size()) {
                        return true;
                    }

                    // Check for cases 2 and 3. The main point of this is to make sure we aren't accidentally
                    // matching against part of another extension substring
                    const auto end_of_extension = isa_string.find_first_of("0123456789_", search_str_end);

                    // If none of the end-of-extension characters appear in the string, then this extension can't be present
                    if(end_of_extension == std::string::npos) {
                        return false;
                    }

                    // If an end-of-extension character appears right after the end of the extension string we're done
                    if(end_of_extension == search_str_end) {
                        return true;
                    }

                    // Otherwise search again
                    start_pos = end_of_extension;
                }

                return false;
            }

        public:
            static inline bool hasExtension(const std::string& isa_string, const char extension) {
                size_t i = 0;

                // RISC-V ISA strings have all single-character extensions at the beginning of the string.
                // These do not have to be separated by _ characters
                for(; i < isa_string.size(); ++i) {
                    // If we find the character before the first _, we're done
                    if(const auto ch = isa_string[i]; ch == extension) {
                        return true;
                    }

                    // Once we reach the first _ character, we need to use a different method to search the
                    // ISA string
                    else if(ch == '_') {
                        break;
                    }
                }

                if(i == isa_string.size()) {
                    return false;
                }

                // At this point we can treat it identically to a multi-character extension
                return findMulticharExtension_(isa_string, std::string(1, extension), i);
            }

            static inline bool hasExtension(const std::string& isa_string, const std::string& extension) {
                // Use the single-character method if this is a single-character extension
                if(extension.size() == 1) {
                    return hasExtension(isa_string, extension.front());
                }

                return findMulticharExtension_(isa_string, extension);
            }
    };
}

#endif
