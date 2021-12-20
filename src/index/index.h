#ifndef NERU_INDEX_H
#define NERU_INDEX_H

#include <vector>

#include "field/fields.h"
#include "utils/utils.h"

namespace Neru {

    // TODO: On disk!
    class Index {
    public:
        // constructors
        Index() = default;

        // apis
        bool has(std::shared_ptr<Field> key) const;
        Entry at(std::shared_ptr<Field> key) const;
        std::vector<Entry> range(std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const;
        bool insert(std::shared_ptr<Field> key, Entry value);
        bool erase(std::shared_ptr<Field> key);

    private:
        std::vector<std::shared_ptr<Field>> _keys;
        std::vector<Entry> _values;
    };

}// namespace Neru

#endif//NERU_INDEX_H
