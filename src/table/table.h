#ifndef NERU_TABLE_H
#define NERU_TABLE_H

#include <map>
#include <queue>
#include "field/fields.h"
#include "file/files.h"
#include "page/pages.h"
#include "record/record.h"
#include "index/index.h"
#include "condition/conditions.h"

namespace Neru {

    class Table {
    public:
        // constructors
        Table() = delete;
        Table(std::string name, Header head, bool create = false, std::string tag = "table");
        explicit Table(const uint8_t *src);

        ~Table();

        // io
        uint8_t *store(uint8_t *dst) const;
        const uint8_t *load(const uint8_t *src);

        // info
        Header type() const;
        size_t size() const;
        std::string info() const;

        // apis
        Entry insert(std::shared_ptr<Record> record);
        Entry update(Entry entry, std::shared_ptr<Record> record);
        bool remove(Entry entry);
        std::shared_ptr<Record> get(Entry entry);

        std::vector<std::shared_ptr<Record>> select() const;

        // index-related apis
        bool has_index(size_t index_idx);
        bool build_index(size_t index_idx);
        bool delete_index(size_t index_idx);
        bool key_has(size_t index_idx, std::shared_ptr<Field> key);
        Entry key_at(size_t index_idx, std::shared_ptr<Field> key);
        std::vector<Entry> key_range(size_t index_idx, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper);
        Entry insert_with_index(size_t index_idx, std::shared_ptr<Record> record);
        Entry update_with_index(size_t index_idx, std::shared_ptr<Field> key, std::shared_ptr<Record> record);
        bool remove_with_index(size_t index_idx, std::shared_ptr<Field> key);

    private:
        std::string _name, _tag;
        Header _head;
        std::unique_ptr<MetadataFile> _metadata;
        std::map<size_t, std::unique_ptr<IndexFile>> _index;
        std::unique_ptr<DataFile> _data;

        friend class Join;
    };

    class Join{
    public:
        Join() = delete;
        Join(std::vector<std::shared_ptr<Table>> table, std::vector<JoinCondition> condition, std::vector<std::pair<std::shared_ptr<Table>, size_t>> output);
        std::shared_ptr<Table> SortMergeJoin();
        std::shared_ptr<Table> HashJoin();

    private:
        Header headerJoin(const Header &h1, const Header &h2);
        std::shared_ptr<Record> recordJoin(std::shared_ptr<Record> record1, std::shared_ptr<Record> record2);
        std::vector<JoinCondition> getConditions(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2);
        void initMapping();
        void updateMapping(std::shared_ptr<Table> t1, std::shared_ptr<Table> t2, std::shared_ptr<Table> newt);

        std::shared_ptr<Table> sort(std::shared_ptr<Table> table, size_t idx);
        std::shared_ptr<Table> selectOutput(std::shared_ptr<Table> table);
        std::shared_ptr<Table> cartesian(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2);
        std::shared_ptr<Table> SortMergeJoin2(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2);
        std::shared_ptr<Table> HashJoin2(std::shared_ptr<Table> table1, std::shared_ptr<Table> table2);

        std::vector<std::shared_ptr<Table>> table;
        std::vector<JoinCondition> condition;
        std::vector<std::pair<std::shared_ptr<Table>, size_t>> output;
        std::map<std::pair<std::shared_ptr<Table>, size_t>, std::pair<std::shared_ptr<Table>, size_t>> mapping;
    };

}// namespace Neru

#endif//NERU_TABLE_H
