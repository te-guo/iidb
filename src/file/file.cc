#include "file.h"

namespace Neru {

    // constructors
    File::File(std::string name, bool create) : _name(name) {
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
            throw std::runtime_error("File: Empty load!");
        for (size_t i = 0; i < size; ++i) {
            auto page = std::make_shared<Page>();
            if (_flag.test(i))
                data.read(page->raw(), PAGE_SIZE);
            else
                data.seekg(PAGE_SIZE, std::ios::cur);
            _pages.push_back(page);
        }
        data.close();
        if (!flag.good() || !data.good())
            throw std::runtime_error("File: Fail to load!");
    }
    File::~File() { this->save(); }

    // info
    FileType File::type() { return FileType::ANY; }
    size_t File::size() const { return _pages.size(); }
    std::string File::info() const {
        std::string _info = std::to_string(this->size()) + "\tFile: " + _name;
        for (size_t i = 0; i < _pages.size(); ++i)
            if (_flag.test(i))
                _info += ' ' + std::to_string(i);
        return _info;
    }
    std::string File::get_name() const{
        return _name;
    }

    // operators
    std::ostream &operator<<(std::ostream &os, const File &file) { return os << file.info(); }
    std::shared_ptr<Page> &File::operator[](size_t idx) {
        if (idx >= _pages.size())
            throw std::runtime_error("File: Out of range!");
        return _pages[idx];
    }

    // tools
    bool File::init(std::filesystem::path path) {
        auto status = std::filesystem::status(path);
        if (!std::filesystem::exists(status)) {
            std::filesystem::create_directory(path);
            return true;
        }
        if (!std::filesystem::is_directory(status))
            return false;
        return true;
    }
    bool File::save() const {
        if (_pages.empty())
            throw std::runtime_error("File: Empty save!");
        std::ofstream flag(_path + ".flag", std::ios::binary);
        std::ofstream data(_path + ".data", std::ios::binary);
        if (!flag || !data)
            return false;
        size_t size = _pages.size();
        flag.write((const char *) &size, sizeof(size_t));
        flag.write((const char *) &_flag, sizeof(_flag));
        flag.close();
        for (size_t i = 0; i < _pages.size(); ++i)
            if (_flag.test(i)) {
                data.seekp(PAGE_SIZE * i);
                data.write(_pages[i]->raw(), PAGE_SIZE);
            }
        data.close();
        return flag.good() && data.good();
    }
    std::shared_ptr<Page> File::allocate() {
        if (_pages.size() == BLOCK_SIZE)
            throw std::runtime_error("File: Pages full!");
        _flag.set(_pages.size());
        _pages.push_back(std::make_shared<Page>());
        return _pages.back();
    }
    File &File::push(std::shared_ptr<Page> page) {
        if (_pages.size() == BLOCK_SIZE)
            throw std::runtime_error("File: Pages full!");
        _flag.set(_pages.size());
        _pages.push_back(page);
        return *this;
    }
    File &File::pop() {
        if (_pages.empty())
            throw std::runtime_error("File: Pages empty!");
        _pages.pop_back();
        _flag.reset(_pages.size());
        return *this;
    }
    std::shared_ptr<Page> File::get(size_t idx) const {
        if (idx >= _pages.size())
            throw std::runtime_error("File: Out of range!");
        return _pages[idx];
    }

    void remove_file(std::string name){
        std::string path = std::filesystem::path(FILE_PREFIX) / name;
        std::filesystem::remove(path + ".flag");
        std::filesystem::remove(path + ".data");
    }

}// namespace Neru
