#ifndef NERU_DATA_FILE_H
#define NERU_DATA_FILE_H

#include "file.h"
#include "page/data_page.h"
#include "record/record.h"
#include "utils/utils.h"

#include <queue>

namespace Neru {

    class DataFile : public File {
    public:
        // constructors
        DataFile(std::string name, Header head, bool create = false);

        // info
        static FileType type();
        size_t length() const;
        std::string info() const;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const DataFile &file);

        // tools
        std::shared_ptr<DataPage> allocate();
        DataFile &push(std::shared_ptr<DataPage> page);
        DataFile &pop();
        std::shared_ptr<DataPage> get(size_t idx) const;

        // apis
        Entry insert(std::shared_ptr<Record const> record);
        Entry update(Entry entry, std::shared_ptr<Record const> record, bool overwrite = true);
        std::shared_ptr<Record>  get(Entry entry);

        bool remove(Entry entry);
        std::vector<std::shared_ptr<Record>> select() const;
        void select_with_entry(std::vector<std::pair<std::shared_ptr<Record>, Entry>> &records) const;

    private:
        Header _head;
        Entry _front;
        std::priority_queue<Entry> _free;

        // tools
        Entry _next();
    };

}// namespace Neru

#endif//NERU_DATA_FILE_H
