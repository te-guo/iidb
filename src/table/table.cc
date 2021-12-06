#include <memory>
#include "table.h"

namespace Neru {

    // constructors
    Table::Table(std::string name, Header head, bool create) : _name(name), _head(head),
    _data(name + ".data", head, create),
    _metadata(name + ".metadata", head, create) {
        uint8_t tmp[PAGE_SIZE];
        if(create)
            return;
        std::shared_ptr<Page> page = _metadata.get(0);
        page->read(tmp, head.size(), 0);
        for(int i = 0; i < head.size(); i++)
            if(tmp[i])
                _index.insert(std::make_pair(i, IndexFile(name + ".index." + std::to_string(i), head[i])));
    }

    bool Table::build_index(size_t idx){
        if(_metadata.set(idx)){
            auto index = _index.insert(std::make_pair(idx, IndexFile(_name + ".index." + std::to_string(idx), _head[idx], true))).first->second;
            std::vector<std::pair<std::shared_ptr<Record>, Entry>> records = _data.select_with_entry();
            for(auto& record: records)
                index.insert((*record.first)[idx], record.second);
            return true;
        }
        else
            return false;
    }

    // info
    Header Table::type() const { return _head; }
    size_t Table::size() const { return _data.length(); }
    std::string Table::info() const { return "Table:\n\t" + _name + "\n\t" + utils::info(_head); }

    // apis
    Entry Table::insert(std::shared_ptr<Record> record) {
        Entry entry = _data.insert(record);
        for(auto& idx: _index)
            idx.second.insert((*record)[idx.first], entry);
        return entry;
    }
    Entry Table::update(Entry entry, std::shared_ptr<Record> record) {
        std::queue<size_t> modified;
        for(auto& idx: _index)
            if((*_data.get(entry.page())->get(entry.slot()))[idx.first] != (*record)[idx.first]){
                idx.second.erase((*_data.get(entry.page())->get(entry.slot()))[idx.first]);
                modified.push(idx.first);
            }
        entry = _data.update(entry, record);
        for(auto& idx: _index)
            if(!modified.empty() && modified.front() == idx.first){
                modified.pop();
                idx.second.insert((*record)[idx.first], entry);
            }
        return entry;
    }
    bool Table::remove(Entry entry) {
        auto record = _data.get(entry.page())->get(entry.slot());
        if(_data.remove(entry)){
            for(auto& idx: _index)
                idx.second.erase((*record)[idx.first]);
            return true;
        }
        else
            return false;
    }
    std::vector<std::shared_ptr<Record>> Table::select() const { return _data.select(); }

}// namespace Neru
