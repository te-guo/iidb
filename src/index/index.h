#ifndef NERU_INDEX_H
#define NERU_INDEX_H

#include <string>
#include <vector>

#include "field/fields.h"
#include "file/files.h"
#include "utils/utils.h"

namespace Neru {

    // TODO: On disk!
    class IndexData{
    public:
        IndexData(std::string name_, bool create_);
        std::string name;
        bool create;
        std::unique_ptr<IndexFile> file;
    };
    class Index {
    public:
        // constructors
        Index();
        Index(std::string name, bool create = false);

        // apis
        bool has(std::shared_ptr<Field> key) const;
        Entry at(std::shared_ptr<Field> key) const;
        std::vector<Entry> range(std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const;
        bool insert(std::shared_ptr<Field> key, Entry value);
        bool erase(std::shared_ptr<Field> key);

    private:
        std::unique_ptr<IndexData> _index;
        void initialize(FieldType type) const ;
    };

}// namespace Neru

#endif//NERU_INDEX_H
