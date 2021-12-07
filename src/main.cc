#include "field/fields.h"
#include "file/files.h"
#include "index/index.h"
#include "page/pages.h"
#include "record/record.h"
#include "table/table.h"
#include "utils/utils.h"

#include <iostream>

using namespace Neru;

using namespace std::string_literals;

size_t Entry::_capacity;

bool field_test() {
    std::cerr << "---Field---" << std::endl;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[640]);
    std::cerr << utils::from("INT") << ' ' << utils::from("DOUBLE") << std::endl;
    IntField int_field1(3);
    std::cerr << int(int_field1) << ' ' << int_field1 << std::endl;
    IntField int_field2("3");
    auto buf = int_field1.store(pool.get());
    IntField int_field3(pool.get());
    // true false false true false true
    std::cerr << std::boolalpha << (int_field1 == int_field2) << ' ' << (int_field1 != int_field2) << ' ' << (int_field1 < int_field2) << ' ' << (int_field1 <= int_field2) << ' ' << (int_field1 > int_field2) << ' ' << (int_field1 >= int_field2) << std::endl;
    std::cerr << int_field3 << std::endl;
    IntField int_field4(4);
    // false true true true false false
    std::cerr << std::boolalpha << (int_field3 == int_field4) << ' ' << (int_field3 != int_field4) << ' ' << (int_field3 < int_field4) << ' ' << (int_field3 <= int_field4) << ' ' << (int_field3 > int_field4) << ' ' << (int_field3 >= int_field4) << std::endl;
    DoubleField double_field1(3.14);
    std::cerr << double(double_field1) << ' ' << double_field1 << std::endl;
    DoubleField double_field2("3.1415926");
    std::cerr << double_field2 << std::endl;
    double_field1.store(buf);
    DoubleField double_field3(buf);
    std::cerr << double_field3 << std::endl;
    // false true false false true true
    std::cerr << std::boolalpha << (double_field2 == double_field3) << ' ' << (double_field2 != double_field3) << ' ' << (double_field2 < double_field3) << ' ' << (double_field2 <= double_field3) << ' ' << (double_field2 > double_field3) << ' ' << (double_field2 >= double_field3) << std::endl;
    auto &field1 = static_cast<Field &>(int_field1);
    auto &field2 = static_cast<Field &>(double_field2);
    std::cerr << field1 << std::endl
              << field2 << std::endl;
    // false true
    std::cerr << (field1 == field2) << ' ' << (field1 != field2) << std::endl;
    // std::runtime_error
    // std::cerr << (field1 < field2) << ' ' << (field1 <= field2) << ' ' << (field1 > field2) << ' ' << (field1 >= field2) << std::endl;
    return true;
}

bool record_test() {
    std::cerr << "---Record---" << std::endl;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[640]);
    std::cerr << utils::from({"INT", "DOUBLE", "DOUBLE"}) << std::endl;
    Record record1({std::make_shared<IntField>(3), std::make_shared<DoubleField>("3.14")});
    std::cerr << record1 << std::endl;
    record1.push(std::static_pointer_cast<Field>(std::make_shared<DoubleField>(3.1415926)));
    std::cerr << *record1.get(2) << std::endl;
    record1.store(pool.get());
    Header head = {FieldType::INT, FieldType::DOUBLE, FieldType::DOUBLE};
    std::cerr << head << std::endl;
    Record record2(pool.get(), head);
    // true false
    std::cerr << std::boolalpha << (record1 == record2) << ' ' << (record1 != record2) << std::endl;
    Record record3({"3", "3.14", "3.1415926"}, head);
    // true false
    std::cerr << std::boolalpha << (record2 == record3) << ' ' << (record2 != record3) << std::endl;
    record3[1] = std::make_shared<IntField>(4);
    std::cerr << record3 << std::endl;
    record3.push(std::make_shared<IntField>(314));
    // false true
    std::cerr << std::boolalpha << (record2 == record3) << ' ' << (record2 != record3) << std::endl;
    return true;
}

bool page_test() {
    std::cerr << "---Page---" << std::endl;
    Page page1;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[640]);
    std::copy_n("Hello, world!\0", 14, pool.get());
    std::cerr << pool.get() << std::endl;
    page1.write(pool.get(), 14, 0);
    page1.read(pool.get() + 13, 14, 0);
    std::cerr << pool.get() << std::endl;
    page1.write(pool.get(), 27, 13);
    page1.read(pool.get(), 40, 0);
    std::cerr << pool.get() << std::endl;
    return true;
}

bool data_page_test() {
    std::cerr << "---Data Page---" << std::endl;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[12800]);
    std::bitset<FLAG_SIZE> bitset{114514};
    std::copy_n((uint8_t *) &bitset, sizeof(bitset), pool.get());
    Header head = {FieldType::INT, FieldType::DOUBLE};
    DataPage data_page1(head);
    //    data_page1.write(pool.get(), PAGE_SIZE, 0);
    //    std::cerr << data_page1._flag->to_ulong() << std::endl;
    std::vector<std::string> data1 = {"1", "2.3"};
    auto record1 = std::make_shared<Record>(data1, head);
    data_page1.set(114, record1);
    std::cerr << *data_page1.get(114) << std::endl;
    //    std::cerr << *data_page1.get(514) << std::endl;
    data_page1.read(pool.get(), PAGE_SIZE, 0);
    DataPage data_page2(head);
    data_page2.write(pool.get(), PAGE_SIZE, 0);
    std::cerr << *data_page1.get(114) << std::endl;
    return true;
}

bool file_test() {
    std::cerr << "---File---" << std::endl;
    std::cerr << File::init() << std::endl;
    {
        File file1("foo", true);
        auto page1 = file1.allocate();
        std::shared_ptr<uint8_t[]> pool(new uint8_t[12800]);
        std::copy_n("Hello, world!\0", 14, pool.get());
        page1->write(pool.get(), 14, 0);
        auto page2 = std::make_shared<Page>();
        page2->write(pool.get(), 14, 14);
        file1.push(page2);
        std::cerr << file1 << std::endl;
        //        file1.save();
    }
    File file2("foo");
    std::cerr << file2 << std::endl;
    std::cerr << file2[0]->raw() << std::endl;
    std::cerr << file2[1]->raw() + 14 << std::endl;
    return true;
}

bool entry_test() {
    std::cerr << "---Entry---" << std::endl;
    Header head = {FieldType::INT, FieldType::DOUBLE};
    Entry::set(DATA_CAPACITY / utils::size(head));
    Entry entry1(0, 2);
    std::cerr << entry1 << std::endl;
    std::cerr << ++entry1 << std::endl;
    std::cerr << --entry1 << std::endl;
    std::cerr << (entry1 += 2) << std::endl;
    std::cerr << (entry1 += 114514) << std::endl;
    std::cerr << (entry1 -= 114) << std::endl;
    //    std::cerr << (entry1 -= 114514) << std::endl;
    Entry entry2(entry1);
    // true false false
    std::cerr << (entry1 == entry2) << ' ' << (entry1 != entry2) << ' ' << (entry1 < entry2) << std::endl;
    ++entry2;
    // false true true
    std::cerr << (entry1 == entry2) << ' ' << (entry1 != entry2) << ' ' << (entry1 < entry2) << std::endl;
    return true;
}

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
        auto int_field = std::static_pointer_cast<IntField>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
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
        auto int_field = std::static_pointer_cast<IntField>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Update passed." << std::endl;
    for (size_t i = 0; i + i < entries.size(); ++i)
        data_file.remove(entries[i]);
    records = data_file.select();
    std::cerr << entries.size() << ' ' << records.size() << std::endl;
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Remove passed." << std::endl;
    return true;
}

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
            auto int_field = std::static_pointer_cast<IntField>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
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
            auto int_field = std::static_pointer_cast<IntField>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
            if (int32_t(*int_field) * 2 + 514 != double(*double_field))
                return false;
        }
        std::cerr << "Update passed." << std::endl;
        for (size_t i = 0; i + i < entries.size(); ++i)
            table.remove(entries[i]);
        records = table.select();
        std::cerr << entries.size() << ' ' << records.size() << std::endl;
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
            if (int32_t(*int_field) * 2 + 514 != double(*double_field))
                return false;
        }
        std::cerr << "Remove passed." << std::endl;
    }
    Table after("table", head);
    auto records = after.select();
    std::cerr << records.size() << std::endl;
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
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

bool index_test() {
    std::cerr << "---Index---" << std::endl;
    Index index;
    for (size_t i = 0; i < 514; ++i) {
        std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
        index.insert(int_field, {0, i});
    }
    for (size_t i = 0; i < 514; ++i) {
        Entry entry = index.at(std::make_shared<IntField>(i));
        if (entry.slot() != i)
            return false;
    }
    std::cerr << "At passed." << std::endl;
    auto entries = index.range(std::make_shared<IntField>(114), std::make_shared<IntField>(411));
    std::cerr << entries.size() << std::endl;
    bool flag[entries.size()];
    for (size_t i = 0; i < entries.size(); ++i)
        flag[i] = false;
    for (auto entry : entries)
        flag[entry.slot() - 114] = true;
    for (size_t i = 0; i < entries.size(); ++i)
        if (!flag[i])
            return false;
    std::cerr << "Range passed." << std::endl;
    index.insert(std::make_shared<IntField>(114), {514, 0});
    std::cerr << index.at(std::make_shared<IntField>(114)) << std::endl;
    // false true
    std::cerr << std::boolalpha << index.erase(std::make_shared<IntField>(1919)) << ' ' << index.erase(std::make_shared<IntField>(114)) << std::endl;
    // false
    std::cerr << std::boolalpha << index.has(std::make_shared<IntField>(114)) << std::endl;
    return true;
}

bool table_with_index_test() {
    std::cerr << "---Table with Index---" << std::endl;
    // true
    std::cerr << File::init() << std::endl;
    Header head = {FieldType::INT, FieldType::DOUBLE};
    Entry::set(DATA_CAPACITY / utils::size(head));
    {
        Table table("table_with_index", head, true);
        if(table.has_index(0) || table.has_index(1)) return false;
        if(!table.build_index(1)) return false;
        std::vector<Entry> entries;
        for (size_t i = 0; i < 114514; ++i) {
            std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
            std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 114);
            auto record = std::make_shared<Record>(std::vector{int_field, double_field});
            entries.push_back(table.insert_with_index(1, record));
        }
        if(!table.build_index(0)) return false;
        auto records = table.select();
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
            if (int32_t(*int_field) * 2 + 114 != double(*double_field))
                return false;
        }
        std::cerr << "Insert passed." << std::endl;
        if(!table.has_index(0) || !table.has_index(1)) return false;
        if(!table.delete_index(1)) return false;
        if(table.has_index(1)) return false;
        for (size_t i = 0; i < entries.size(); ++i) {
            auto entry = entries[i];
            std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
            std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 514);
            auto record = std::make_shared<Record>(std::vector{int_field, double_field});
            table.update_with_index(0, int_field, record);
        }
        if(!table.build_index(1)) return false;
        records = table.select();
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
            if (int32_t(*int_field) * 2 + 514 != double(*double_field))
                return false;
        }
        std::cerr << "Update passed." << std::endl;
        for (size_t i = 0; i + i < entries.size(); ++i)
            if(i & 1u)
                table.remove_with_index(0, std::make_shared<IntField>(i));
            else
                table.remove_with_index(1, std::make_shared<DoubleField>(i * 2 + 514));
        for (size_t i = 0; i + i < entries.size(); ++i)
            if(i + i < entries.size())
                if(i & 1u){
                    if(table.key_has(0, std::make_shared<IntField>(i)))
                        return false;
                }
                else{
                    if(table.key_has(1, std::make_shared<DoubleField>(i * 2 + 514)))
                        return false;
                }
            else
                if(i & 1u){
                    if(!table.key_has(0, std::make_shared<IntField>(i)))
                        return false;
                    if(table.key_at(0, std::make_shared<IntField>(i)) != entries[i])
                        return false;
                }
                else{
                    if(!table.key_has(1, std::make_shared<DoubleField>(i * 2 + 514)))
                        return false;
                    if(table.key_at(1, std::make_shared<DoubleField>(i * 2 + 514)) != entries[i])
                        return false;
                }
        records = table.select();
        std::cerr << entries.size() << ' ' << records.size() << std::endl;
        for (auto record : records) {
            auto int_field = std::static_pointer_cast<IntField>(record->get(0));
            auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
            if (int32_t(*int_field) * 2 + 514 != double(*double_field))
                return false;
        }
        std::cerr << "Remove passed." << std::endl;
    }
    Table after("table_with_index", head);
    if(!after.has_index(0) || !after.has_index(1)) return false;
    auto records = after.select();
    std::cerr << records.size() << std::endl;
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    std::cerr << "Load passed." << std::endl;
    for (size_t i = 0; i + i < 114514; ++i) {
        std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
        std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 514);
        auto record = std::make_shared<Record>(std::vector{int_field, double_field});
        after.insert_with_index(0, record);
    }
    records = after.select();
    for (auto record : records) {
        auto int_field = std::static_pointer_cast<IntField const>(record->get(0));
        auto double_field = std::static_pointer_cast<DoubleField const>(record->get(1));
        if (int32_t(*int_field) * 2 + 514 != double(*double_field))
            return false;
    }
    if(!after.delete_index(1) || !after.delete_index(0)) return false;
    std::cerr << "Load Insert passed." << std::endl;
    return true;
}

int main() {
    field_test();
    record_test();
    page_test();
    data_page_test();
    file_test();
    entry_test();
    std::cerr << data_file_test() << std::endl;
    std::cerr << table_test() << std::endl;
    std::cerr << index_test() << std::endl;
    std::cerr << table_with_index_test() << std::endl;
    std::cerr << "---Exiting---" << std::endl;
    return 0;
}
