#include "data_file.h"

namespace Neru {

    // constructors
    DataFile::DataFile(std::string name, Header head, bool create) : File(name, true), _head(head) {
        _path = std::filesystem::path(FILE_PREFIX) / _name;
        if (create)
            return;
        std::ifstream flag(_path + ".flag", std::ios::binary);
        std::ifstream data(_path + ".data", std::ios::binary);
        if (!flag || !data)
            return;
        size_t size;
        flag.read((char *) &size, sizeof(size_t));
        flag.read((char *) &_flag, sizeof(_flag));
        flag.close();
        if (!size)
            throw std::runtime_error("DataFile: Empty load!");
        for (size_t i = 0; i < size; ++i) {
            auto page = std::make_shared<DataPage>(head);
            if (_flag.test(i))
                data.read(page->raw(), PAGE_SIZE);
            else
                data.seekg(PAGE_SIZE, std::ios::cur);
            _pages.push_back(page);
        }
        data.close();
        if (!flag.good() || !data.good())
            throw std::runtime_error("DataFile: Fail to load!");
        size_t last_page = BLOCK_SIZE, last_slot = PAGE_SIZE;
        for (size_t i = size; i > 0; --i)
            if (_flag.test(i - 1)) {
                last_page = i - 1;
                break;
            }
        if (last_page == BLOCK_SIZE)
            throw std::runtime_error("DataFile: File empty!");
        auto page = this->get(last_page);
        for (size_t i = page->capacity(); i > 0; --i)
            if (page->has(i - 1)) {
                last_slot = i - 1;
                break;
            }
        if (last_slot == PAGE_SIZE)
            throw std::runtime_error("DataFile: Last Page empty!");
        _front = {last_page, last_slot};
        //        std::cerr << "Front: " << _front << std::endl;
        for (size_t i = 0; i <= last_page; ++i) {
            auto free = this->get(i)->free();
            for (auto slot : free) {
                if (i == last_page)
                    if (slot >= last_slot)
                        continue;
                _free.push({i, slot});
            }
        }
        //        std::cerr << "Free: " << std::endl;
        //        while (!_free.empty()) {
        //            std::cerr << _free.top() << std::endl;
        //            _free.pop();
        //        }
    }

    // info
    FileType DataFile::type() { return FileType::DATA; }
    size_t DataFile::length() const {
        size_t length = 0;
        for (size_t i = 0; i < this->size(); ++i)
            length += this->get(i)->length();
        return length;
    }
    std::string DataFile::info() const {
        std::string _info = std::to_string(this->size()) + "\tDataFile: " + _name;
        for (size_t i = 0; i < _pages.size(); ++i)
            if (_flag.test(i))
                _info += ' ' + std::to_string(i);
        return _info;
    }

    // operators
    std::ostream &operator<<(std::ostream &os, const DataFile &file) { return os << file.info(); }

    // tools
    std::shared_ptr<DataPage> DataFile::allocate() {
        if (_pages.size() == BLOCK_SIZE)
            throw std::runtime_error("DataFile: Pages full!");
        _flag.set(_pages.size());
        _pages.push_back(std::make_shared<DataPage>(_head));
        return std::static_pointer_cast<DataPage>(_pages.back());
    }
    DataFile &DataFile::push(std::shared_ptr<DataPage> page) {
        if (_pages.size() == BLOCK_SIZE)
            throw std::runtime_error("DataFile: Pages full!");
        _flag.set(_pages.size());
        _pages.push_back(page);
        return *this;
    }
    DataFile &DataFile::pop() {
        if (_pages.empty())
            throw std::runtime_error("DataFile: Pages empty!");
        _pages.pop_back();
        _flag.reset(_pages.size());
        return *this;
    }
    std::shared_ptr<DataPage> DataFile::get(size_t idx) const {
        if (idx >= _pages.size())
            throw std::runtime_error("DataFile: Out of range!");
        return std::static_pointer_cast<DataPage>(_pages[idx]);
    }

    // tools
    Entry DataFile::_next() {
        if (_pages.empty()) {
            this->allocate();
            return _front;
        }
        if (!_free.empty()) {
            Entry entry = _free.top();
            _free.pop();
            return entry;
        }
        if (_front.last())
            this->allocate();
        return ++_front;
    }

    // apis
    Entry DataFile::insert(std::shared_ptr<Record> record) {
        Entry entry = _next();
        return this->update(entry, record, false);
    }
    Entry DataFile::update(Entry entry, std::shared_ptr<Record> record, bool overwrite) {
        auto page = this->get(entry.page());
        size_t slot = entry.slot();
        if (!overwrite && page->has(slot))
            throw std::runtime_error("DataFile: Overwriting record!");
        page->set(slot, record);
        return entry;
    }
    bool DataFile::remove(Entry entry) {
        auto page = this->get(entry.page());
        size_t slot = entry.slot();
        if (!page->has(slot))
            return false;
        page->unset(slot);
        _free.push(entry);
        return true;
    }
    std::vector<std::shared_ptr<Record>> DataFile::select() const {
        std::vector<std::shared_ptr<Record>> records;
        for (size_t i = 0; i < this->size(); ++i)
            if (_flag.test(i)) {
                auto _records = this->get(i)->select();
                std::move(_records.begin(), _records.end(), std::back_inserter(records));
            }
        return records;
    }
    std::vector<std::pair<std::shared_ptr<Record>, Entry>> DataFile::select_with_entry() const {
        std::vector<std::pair<std::shared_ptr<Record>, Entry>> records;
        for (size_t i = 0; i < this->size(); ++i)
            if (_flag.test(i)) {
                auto _records = this->get(i)->select_with_slot();
                for(auto& record: _records)
                    records.push_back(std::make_pair(record.first, Entry(i, record.second)));
            }
        return records;
    }


}// namespace Neru
