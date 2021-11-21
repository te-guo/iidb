#include "data_page.h"

namespace Neru {

    // constructors
    DataPage::DataPage(Header head) : _head(head) {
        _size = utils::size(head);
        _capacity = DATA_CAPACITY / _size;
    }

    // info
    size_t DataPage::size() const { return _size; }
    size_t DataPage::length() const { return _flag->count(); }
    size_t DataPage::capacity() const { return _capacity; }
    std::vector<size_t> DataPage::free() const {
        std::vector<size_t> _free;
        for (size_t i = 0; i < _capacity; ++i)
            if (_flag->test(i))
                _free.push_back(i);
        return _free;
    }

    // tools
    DataPage &DataPage::set(size_t idx, std::shared_ptr<Record> record) {
        if (_size != record->size())
            throw std::runtime_error("DataPage: Size not match!");
        record->store(_data.get() + DATA_OFFSET + idx * _size);
        _flag->set(idx);
        return *this;
    }
    DataPage &DataPage::unset(size_t idx) {
        if (idx >= _capacity)
            throw std::runtime_error("DataPage: Out of range!");
        if (!_flag->test(idx))
            std::cerr << "DataPage: Already unsetted." << std::endl;
        _flag->reset(idx);
        return *this;
    }
    bool DataPage::has(size_t idx) const { return idx < _capacity && _flag->test(idx); }
    std::shared_ptr<Record> DataPage::get(size_t idx) const {
        if (!this->has(idx))
            throw std::runtime_error("DataPage: No such slot!");
        return std::make_shared<Record>((*this)[idx], _head);
    }
    std::vector<std::shared_ptr<Record>> DataPage::select() const {
        std::vector<std::shared_ptr<Record>> records;
        for (size_t i = 0; i < _capacity; ++i)
            if (_flag->test(i))
                records.push_back(std::make_shared<Record>(_data.get() + DATA_OFFSET + i * _size, _head));
        return records;
    }

    // tools
    uint8_t *DataPage::operator[](size_t idx) const {
        if (idx >= _capacity)
            throw std::runtime_error("DataPage: Out of range!");
        return _data.get() + DATA_OFFSET + idx * _size;
    }

}// namespace Neru