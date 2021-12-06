#ifndef NERU_INDEX_PAGE_H
#define NERU_INDEX_PAGE_H

#include "page.h"
#include "field/fields.h"
#include "record/record.h"
#include "utils/utils.h"

#include <cstring>
#include <bitset>

namespace Neru {

    enum class IndexPageType { ANY,
                          INTERNAL,
                          LEAF };

    class IndexPage : public Page {
    public:
        // constructors
        IndexPage() = delete;
        explicit IndexPage(FieldType field, size_t id_);

        // info
        size_t key_size() const;
        size_t capacity() const;
        IndexPageType type();
        size_t count();
        size_t id() const;
        size_t parent() const;

        // tools
        void set_count(size_t val);
        void set_id(size_t val);
        void set_parent(size_t val);
        void reset(FieldType field, size_t id_);

    protected:
        size_t _id, _parent;
        size_t _capacity;
        size_t _key_size;
        FieldType _field;
    };

#define INTERNAL_PAGE_METADATA_SIZE 8

    class InternalPage : public IndexPage {
    public:
        explicit InternalPage(FieldType field, size_t id_);

        //tool
        void reset(FieldType field, size_t id_);
        IndexPageType type();
        std::shared_ptr<Field> key(size_t idx);
        void set_key(size_t idx, std::shared_ptr<Field> val);
        size_t ptr(size_t idx);
        void set_ptr(size_t idx, size_t val);
        size_t find(std::shared_ptr<Field> k);
        size_t find(size_t p);
        void insert(size_t idx, std::shared_ptr<Field> k, size_t p, bool ptr_on_left = false);
        bool insert(std::shared_ptr<Field> k, size_t p, bool ptr_on_left = false);
        void erase(size_t idx, bool ptr_on_left = false);
        bool erase(std::shared_ptr<Field> k, bool ptr_on_left = false);
    };

#define LEAF_PAGE_METADATA_SIZE 24

    class LeafPage : public IndexPage {
    public:
        explicit LeafPage(FieldType field, size_t id_);

        //tool
        void reset(FieldType field, size_t id_);
        IndexPageType type();
        size_t prev_page();
        void set_prev_page(size_t val);
        size_t next_page();
        void set_next_page(size_t val);
        std::shared_ptr<Field> key(size_t idx);
        void set_key(size_t idx, std::shared_ptr<Field> val);
        Entry entry(size_t idx);
        void set_entry(size_t idx, Entry val);
        void insert(size_t idx, std::shared_ptr<Field> k, Entry e);
        bool insert(std::shared_ptr<Field> k, Entry e);
        void erase(size_t idx);
        bool erase(std::shared_ptr<Field> k);
    };

}// namespace Neru

#endif//NERU_INDEX_PAGE_H
