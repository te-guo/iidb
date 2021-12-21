#ifndef NERU_TABLE_H
#define NERU_TABLE_H

#include "field/fields.h"
#include "file/files.h"
#include "page/pages.h"
#include "record/record.h"

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

    private:
        std::string _name;
        Header _head;
        DataFile _file;
    };

}// namespace Neru

#endif//NERU_TABLE_H
