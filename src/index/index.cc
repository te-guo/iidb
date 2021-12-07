#include "index.h"

namespace Neru {

    IndexData::IndexData(std::string name_, bool create_): name(name_), create(create_) {}
    void IndexData::initialize(FieldType type){
        file = std::make_unique<IndexFile>(name, type, create);
    }

    // constructors
    Index::Index(): _index(std::make_unique<IndexData>(std::string(".trash"), true)) {}
    Index::Index(std::string name, bool create): _index(std::make_unique<IndexData>(name, create)) {}

    // apis
    bool Index::has(std::shared_ptr<Field> key) const {
        if(!_index->file)
            _index->initialize(key->type());
        return _index->file->has(key);
    }
    Entry Index::at(std::shared_ptr<Field> key) const {
        if(!_index->file)
            _index->initialize(key->type());
        return _index->file->at(key);
    }
    std::vector<Entry> Index::range(std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const {
        if(!_index->file)
            _index->initialize(lower->type());
        return _index->file->range(lower, upper);
    }
    bool Index::insert(std::shared_ptr<Field> key, Entry value) {
        if(!_index->file)
            _index->initialize(key->type());
        return _index->file->insert(key, value);
    }
    bool Index::erase(std::shared_ptr<Field> key) {
        if(!_index->file)
            _index->initialize(key->type());
        return _index->file->erase(key);
    }


}// namespace Neru
