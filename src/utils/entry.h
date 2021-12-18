#ifndef NERU_ENTRY_H
#define NERU_ENTRY_H

#include <iostream>

namespace Neru {

    class Entry {
    public:
        // constructors
        Entry() = default;
        Entry(size_t page, size_t slot);

        // info
        std::string info() const;
        size_t page() const;
        size_t slot() const;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const Entry &entry);
        Entry &operator++();
        Entry &operator--();
        Entry &operator+=(size_t offset);
        Entry &operator-=(size_t offset);
        bool operator==(const Entry &entry) const;
        bool operator!=(const Entry &entry) const;
        bool operator<(const Entry &entry) const;

        // tools
        bool last() const;

        // static
        static void set(size_t capacity);

    private:
        size_t _page{};
        size_t _slot{};

        // static
        static size_t _capacity;
    };

}// namespace Neru

#endif//NERU_ENTRY_H
