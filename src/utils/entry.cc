#include "entry.h"

namespace Neru {

    // constructors
    Entry::Entry(size_t page, size_t slot) : _page(page), _slot(slot) {
        if (_slot >= _capacity)
            throw std::runtime_error("Entry: Slot exceeds!");
    }

    // info
    std::string Entry::info() const { return "PageID: " + std::to_string(_page) + ", SlotID: " + std::to_string(_slot); }
    size_t Entry::page() const { return _page; }
    size_t Entry::slot() const { return _slot; }

    // operators
    std::ostream &operator<<(std::ostream &os, const Entry &entry) { return os << entry.info(); }
    Entry &Entry::operator++() {
        return *this += 1;
    }
    Entry &Entry::operator--() {
        return *this -= 1;
    }
    Entry &Entry::operator+=(size_t offset) {
        _slot += offset;
        _page += _slot / _capacity;
        _slot %= _capacity;
        return *this;
    }
    Entry &Entry::operator-=(size_t offset) {
        if (_page * _capacity + _slot < offset)
            throw std::runtime_error("Entry: Less than zero!");
        _slot -= offset;
        _page += _slot / _capacity;
        _slot %= _capacity;
        _slot = (_slot + _capacity) % _capacity;
        return *this;
    }
    bool Entry::operator==(const Entry &entry) const { return _page == entry._page && _slot == entry._slot; }
    bool Entry::operator!=(const Entry &entry) const { return !(*this == entry); }
    bool Entry::operator<(const Entry &entry) const { return _page < entry._page || (_page == entry._page && _slot < entry._slot); }

    // tools
    bool Entry::last() const { return _slot + 1 == _capacity; }

    // static
    void Entry::set(size_t capacity) { _capacity = capacity; }

}// namespace Neru
