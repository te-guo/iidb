#ifndef NERU_DATA_PAGE_H
#define NERU_DATA_PAGE_H

#include "page.h"
#include "record/record.h"
#include "utils/utils.h"

#include <bitset>

namespace Neru {

    constexpr size_t FLAG_SIZE = 1024;
    constexpr size_t DATA_OFFSET = FLAG_SIZE >> 3;
    constexpr size_t DATA_CAPACITY = PAGE_SIZE - DATA_OFFSET;

    class DataPage : public Page {
    public:
        // constructors
        DataPage() = delete;
        explicit DataPage(Header head);

        // info
        size_t size() const;
        size_t length() const;
        size_t capacity() const;
        std::vector<size_t> free() const;

        // tools
        DataPage &set(size_t idx, std::shared_ptr<Record> record);
        DataPage &unset(size_t idx);
        bool has(size_t idx) const;
        std::shared_ptr<Record> get(size_t idx) const;
        std::vector<std::shared_ptr<Record>> select() const;
        std::vector<std::pair<std::shared_ptr<Record>, size_t>> select_with_slot() const;

    private:
        size_t _size;
        size_t _capacity;
        std::bitset<FLAG_SIZE> *_flag{(std::bitset<FLAG_SIZE> *) _data.get()};
        Header _head;

        // operators
        uint8_t *operator[](size_t idx) const;
    };

}// namespace Neru

#endif//NERU_DATA_PAGE_H
