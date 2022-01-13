#include "index_file.h"

namespace Neru {

    // constructors
    IndexFile::IndexFile(std::string name, FieldType field, bool create) : File(name + ".index", true), _field(field) {
        _path = std::filesystem::path(FILE_PREFIX) / _name;
        if (create){
            allocate();
            return;
        }
        std::ifstream flag(_path + ".flag", std::ios::binary);
        std::ifstream data(_path + ".data", std::ios::binary);
        if (!flag || !data)
            return;
        size_t size;
        flag.read((char *) &size, sizeof(size_t));
        flag.read((char *) &_flag, sizeof(_flag));
        flag.close();
        if (!size)
            throw std::runtime_error("IndexFile: Empty load!");
        for (size_t i = 0; i < 1; ++i){
            auto page = std::make_shared<Page>();
            if (_flag.test(i))
                data.read(page->raw(), PAGE_SIZE);
            else
                data.seekg(PAGE_SIZE, std::ios::cur);
            _pages.push_back(page);
        }
        for (size_t i = 1; i < size; ++i)
            if (_flag.test(i)){
                auto type = page_type(i);
                if(type == IndexPageType::LEAF){
                    auto page = std::make_shared<LeafPage>(_field, i);
                    data.read(page->raw(), PAGE_SIZE);
                    _pages.push_back(page);
                }
                else if(type == IndexPageType::INTERNAL){
                    auto page = std::make_shared<InternalPage>(_field, i);
                    data.read(page->raw(), PAGE_SIZE);
                    _pages.push_back(page);
                }
                else{
                    auto page = std::make_shared<IndexPage>(_field, i);
                    data.read(page->raw(), PAGE_SIZE);
                    _pages.push_back(page);
                    _free.push(i);
                }
            }
            else{
                auto page = std::make_shared<IndexPage>(_field, i);
                data.seekg(PAGE_SIZE, std::ios::cur);
                _pages.push_back(page);
            }
        for (size_t i = 1; i < size; ++i)
            if(page_type(i) == IndexPageType::INTERNAL){
                auto page = get_internal_page(i);
                for(size_t j = 0; j <= page->count(); ++j)
                    get_index_page(page->ptr(j))->set_parent(i);
            }
        data.close();
        if (!flag.good() || !data.good())
            throw std::runtime_error("IndexFile: Fail to load!");
    }

    // info
    FileType IndexFile::type() { return FileType::INDEX; }
    std::string IndexFile::info() const {
        std::string _info = std::to_string(this->size()) + "\tIndexFile: " + _name;
        for (size_t i = 0; i < _pages.size(); ++i)
            if (_flag.test(i))
                _info += ' ' + std::to_string(i);
        return _info;
    }
    void IndexFile::debug(){
        std::cerr<<"-----Begin: IndexFile-----"<<std::endl;
        std::cerr<<"  root: "<<root()<<std::endl;
        for(size_t i = 1; i < _pages.size(); i++)
            if(page_type(i) == IndexPageType::LEAF){
                std::cerr<<"Page "<<i<<": ";
                get_leaf_page(i)->debug();
            }
            else if(page_type(i) == IndexPageType::INTERNAL){
                std::cerr<<"Page "<<i<<": ";
                get_internal_page(i)->debug();
            }
            else{
                std::cerr<<"Page "<<i<<": FREE"<<std::endl;
            }
        std::cerr<<"----END-----"<<std::endl;
    }

    // operators
    std::shared_ptr<Page> &IndexFile::operator[](size_t idx) {
        if (idx >= _pages.size())
            throw std::runtime_error("IndexFile: Out of range!");
        return _pages[idx];
    }

    // tools
    IndexPageType IndexFile::page_type(size_t idx) const{
        uint8_t tmp;
        _pages[0]->read(&tmp, 1, INDEX_FILE_METADATA_SIZE + idx);
        return (IndexPageType)tmp;
    }
    void IndexFile::set_page_type(size_t idx, IndexPageType type){
        uint8_t tmp = (uint8_t) type;
        _pages[0]->write(&tmp, 1, INDEX_FILE_METADATA_SIZE + idx);
    }
    size_t IndexFile::root() const{
        return _pages[0]->read_int(0);
    }
    void IndexFile::set_root(size_t idx){
        _pages[0]->write_int(idx, 0);
    }
    void IndexFile::garbage_collect(size_t idx){
        set_page_type(idx, IndexPageType::ANY);
        _free.push(idx);
    }
    std::shared_ptr<Page> IndexFile::allocate() {
        if (_pages.size() == BLOCK_SIZE)
            throw std::runtime_error("IndexFile: Pages full!");
        _flag.set(_pages.size());
        _pages.push_back(std::make_shared<Page>());
        return std::static_pointer_cast<Page>(_pages.back());
    }
    std::shared_ptr<IndexPage> IndexFile::allocate(IndexPageType type) {
        if (_pages.size() == BLOCK_SIZE)
            throw std::runtime_error("IndexFile: Pages full!");
        _flag.set(_pages.size());
        set_page_type(_pages.size(), type);
        switch(type){
            case IndexPageType::ANY:
                _pages.push_back(std::make_shared<IndexPage>(_field, _pages.size()));
                break;
            case IndexPageType::INTERNAL:
                _pages.push_back(std::make_shared<InternalPage>(_field, _pages.size()));
                break;
            case IndexPageType::LEAF:
                _pages.push_back(std::make_shared<LeafPage>(_field, _pages.size()));
                break;
        }
        return std::static_pointer_cast<IndexPage>(_pages.back());
    }
    std::shared_ptr<IndexPage> IndexFile::get_index_page(size_t idx) const {
        if (idx >= _pages.size() || idx == 0)
            throw std::runtime_error("IndexFile: Out of range!");
        return std::static_pointer_cast<IndexPage>(_pages[idx]);
    }
    std::shared_ptr<InternalPage> IndexFile::get_internal_page(size_t idx) const {
        if (idx >= _pages.size() || idx == 0)
            throw std::runtime_error("IndexFile: Out of range!");
        if (page_type(idx) != IndexPageType::INTERNAL)
            throw std::runtime_error("IndexFile: Got Non-internal page!");
        return std::static_pointer_cast<InternalPage>(_pages[idx]);
    }
    std::shared_ptr<LeafPage> IndexFile::get_leaf_page(size_t idx) const {
        if (idx >= _pages.size() || idx == 0)
            throw std::runtime_error("IndexFile: Out of range!");
        if (page_type(idx) != IndexPageType::LEAF)
            throw std::runtime_error("IndexFile: Got Non-leaf page!");
        return std::static_pointer_cast<LeafPage>(_pages[idx]);
    }

    // tools
    size_t IndexFile::_next(IndexPageType type) {
        if (!_free.empty()) {
            size_t idx = _free.top();
            _free.pop();
            set_page_type(idx, type);
            switch(type){
                case IndexPageType::ANY:
                    std::static_pointer_cast<IndexPage>(_pages[idx])->reset(_field, idx);
                    break;
                case IndexPageType::INTERNAL:
                    std::static_pointer_cast<InternalPage>(_pages[idx])->reset(_field, idx);
                    break;
                case IndexPageType::LEAF:
                    std::static_pointer_cast<LeafPage>(_pages[idx])->reset(_field, idx);
                    break;
            }
            return idx;
        }
        this->allocate(type);
        return _pages.size() - 1;
    }
    size_t IndexFile::find_leaf(std::shared_ptr<Field> key) const{
        size_t pid = root();
        while(page_type(pid) == IndexPageType::INTERNAL){
            auto page = get_internal_page(pid);
            pid = page->ptr(page->count());
            for(size_t i = 0; i < page->count(); i++)
                if(*key < *(page->key(i))){
                    pid = page->ptr(i);
                    break;
                }
        }
        return pid;
    }

    // apis
    bool IndexFile::has(std::shared_ptr<Field> key) const{
        if(root() == 0)
            return false;
        size_t pid = find_leaf(key);
        auto page = get_leaf_page(pid);
        for(size_t i = 0; i < page->count(); i++)
            if(*key == *(page->key(i)))
                return true;
        return false;
    }
    Entry IndexFile::at(std::shared_ptr<Field> key) const{
        if(root() == 0)
            throw std::runtime_error("IndexFile: Key does not exist");
        size_t pid = find_leaf(key);
        auto page = get_leaf_page(pid);
        for(size_t i = 0; i < page->count(); i++)
            if(*key == *(page->key(i)))
                return page->entry(i);
        throw std::runtime_error("IndexFile: Key does not exist");
    }
    void IndexFile::range(size_t pid, std::vector<Entry>& entries, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const{
        if(page_type(pid) == IndexPageType::INTERNAL){
            auto page = get_internal_page(pid);
            if(*lower < *(page->key(0)))
                range(page->ptr(0), entries, lower, upper);
            for(size_t i = 1; i < page->count(); i++)
                if(*lower < *(page->key(i)) && *upper > *(page->key(i - 1)))
                    range(page->ptr(i), entries, lower, upper);
            if(*upper > *(page->key(page->count() - 1)))
                range(page->ptr(page->count()), entries, lower, upper);
        }
        else{
            auto page = get_leaf_page(pid);
            for(size_t i = 0; i < page->count(); i++)
                if(*lower <= *(page->key(i)) && *(page->key(i)) < *upper)
                    entries.push_back(page->entry(i));
        }
    }
    std::vector<Entry> IndexFile::range(std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const{
        if(root() == 0)
            return std::vector<Entry>();
        std::vector<Entry> entries;
        this->range(root(), entries, lower, upper);
        return entries;
    }
    void IndexFile::update_internal_pages(size_t pid, std::shared_ptr<Field> prev_key, std::shared_ptr<Field> key){
        while(pid != root()){
            pid = get_index_page(pid)->parent();
            auto p = get_internal_page(pid);
            auto idx = p->find(prev_key);
            if(idx < p->count())
                p->set_key(idx, key);
        }
    }
    bool IndexFile::insert(std::shared_ptr<Field> key, Entry value){
        if(root() == 0){
            set_root(_next(IndexPageType::LEAF));
            get_leaf_page(root())->insert(key, value);
            return true;
        }
        size_t pid = find_leaf(key), v;
        {
            auto p = get_leaf_page(pid);
            if(p->insert(key, value)){
                if(*(p->key(0)) == *key && p->count() > 1)
                    update_internal_pages(pid, p->key(1), key);
                return true;
            }
            auto q = get_leaf_page(_next(IndexPageType::LEAF));
            for(size_t i = p->count() / 2; i < p->count(); i++)
                q->insert_at(q->count(), p->key(i), p->entry(i));
            p->set_count(p->count() / 2);
            if(*key < *(q->key(0)))
                p->insert(key, value);
            else
                q->insert(key, value);
            if(p->id() == root()){
                set_root(_next(IndexPageType::INTERNAL));
                p->set_parent(root());
                get_internal_page(root())->set_ptr(0, p->id());
            }
            q->set_next_page(p->next_page());
            q->set_prev_page(p->id());
            q->set_parent(p->parent());
            p->set_next_page(q->id());
            pid = p->parent();
            key = q->key(0);
            v = q->id();
        }
        while(true){
            auto p = get_internal_page(pid);
            if(p->insert(key, v, false))
                return true;
            auto q = get_internal_page(_next(IndexPageType::INTERNAL));
            for(size_t i = p->count() / 2; i < p->count(); i++){
                q->insert_at(q->count(), p->key(i), p->ptr(i), true);
                get_index_page(p->ptr(i))->set_parent(q->id());
            }
            q->set_ptr(q->count(), p->ptr(p->count()));
            get_index_page(p->ptr(p->count()))->set_parent(q->id());
            p->set_count(p->count() / 2);
            if(*key < *(q->key(0))){
                p->insert(key, v, false);
                get_index_page(v)->set_parent(p->id());
            }
            else{
                q->insert(key, v, false);
                get_index_page(v)->set_parent(q->id());
            }
            p->erase_at(p->count() - 1, false);
            if(p->id() == root()){
                set_root(_next(IndexPageType::INTERNAL));
                p->set_parent(root());
                get_internal_page(root())->set_ptr(0, p->id());
            }
            q->set_parent(p->parent());
            pid = p->parent();
            key = p->key(p->count());
            v = q->id();
        }
        return false;
    }
    bool IndexFile::erase(std::shared_ptr<Field> key){
        if(root() == 0)
            return false;
        size_t pid = find_leaf(key), v;
        {
            auto p = get_leaf_page(pid);
            if(*(p->key(0)) == *key && p->count() > 1)
                update_internal_pages(pid, key, p->key(1));
            if(!p->erase(key))
                return false;
            if(p->id() == root() || p->count() >= p->capacity() / 2)
                return true;
            auto parent = get_internal_page(p->parent());
            bool qleft;
            size_t qid = 0, p_idx = parent->find(p->id());
            if(p_idx != 0 && get_leaf_page(p->prev_page())->count() > p->capacity() / 2)
                qleft = true, qid = p->prev_page();
            else if(p_idx != parent->count() && get_leaf_page(p->next_page())->count() > p->capacity() / 2)
                qleft = false, qid = p->next_page();
            if(qid != 0){
                auto q = get_leaf_page(qid);
                if(qleft){
                    p->insert_at(0, q->key(q->count() - 1), q->entry(q->count() - 1));
                    q->erase_at(q->count() - 1);
                    parent->set_key(p_idx - 1, p->key(0));
                }
                else{
                    p->insert_at(p->count(), q->key(0), q->entry(0));
                    q->erase_at(0);
                    parent->set_key(p_idx, q->key(0));
                }
                return true;
            }
            if(p_idx != parent->count())
                qid = p->next_page();
            else
                qid = pid, pid = get_leaf_page(qid)->prev_page(), p = get_leaf_page(pid);
            auto q = get_leaf_page(qid);
            for(size_t i = 0; i < q->count(); i++)
                p->insert_at(p->count(), q->key(i), q->entry(i));
            p->set_next_page(q->next_page());
            if(p->next_page() != 0)
                get_leaf_page(p->next_page())->set_prev_page(p->id());
            pid = q->parent();
            v = q->id();
            garbage_collect(q->id());
        }
        while(true){
            auto p = get_internal_page(pid);
            p->erase_at(p->find(v) - 1, false);
            if(p->id() == root()){
                if(p->count() == 0){
                    set_root(p->ptr(0));
                    get_index_page(root())->set_parent(0);
                    garbage_collect(p->id());
                }
                return true;
            }
            if(p->count() >= p->capacity() / 2)
                return true;
            auto parent = get_internal_page(p->parent());
            bool qleft;
            size_t qid = 0, p_idx = parent->find(p->id());
            if(p_idx != 0 && get_internal_page(parent->ptr(p_idx - 1))->count() > p->capacity() / 2)
                qleft = true, qid = parent->ptr(p_idx - 1);
            else if(p_idx != parent->count() && get_internal_page(parent->ptr(p_idx + 1))->count() > p->capacity() / 2)
                qleft = false, qid = parent->ptr(p_idx + 1);
            if(qid != 0){
                auto q = get_internal_page(qid);
                if(qleft){
                    p->insert_at(0, parent->key(p_idx - 1), q->ptr(q->count()), true);
                    parent->set_key(p_idx - 1, q->key(q->count() - 1));
                    q->erase_at(q->count() - 1, false);
                    get_index_page(p->ptr(0))->set_parent(p->id());
                }
                else{
                    p->insert_at(p->count(), parent->key(p_idx), q->ptr(0), false);
                    parent->set_key(p_idx, q->key(0));
                    q->erase_at(0, true);
                    get_index_page(p->ptr(p->count()))->set_parent(p->id());
                }
                return true;
            }
            if(p_idx != parent->count())
                qid = parent->ptr(p_idx + 1);
            else
                qid = pid, pid = parent->ptr(--p_idx), p = get_internal_page(pid);
            auto q = get_internal_page(qid);
            p->insert_at(p->count(), parent->key(p_idx), q->ptr(0), false);
            get_index_page(q->ptr(0))->set_parent(p->id());
            for(size_t i = 0; i < q->count(); i++){
                p->insert_at(p->count(), q->key(i), q->ptr(i + 1), false);
                get_index_page(q->ptr(i + 1))->set_parent(p->id());
            }
            pid = q->parent();
            v = q->id();
            garbage_collect(q->id());
        }
        return false;
    }


}// namespace Neru
