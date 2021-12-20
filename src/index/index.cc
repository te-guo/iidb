#include "index.h"

namespace Neru {

    // apis
    bool Index::has(std::shared_ptr<Field> key) const {
        for (auto _key : _keys)
            if (*_key == *key)
                return true;
        return false;
    }
    Entry Index::at(std::shared_ptr<Field> key) const {
        for (size_t i = 0; i < _keys.size(); ++i)
            if (*_keys[i] == *key)
                return _values[i];
        throw std::runtime_error("Index: Key not found!");
    }
    std::vector<Entry> Index::range(std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const {
        std::vector<Entry> values;
        for (size_t i = 0; i < _keys.size(); ++i)
            if (*_keys[i] >= *lower && *_keys[i] < *upper)
                values.push_back(_values[i]);
        return values;
    }
    bool Index::insert(std::shared_ptr<Field> key, Entry value) {
        for (size_t i = 0; i < _keys.size(); ++i)
            if (*_keys[i] == *key) {
                _values[i] = value;
                return true;
            }
        // may encounter bug (shallow copy)
        _keys.push_back(key);
        _values.push_back(value);
        return true;
    }
    bool Index::erase(std::shared_ptr<Field> key) {
        for (size_t i = 0; i < _keys.size(); ++i)
            if (*_keys[i] == *key) {
                _keys.erase(_keys.begin() + i);
                _values.erase(_values.begin() + i);
                return true;
            }
        return false;
    }


}// namespace Neru
