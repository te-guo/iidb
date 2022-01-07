#ifndef NERU_TABLE_H
#define NERU_TABLE_H

#include <map>
#include <queue>
#include "field/fields.h"
#include "file/files.h"
#include "page/pages.h"
#include "record/record.h"
#include "index/index.h"

namespace Neru {

    class Table {
    public:
        // constructors
        Table() = delete;
        Table(std::string name, Header head, bool create = false);
        explicit Table(const uint8_t *src);

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
        std::string _name;
        Header _head;
        MetadataFile _metadata;
        std::map<size_t, std::shared_ptr<IndexFile>> _index;
        DataFile _data;
    };

}// namespace Neru

#endif//NERU_TABLE_H
