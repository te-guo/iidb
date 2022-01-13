#include "metadata_file.h"

namespace Neru {

    // constructors
    MetadataFile::MetadataFile(std::string name, Header head, bool create) : File(name + ".metadata", create) {
        _size = head.size();
        if (create){
            allocate();
            return;
        }
    }
    // info
    FileType MetadataFile::type() { return FileType::METADATA; }
    std::string MetadataFile::info() const {
        std::string _info = std::to_string(this->size()) + "\tMetadataFile: " + _name;
        for (size_t i = 0; i < _pages.size(); ++i)
            if (_flag.test(i))
                _info += ' ' + std::to_string(i);
        return _info;
    }
    // tools
    bool MetadataFile::test(size_t idx){
        if(idx >= _size)
            throw std::runtime_error("Build_index: Out of range!");
        uint8_t tmp;
        std::shared_ptr<Page> page = this->get(0);
        page->read(&tmp, 1, idx);
        return tmp;
    }
    bool MetadataFile::set(size_t idx){
        if(idx >= _size)
            throw std::runtime_error("Build_index: Out of range!");
        uint8_t tmp;
        std::shared_ptr<Page> page = this->get(0);
        page->read(&tmp, 1, idx);
        if(tmp)
            return false;
        tmp = 1;
        page->write(&tmp, 1, idx);
        return true;
    }
    bool MetadataFile::reset(size_t idx){
        if(idx >= _size)
            throw std::runtime_error("Build_index: Out of range!");
        uint8_t tmp;
        std::shared_ptr<Page> page = this->get(0);
        page->read(&tmp, 1, idx);
        if(!tmp)
            return false;
        tmp = 0;
        page->write(&tmp, 1, idx);
        return true;
    }

}// namespace Neru
