#include "index.h"

namespace Neru {

    IndexData::IndexData(std::string name_, bool create_): name(name_), create(create_) {}
    void IndexData::initialize(FieldType type){
        file = std::make_unique<IndexFile>(name, type, create);
    }

    int indextmp_counter = 0;

    // constructors
    Index::Index(): _index(std::make_unique<IndexData>("indextmp." + std::to_string(indextmp_counter++), true)) {}
    Index::Index(std::string name, bool create): _index(std::make_unique<IndexData>("index." + name, create)) {}

    Index::~Index(){
        if(_index->name.substr(0, _index->name.find('.')) == "indextmp"){
            auto filename = _index->file->get_name();
            _index.release();
            remove_file(filename);
        }
    }

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
