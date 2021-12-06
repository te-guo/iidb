#include "index_page.h"

namespace Neru {

    // constructors
    IndexPage::IndexPage(FieldType field, size_t id_) : _field(field), _id(id_), _parent(0) {
        _key_size = Field::size(field);
    }
    void IndexPage::reset(FieldType field, size_t id_){
        _field = field;
        _id = id_;
        _parent = 0;
        _key_size = Field::size(field);
        memset(_data.get(), 0, sizeof(_data));
    }

    // info
    size_t IndexPage::key_size() const { return _key_size; }
    size_t IndexPage::capacity() const { return _capacity; }
    IndexPageType IndexPage::type() { return IndexPageType::ANY; }
    size_t IndexPage::id() const { return _id; }
    size_t IndexPage::parent() const { return _parent; }
    size_t IndexPage::count(){
        return read_int(0);
    }

    // tools
    void IndexPage::set_id(size_t val) { _id = val; }
    void IndexPage::set_parent(size_t val) { _parent = val; }
    void IndexPage::set_count(size_t val) {
        write_int(val, 0);
    }



    InternalPage::InternalPage(FieldType field, size_t id_) : IndexPage(field, id_) {
        _capacity = (PAGE_SIZE - INTERNAL_PAGE_METADATA_SIZE - sizeof(size_t)) / (key_size() + sizeof(size_t));
    }
    void InternalPage::reset(FieldType field, size_t id_){
        _field = field;
        _id = id_;
        _parent = 0;
        _key_size = Field::size(field);
        memset(_data.get(), 0, sizeof(_data));
        _capacity = (PAGE_SIZE - INTERNAL_PAGE_METADATA_SIZE - sizeof(size_t)) / (key_size() + sizeof(size_t));
    }
    IndexPageType InternalPage::type() { return IndexPageType::INTERNAL; }
    std::shared_ptr<Field> InternalPage::key(size_t idx){
        size_t offset = INTERNAL_PAGE_METADATA_SIZE + sizeof(size_t) + idx * (sizeof(size_t) + _key_size);
        switch(_field){
            case FieldType::INT:
                return std::make_shared<IntField>(_data.get() + offset);
            case FieldType::DOUBLE:
                return std::make_shared<DoubleField>(_data.get() + offset);
        }
        return std::shared_ptr<Field>();
    }
    void InternalPage::set_key(size_t idx, std::shared_ptr<Field> val){
        size_t offset = INTERNAL_PAGE_METADATA_SIZE + sizeof(size_t) + idx * (sizeof(size_t) + _key_size);
        switch(_field){
            case FieldType::INT:
                std::static_pointer_cast<IntField>(val)->store(_data.get() + offset);
                break;
            case FieldType::DOUBLE:
                std::static_pointer_cast<DoubleField>(val)->store(_data.get() + offset);
                break;
        }
    }
    size_t InternalPage::ptr(size_t idx){
        return read_int(INTERNAL_PAGE_METADATA_SIZE + idx * (sizeof(size_t) + _key_size));
    }
    void InternalPage::set_ptr(size_t idx, size_t val){
        write_int(val, INTERNAL_PAGE_METADATA_SIZE + idx * (sizeof(size_t) + _key_size));
    }
    size_t InternalPage::find(std::shared_ptr<Field> k){
        for(size_t i = 0; i < count(); i++)
            if(*k == *key(i))
                return i;
        return count();
    }
    size_t InternalPage::find(size_t p){
        for(size_t i = 0; i <= count(); i++)
            if(p == ptr(i))
                return i;
        return count() + 1;
    }
    void InternalPage::insert(size_t idx, std::shared_ptr<Field> k, size_t p, bool ptr_on_left){
        for(size_t j = count(); j > idx; j--){
            set_key(j, key(j - 1));
            set_ptr(j + 1, ptr(j));
        }
        set_key(idx, k);
        if(ptr_on_left){
            set_ptr(idx + 1, ptr(idx));
            set_ptr(idx, p);
        }
        else
            set_ptr(idx + 1, p);
        set_count(count() + 1);
    }
    bool InternalPage::insert(std::shared_ptr<Field> k, size_t p, bool ptr_on_left){
        if(count() >= capacity())
            return false;
        for(size_t i = 0; i <= count(); i++)
            if(i == count() || *k < *key(i)){
                insert(i, k, p, ptr_on_left);
                return true;
            }
            else if(*k == *key(i))
                throw std::runtime_error("IndexPage: insert duplicated keys");
        return false;
    }
    void InternalPage::erase(size_t idx, bool ptr_on_left){
        set_count(count() - 1);
        if(ptr_on_left)
            set_ptr(idx, ptr(idx + 1));
        for(size_t j = idx; j < count(); j++){
            set_key(j, key(j + 1));
            set_ptr(j + 1, ptr(j + 2));
        }
    }
    bool InternalPage::erase(std::shared_ptr<Field> k, bool ptr_on_left){
        for(size_t i = 0; i < count(); i++)
            if(*k == *key(i)){
                erase(i, ptr_on_left); 
                return true;
            }
        return false;
    }


    LeafPage::LeafPage(FieldType field, size_t id_) : IndexPage(field, id_) {
        _capacity = (PAGE_SIZE - LEAF_PAGE_METADATA_SIZE) / (key_size() + sizeof(Entry));
    }
    void LeafPage::reset(FieldType field, size_t id_){
        _field = field;
        _id = id_;
        _parent = 0;
        _key_size = Field::size(field);
        memset(_data.get(), 0, sizeof(_data));
        _capacity = (PAGE_SIZE - LEAF_PAGE_METADATA_SIZE) / (key_size() + sizeof(Entry));
    }
    IndexPageType LeafPage::type() { return IndexPageType::LEAF; }
    size_t LeafPage::prev_page(){
        return read_int(8);
    }
    void LeafPage::set_prev_page(size_t val){
        write_int(val, 8);
    }
    size_t LeafPage::next_page(){
        return read_int(16);
    }
    void LeafPage::set_next_page(size_t val){
        write_int(val, 16);
    }
    std::shared_ptr<Field> LeafPage::key(size_t idx){
        size_t offset = LEAF_PAGE_METADATA_SIZE + idx * (sizeof(Entry) + _key_size);
        switch(_field){
            case FieldType::INT:
                return std::make_shared<IntField>(_data.get() + offset);
            case FieldType::DOUBLE:
                return std::make_shared<DoubleField>(_data.get() + offset);
        }
        return std::shared_ptr<Field>();
    }
    void LeafPage::set_key(size_t idx, std::shared_ptr<Field> val){
        size_t offset = LEAF_PAGE_METADATA_SIZE + idx * (sizeof(Entry) + _key_size);
        switch(_field){
            case FieldType::INT:
                std::static_pointer_cast<IntField>(val)->store(_data.get() + offset);
                break;
            case FieldType::DOUBLE:
                std::static_pointer_cast<DoubleField>(val)->store(_data.get() + offset);
                break;
        }
    }
    Entry LeafPage::entry(size_t idx){
        size_t offset = LEAF_PAGE_METADATA_SIZE + _key_size + idx * (sizeof(Entry) + _key_size);
        return Entry(read_int(offset), read_int(offset + sizeof(size_t)));
    }
    void LeafPage::set_entry(size_t idx, Entry val){
        size_t offset = LEAF_PAGE_METADATA_SIZE + _key_size + idx * (sizeof(Entry) + _key_size);
        write_int(val.page(), offset);
        write_int(val.slot(), offset + sizeof(size_t));
    }
    void LeafPage::insert(size_t idx, std::shared_ptr<Field> k, Entry e){
        for(size_t j = count(); j > idx; j--){
            set_key(j, key(j - 1));
            set_entry(j, entry(j - 1));
        }
        set_key(idx, k);
        set_entry(idx, e);
        set_count(count() + 1);
    }
    bool LeafPage::insert(std::shared_ptr<Field> k, Entry e){
        for(size_t i = 0; i <= count(); i++)
            if(i == count() || *k < *key(i)){
                if(count() >= capacity())
                    return false;
                insert(i, k, e);
                return true;
            }
            else if(*k == *key(i)){
                set_entry(i, e);
                return true;
            }
        return false;
    }
    void LeafPage::erase(size_t idx){
        set_count(count() - 1);
        for(size_t j = idx; j < count(); j++){
            set_key(j, key(j + 1));
            set_entry(j, entry(j + 1));
        }
    }
    bool LeafPage::erase(std::shared_ptr<Field> k){
        for(size_t i = 0; i < count(); i++)
            if(*k == *key(i)){
                erase(i);
                return true;
            }
        return false;
    }

}// namespace Neru