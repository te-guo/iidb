#include "record/record.h"
#include "utils/utils.h"

using namespace Neru;

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
