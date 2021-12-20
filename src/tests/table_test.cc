#include "table/table.h"
#include "utils/utils.h"

using namespace Neru;

bool table_test() {
    std::cerr << "---Table---" << std::endl;
    // true
    std::cerr << File::init() << std::endl;
    Header head = {FieldType::INT, FieldType::DOUBLE};
    Entry::set(DATA_CAPACITY / utils::size(head));
    {
        Table table("table", head, true);
        std::vector<Entry> entries;
        for (size_t i = 0; i < 114514; ++i) {
            std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
            std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 114);
            auto record = std::make_shared<Record>(std::vector{int_field, double_field});
            entries.push_back(table.insert(record));
        }
        auto records = table.select();
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
            if (int32_t(*int_field) * 2 + 114 != double(*double_field))
                return false;
        }
        std::cerr << "Insert passed." << std::endl;
        for (size_t i = 0; i < entries.size(); ++i) {
            auto entry = entries[i];
            std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
            std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 514);
            auto record = std::make_shared<Record>(std::vector{int_field, double_field});
            table.update(entry, record);
        }
        records = table.select();
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
            if (int32_t(*int_field) * 2 + 514 != double(*double_field))
                return false;
        }
        std::cerr << "Update passed." << std::endl;
        for (size_t i = 0; i + i < entries.size(); ++i)
            table.remove(entries[i]);
        records = table.select();
        std::cerr << entries.size() << ' ' << records.size() << std::endl;
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
            if (int32_t(*int_field) * 2 + 514 != double(*double_field))
                return false;
        }
        std::cerr << "Remove passed." << std::endl;
    }
    Table after("table", head);
    auto records = after.select();
    std::cerr << records.size() << std::endl;
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Load passed." << std::endl;
    for (size_t i = 0; i < 114514; ++i) {
        std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
        std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 514);
        auto record = std::make_shared<Record>(std::vector{int_field, double_field});
        after.insert(record);
    }
    records = after.select();
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Load Insert passed." << std::endl;
    return true;
}
