#include <memory>
#include "table.h"

namespace Neru {

    // constructors
    Table::Table(std::string name, Header head, bool create) : _name(name), _head(head),
    _data(name + ".data", head, create),
    _metadata(name + ".metadata", head, create) {
        if(create)
            return;
        for(int i = 0; i < head.size(); i++)
            if(has_index(i))
                _index.insert(std::make_pair(i, std::make_shared<IndexFile>(name + ".index." + std::to_string(i), head[i])));
    }

    // info
    Header Table::type() const { return _head; }
    size_t Table::size() const { return _data.length(); }
    std::string Table::info() const { return "Table:\n\t" + _name + "\n\t" + utils::info(_head); }

    // apis
    Entry Table::insert(std::shared_ptr<Record> record) {
        Entry entry = _data.insert(record);
        for(auto& idx: _index)
            idx.second->insert((*record)[idx.first], entry);
        return entry;
    }
    Entry Table::update(Entry entry, std::shared_ptr<Record> record) {
        std::queue<size_t> modified;
        for(auto& idx: _index)
            if(*(*_data.get(entry.page())->get(entry.slot()))[idx.first] != *(*record)[idx.first]){
                idx.second->erase((*_data.get(entry.page())->get(entry.slot()))[idx.first]);
                modified.push(idx.first);
            }
        entry = _data.update(entry, record);
        for(auto& idx: _index)
            if(!modified.empty() && modified.front() == idx.first){
                modified.pop();
                idx.second->insert((*record)[idx.first], entry);
            }
        return entry;
    }
    bool Table::remove(Entry entry) {
        auto record = _data.get(entry.page())->get(entry.slot());
        if(_data.remove(entry)){
            for(auto& idx: _index)
                idx.second->erase((*record)[idx.first]);
            return true;
        }
        else
            return false;
    }
    std::shared_ptr<Record> Table::get(Entry entry)  { return _data.get(entry); }

    std::vector<std::shared_ptr<Record>> Table::select() const { return _data.select(); }

    // index-related apis
    bool Table::has_index(size_t index_idx){
        return _metadata.test(index_idx);
    }
    bool Table::build_index(size_t index_idx){
        if(_metadata.set(index_idx)){
            auto& index = _index.insert(std::make_pair(index_idx, std::make_shared<IndexFile>(_name + ".index." + std::to_string(index_idx), _head[index_idx], true))).first->second;
            std::vector<std::pair<std::shared_ptr<Record>, Entry>> records;
            _data.select_with_entry(records);
            for(auto& record: records)
                index->insert((*record.first)[index_idx], record.second);
            return true;
        }
        else
            return false;
    }
    bool Table::delete_index(size_t index_idx){
        if(_metadata.reset(index_idx)){
            _index.erase(index_idx);
            return true;
        }
        else
            return false;
    }
    bool Table::key_has(size_t index_idx, std::shared_ptr<Field> key){
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return _index[index_idx]->has(key);
    }
    Entry Table::key_at(size_t index_idx, std::shared_ptr<Field> key){
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return _index[index_idx]->at(key);
    }
    std::vector<Entry> Table::key_range(size_t index_idx, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper){
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return _index[index_idx]->range(lower, upper);
    }
    Entry Table::insert_with_index(size_t index_idx, std::shared_ptr<Record> record){
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return insert(record);
    }
    Entry Table::update_with_index(size_t index_idx, std::shared_ptr<Field> key, std::shared_ptr<Record> record){
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return update(_index[index_idx]->at(key), record);
    }
    bool Table::remove_with_index(size_t index_idx, std::shared_ptr<Field> key){
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return remove(_index[index_idx]->at(key));
    }

}// namespace Neru
