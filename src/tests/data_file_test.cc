#include "file/data_file.h"
#include "utils/utils.h"

using namespace Neru;

bool data_file_test() {
    std::cerr << "---Data File---" << std::endl;
    // true
    std::cerr << File::init() << std::endl;
    Header head = {FieldType::INT, FieldType::DOUBLE};
    Entry::set(DATA_CAPACITY / utils::size(head));
    {
        DataFile data_file1("bar", head, true);
        auto data_page1 = data_file1.allocate();
        std::vector<std::string> data1 = {"1", "2.3"};
        auto record1 = std::make_shared<Record>(data1, head);
        data_page1->set(114, record1);
        (*record1)[1] = std::make_shared<DoubleField>("3.14");
        data_page1->set(514, record1);
        auto data_page2 = std::make_shared<DataPage>(head);
        data_page2->set(114, record1);
        data_page2->set(514, record1);
        data_file1.push(data_page2);
    }
    DataFile data_file2("bar", head);
    std::cerr << data_file2 << std::endl;
    std::cerr << *data_file2.get(0)->get(114) << std::endl;
    std::cerr << *data_file2.get(1)->get(514) << std::endl;
    auto records = data_file2.get(0)->select();
    for (auto record : records)
        std::cerr << *record << std::endl;
    records = data_file2.select();
    for (auto record : records)
        std::cerr << *record << std::endl;
    DataFile data_file("baz", head, true);
    std::vector<Entry> entries;
    for (size_t i = 0; i < 114514; ++i) {
        std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
        std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 114);
        auto record = std::make_shared<Record>(std::vector{int_field, double_field});
        entries.push_back(data_file.insert(record));
    }
    records = data_file.select();
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
        data_file.update(entry, record);
    }
    records = data_file.select();
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Update passed." << std::endl;
    for (size_t i = 0; i + i < entries.size(); ++i)
        data_file.remove(entries[i]);
    records = data_file.select();
    std::cerr << entries.size() << ' ' << records.size() << std::endl;
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Remove passed." << std::endl;
    return true;
}
