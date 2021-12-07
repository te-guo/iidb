#ifndef NERU_INDEX_FILE_H
#define NERU_INDEX_FILE_H

#include "file.h"
#include "page/index_page.h"
#include "record/record.h"
#include "utils/utils.h"
#include <vector>
#include <queue>

namespace Neru {

#define INDEX_FILE_METADATA_SIZE 8

    class IndexFile : public File {
    public:
        // constructors
        IndexFile(std::string name, FieldType field, bool create = false);

        // info
        static FileType type();
        std::string info() const;
        void debug();

        // operators
        std::shared_ptr<Page> &operator[](size_t idx);
        
        // tools
        IndexPageType page_type(size_t idx) const;
        void set_page_type(size_t idx, IndexPageType type);
        size_t root() const;
        void set_root(size_t idx);
        std::shared_ptr<Page> allocate();
        std::shared_ptr<IndexPage> allocate(IndexPageType type);
        void garbage_collect(size_t idx);
        std::shared_ptr<IndexPage> get_index_page(size_t idx) const;
        std::shared_ptr<InternalPage> get_internal_page(size_t idx) const;
        std::shared_ptr<LeafPage> get_leaf_page(size_t idx) const;

        // apis
        bool has(std::shared_ptr<Field> key) const;
        Entry at(std::shared_ptr<Field> key) const;
        std::vector<Entry> range(std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const;
        bool insert(std::shared_ptr<Field> key, Entry value);
        bool erase(std::shared_ptr<Field> key);

    private:
        FieldType _field;
        std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> _free;

        // tools
        size_t _next(IndexPageType type);
        size_t find_leaf(std::shared_ptr<Field> key) const;
        void range(size_t pid, std::vector<Entry>& entries, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) const;
        void update_internal_pages(size_t pid, std::shared_ptr<Field> prev_key, std::shared_ptr<Field> key);
    };

}// namespace Neru

#endif//NERU_INDEX_FILE_H
