#include "table.h"

namespace Neru {

    // constructors
    Table::Table(std::string name, Header head, bool create) : _name(name), _head(head), _file(name, head, create) {}

    // info
    Header Table::type() const { return _head; }
    size_t Table::size() const { return _file.length(); }
    std::string Table::info() const { return "Table:\n\t" + _name + "\n\t" + utils::info(_head); }

    // apis
    Entry Table::insert(std::shared_ptr<Record> record) { return _file.insert(record); }
    Entry Table::update(Entry entry, std::shared_ptr<Record> record) { return _file.update(entry, record); }
    bool Table::remove(Entry entry) { return _file.remove(entry); }
    std::shared_ptr<Record> Table::get(Entry entry)  { return _file.get(entry); }
    std::vector<std::shared_ptr<Record>> Table::select() const { return _file.select(); }

}// namespace Neru
