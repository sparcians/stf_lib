#ifndef __STF_READER_WRITER_BASE_HPP__
#define __STF_READER_WRITER_BASE_HPP__

#include <cstdio>
#include <sstream>
#include <string_view>

#include "stf_enums.hpp"

namespace stf {
    /**
     * \class STFReaderWriterBase
     *
     * Common base class for STFReader and STFWriter.
     */
    class STFReaderWriterBase {
        private:
            STF_FILE_TYPE file_type_; /**< File type */

        protected:
            /**
             * Checks whether the file extension matches the given extension
             * \param filename filename to check
             * \param ext extension to check for
             */
            static constexpr bool check_extension_(const std::string_view filename, const std::string_view ext) {
                return filename.rfind(ext) != std::string_view::npos;
            }

            STFReaderWriterBase() = default;
            virtual ~STFReaderWriterBase() = default;

            /**
             * Gets and remembers the file type based on the file extension
             * \param filename Filename to get the file type for
             */
            STF_FILE_TYPE getFileType_(const std::string_view filename) {
                file_type_ = guessFileType(filename);
                return file_type_;
            }

        public:
            STFReaderWriterBase(const STFReaderWriterBase&) = delete;
            STFReaderWriterBase& operator=(const STFReaderWriterBase&) = delete;

            /**
             * Move constructor
             */
            STFReaderWriterBase(STFReaderWriterBase&&) = default;

            /**
             * Move assignment operator
             */
            STFReaderWriterBase& operator=(STFReaderWriterBase&&) = default;

            /**
             * Gets the file type
             */
            STF_FILE_TYPE getFileType() const {
                return file_type_;
            }

            /**
             * Guesses the file type based on the file extension
             * \param filename Filename to get file type for
             */
            static constexpr STF_FILE_TYPE guessFileType(std::string_view filename) {
                constexpr std::string_view gz_ext = ".stf.gz";
                constexpr std::string_view xz_ext = ".stf.xz";
                constexpr std::string_view sh_ext = ".sh";
                constexpr std::string_view zstf_ext = ".zstf";
                constexpr std::string_view stf_ext = ".stf";

                if(check_extension_(filename, zstf_ext)) {
                    return STF_FILE_TYPE::ZSTF;
                }
                if (filename == "-") {
                    return STF_FILE_TYPE::STDIO;
                }
                if (check_extension_(filename, gz_ext)) {
                    return STF_FILE_TYPE::STF_GZ;
                }
                if (check_extension_(filename, xz_ext)) {
                    return STF_FILE_TYPE::STF_XZ;
                }
                if (check_extension_(filename, sh_ext)) {
                    return STF_FILE_TYPE::STF_SH;
                }
                if (check_extension_(filename, stf_ext)) {
                    return STF_FILE_TYPE::STF;
                }

                return STF_FILE_TYPE::UNKNOWN;
            }

            /**
             * Returns whether the file is compressed
             */
            static constexpr bool isCompressedFile(STF_FILE_TYPE filetype) {
                switch(filetype) {
                    case STF_FILE_TYPE::ZSTF:
                    case STF_FILE_TYPE::STF_GZ:
                    case STF_FILE_TYPE::STF_XZ:
                        return true;
                    case STF_FILE_TYPE::STDIO:
                    case STF_FILE_TYPE::STF_SH:
                    case STF_FILE_TYPE::STF:
                    case STF_FILE_TYPE::UNKNOWN:
                        return false;
                };

                return false;
            }

            /**
             * Returns whether the file is compressed
             */
            static inline bool isCompressedFile(std::string_view filename) {
                return isCompressedFile(guessFileType(filename));
            }
    };

} // end namespace stf

#endif
