#include <memory>
#include <unordered_map>
#include "table.h"

namespace Neru {

    // constructors
    Table::Table(std::string name, Header head, bool create, std::string tag) : _name(name), _head(head), _tag(tag),
    _data(std::make_unique<DataFile>(tag + "." + name, head, create)),
    _metadata(std::make_unique<MetadataFile>(tag + "." + name, head, create)) {
        if(create)
            return;
        for(int i = 0; i < head.size(); i++)
            if(has_index(i))
                _index.insert(std::make_pair(i, std::make_unique<IndexFile>(tag + "." + name + "." + std::to_string(i), head[i])));
    }
    
    Table::~Table(){
        if(this->_tag != "table"){
            std::string filename;
            for(int i = 0; i < _head.size(); i++)
                if(has_index(i))
                    delete_index(i);
            filename = _data->get_name();
            _data.release();
            remove_file(filename);
            filename = _metadata->get_name();
            _metadata.release();
            remove_file(filename);
        }
    }

    // info
    Header Table::type() const { return _head; }
    size_t Table::size() const { return _data->length(); }
    std::string Table::info() const { return "Table:\n\t" + _name + "\n\t" + utils::info(_head); }

    // apis
    Entry Table::insert(std::shared_ptr<Record> record) {
        Entry::set(DATA_CAPACITY / utils::size(_head));
        Entry entry = _data->insert(record);
        for(auto& idx: _index)
            idx.second->insert((*record)[idx.first], entry);
        return entry;
    }
    Entry Table::update(Entry entry, std::shared_ptr<Record> record) {
        Entry::set(DATA_CAPACITY / utils::size(_head));
        std::queue<size_t> modified;
        for(auto& idx: _index)
            if(*(*_data->get(entry.page())->get(entry.slot()))[idx.first] != *(*record)[idx.first]){
                idx.second->erase((*_data->get(entry.page())->get(entry.slot()))[idx.first]);
                modified.push(idx.first);
            }
        entry = _data->update(entry, record);
        for(auto& idx: _index)
            if(!modified.empty() && modified.front() == idx.first){
                modified.pop();
                idx.second->insert((*record)[idx.first], entry);
            }
        return entry;
    }
    bool Table::remove(Entry entry) {
        Entry::set(DATA_CAPACITY / utils::size(_head));
        auto record = _data->get(entry.page())->get(entry.slot());
        if(_data->remove(entry)){
            for(auto& idx: _index)
                idx.second->erase((*record)[idx.first]);
            return true;
        }
        else
            return false;
    }
    std::shared_ptr<Record> Table::get(Entry entry){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        return _data->get(entry);
    }

    std::vector<std::shared_ptr<Record>> Table::select() const {
        Entry::set(DATA_CAPACITY / utils::size(_head));
        return _data->select();
    }

    // index-related apis
    bool Table::has_index(size_t index_idx){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        return _metadata->test(index_idx);
    }
    bool Table::build_index(size_t index_idx){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(_metadata->set(index_idx)){
            auto& index = _index.insert(std::make_pair(index_idx, std::make_unique<IndexFile>(_tag + "." + _name + "." + std::to_string(index_idx), _head[index_idx], true))).first->second;
            std::vector<std::pair<std::shared_ptr<Record>, Entry>> records;
            _data->select_with_entry(records);
            for(auto& record: records)
                index->insert((*record.first)[index_idx], record.second);
            return true;
        }
        else
            return false;
    }
    bool Table::delete_index(size_t index_idx){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(_metadata->reset(index_idx)){
            auto filename = _index[index_idx]->get_name();
            _index.erase(index_idx);
            remove_file(filename);
            return true;
        }
        else
            return false;
    }
    bool Table::key_has(size_t index_idx, std::shared_ptr<Field> key){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return _index[index_idx]->has(key);
    }
    Entry Table::key_at(size_t index_idx, std::shared_ptr<Field> key){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return _index[index_idx]->at(key);
    }
    std::vector<Entry> Table::key_range(size_t index_idx, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return _index[index_idx]->range(lower, upper);
    }
    Entry Table::insert_with_index(size_t index_idx, std::shared_ptr<Record> record){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return insert(record);
    }
    Entry Table::update_with_index(size_t index_idx, std::shared_ptr<Field> key, std::shared_ptr<Record> record){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return update(_index[index_idx]->at(key), record);
    }
    bool Table::remove_with_index(size_t index_idx, std::shared_ptr<Field> key){
        Entry::set(DATA_CAPACITY / utils::size(_head));
        if(!_index.count(index_idx))
            throw std::runtime_error("Table: Expected index does not exist");
        return remove(_index[index_idx]->at(key));
    }

    // Join
    size_t jointmp_counter = 0;

    Join::Join(std::vector<std::shared_ptr<Table>> table, std::vector<JoinCondition> condition, std::vector<std::pair<std::shared_ptr<Table>, size_t>> output){
        this->table = table;
        this->condition = condition;
        this->output = output;
    }

    Header Join::headerJoin(const Header &h1, const Header &h2){
        Header result;
        for(auto type: h1)
            result.push_back(type);
        for(auto type: h2)
            result.push_back(type);
        return result;
    }
    std::shared_ptr<Record> Join::recordJoin(std::shared_ptr<Record> record1, std::shared_ptr<Record> record2){
        std::shared_ptr<Record> record = std::make_shared<Record>();
        for(int i = 0; i < record1->length(); i++)
            record->push((*record1)[i]);
        for(int i = 0; i < record2->length(); i++)
            record->push((*record2)[i]);
        return record;
    }

    std::vector<JoinCondition> Join::getConditions(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2){
        std::vector<JoinCondition> conditions;
        for(auto &c: condition){
            auto column1 = mapping[std::make_pair(c._lhst, c._lhs)], column2 = mapping[std::make_pair(c._rhst, c._rhs)];
            if(column1.first == table1 && column2.first == table2)
                conditions.push_back(JoinCondition(column1.first, column1.second, column2.first, column2.second));
            else if(column1.first == table2 && column2.first == table1)
                conditions.push_back(JoinCondition(column2.first, column2.second, column1.first, column1.second));
        }
        return conditions;
    }

    void Join::initMapping(){
        mapping.clear();
        for(auto &t: table)
            for(size_t i = 0; i < t->_head.size(); i++)
                mapping[std::make_pair(t, i)] = std::make_pair(t, i);
    }
    void Join::updateMapping(std::shared_ptr<Table> t1, std::shared_ptr<Table> t2, std::shared_ptr<Table> newt){
        for(auto &mp: mapping)
            if(mp.second.first == t1)
                mp.second = std::make_pair(newt, mp.second.second);
            else if(mp.second.first == t2)
                mp.second = std::make_pair(newt, mp.second.second + t1->_head.size());
    }

    std::shared_ptr<Table> Join::cartesian(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2){
        auto result = std::make_shared<Table>(std::to_string(jointmp_counter++), headerJoin(table1->_head, table2->_head), true, "jointmp");
        auto data1 = table1->select(), data2 = table2->select();
        for(auto &rec1: data1)
            for(auto &rec2: data2)
                result->insert(recordJoin(rec1, rec2));
        return result;
    }

    std::shared_ptr<Table> Join::sort(std::shared_ptr<Table> table, size_t idx){
        auto result = std::make_shared<Table>(std::to_string(jointmp_counter++), table->_head, true, "jointmp");
        auto data = table->select();
        std::sort(data.begin(), data.end(), [idx](std::shared_ptr<Record> a, std::shared_ptr<Record> b){return *(*a)[idx] < *(*b)[idx];});
        for(auto &rec: data)
            result->insert(rec);
        return result;
    }

    std::shared_ptr<Table> Join::selectOutput(std::shared_ptr<Table> table){
        Header head;
        for(int i = 0; i < output.size(); i++)
            head.push_back(mapping[output[i]].first->_head[mapping[output[i]].second]);
        auto result = std::make_shared<Table>(std::to_string(jointmp_counter++), head, true, "jointmp");
        auto data = table->select();
        for(auto &rec: data){
            auto r = std::make_shared<Record>();
            for(int i = 0; i < output.size(); i++)
                r->push((*rec)[mapping[output[i]].second]);
            result->insert(r);
        }
        return result;
    }

    std::shared_ptr<Table> Join::SortMergeJoin2(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2){
        std::vector<JoinCondition> conditions = getConditions(table1, table2);
        if(conditions.empty())
            return cartesian(table1, table2);
        auto result = std::make_shared<Table>(std::to_string(jointmp_counter++), headerJoin(table1->_head, table2->_head), true, "jointmp");
        auto &cond = conditions.front();
        auto sortedtable1 = sort(table1, cond._lhs), sortedtable2 = sort(table2, cond._rhs);
        auto data1 = sortedtable1->select(), data2 = sortedtable2->select();
        auto p1 = data1.begin(), p2 = data2.begin();
        while(p1 < data1.end() && p2 < data2.end())
            if(cond.match(*p1, *p2)){
                auto q1 = p1 + 1, q2 = p2 + 1;
                while(q1 < data1.end() && cond.match(*q1, *p2))q1++;
                while(q2 < data2.end() && cond.match(*p1, *q2))q2++;
                for(auto r1 = p1; r1 < q1; r1++)
                    for(auto r2 = p2; r2 < q2; r2++){
                        bool ok = true;
                        for(int i = 1; i < conditions.size(); i++)
                            if(!conditions[i].match(*r1, *r2)){
                                ok = false;
                                break;
                            }
                        if(ok)
                            result->insert(recordJoin(*r1, *r2));
                    }
                p1 = q1;
                p2 = q2;
            }
            else
                if(*(**p1)[cond._lhs] < *(**p2)[cond._rhs])
                    p1++;
                else
                    p2++;
        return result;
    }

    class func_hash{
        public: size_t operator()(std::shared_ptr<Field> a) const {return a->hash();}
    };
    class func_equal{
        public: bool operator()(std::shared_ptr<Field> a, std::shared_ptr<Field> b) const {return *a == *b;}
    };
    std::shared_ptr<Table> Join::HashJoin2(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2){
        std::vector<JoinCondition> conditions = getConditions(table1, table2);
        if(conditions.empty())
            return cartesian(table1, table2);
        auto result = std::make_shared<Table>(std::to_string(jointmp_counter++), headerJoin(table1->_head, table2->_head), true, "jointmp");
        auto &cond = conditions.front();
        auto data1 = table1->select(), data2 = table2->select();
        std::unordered_map<std::shared_ptr<Field>, std::vector<std::shared_ptr<Record>>, func_hash, func_equal> hashTable;
        if(table1->size() <= table2->size()){
            for(int i = 0; i < data1.size(); i++)
                hashTable[(*data1[i])[cond._lhs]].push_back(data1[i]);
            for(int i = 0; i < data2.size(); i++)
                for(auto &record1: hashTable[(*data2[i])[cond._rhs]]){
                    bool ok = true;
                    for(int j = 1; j < conditions.size(); j++)
                        if(!conditions[j].match(record1, data2[i])){
                            ok = false;
                            break;
                        }
                    if(ok)
                        result->insert(recordJoin(record1, data2[i]));
                }
        }
        else{
            for(int i = 0; i < data2.size(); i++)
                hashTable[(*data2[i])[cond._rhs]].push_back(data2[i]);
            for(int i = 0; i < data1.size(); i++)
                for(auto &record2: hashTable[(*data1[i])[cond._lhs]]){
                    bool ok = true;
                    for(int j = 1; j < conditions.size(); j++)
                        if(!conditions[j].match(data1[i], record2)){
                            ok = false;
                            break;
                        }
                    if(ok)
                        result->insert(recordJoin(data1[i], record2));
                }
        }
        return result;
    }

    std::shared_ptr<Table> Join::SortMergeJoin(){
        initMapping();
        auto tables = table;
        while(tables.size() > 1){
            auto table1 = tables[tables.size() - 2], table2 = tables[tables.size() - 1];
            auto res = SortMergeJoin2(table1, table2);
            updateMapping(table1, table2, res);
            tables.pop_back();
            tables.pop_back();
            tables.push_back(res);
        }
        return selectOutput(tables.front());
    }
    std::shared_ptr<Table> Join::HashJoin(){
        initMapping();
        auto tables = table;
        while(tables.size() > 1){
            auto table1 = tables[tables.size() - 2], table2 = tables[tables.size() - 1];
            auto res = HashJoin2(table1, table2);
            updateMapping(table1, table2, res);
            tables.pop_back();
            tables.pop_back();
            tables.push_back(res);
        }
        return selectOutput(tables.front());
    }

}// namespace Neru
