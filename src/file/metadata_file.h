#ifndef NERU_METADATA_FILE_H
#define NERU_METADATA_FILE_H

#include "file.h"
#include "page/data_page.h"
#include "record/record.h"
#include "utils/utils.h"

#include <queue>

namespace Neru {

    class MetadataFile : public File {
    public:
        // constructors
        MetadataFile(std::string name, Header head, bool create = false);
        // info
        static FileType type();
        std::string info() const;
        //tools
        bool has_index(size_t idx);
        bool set(size_t idx);
    private:
        size_t _size;
    };

}// namespace Neru

#endif//NERU_METADATA_FILE_H
