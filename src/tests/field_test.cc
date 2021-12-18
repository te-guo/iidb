#include "field/fields.h"
#include "utils/utils.h"

using namespace Neru;

bool field_test() {
    std::cerr << "---Field---" << std::endl;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[640]);
    std::cerr << utils::from("INT") << ' ' << utils::from("DOUBLE") << std::endl;
    IntField int_field1(3);
    std::cerr << int(int_field1) << ' ' << int_field1 << std::endl;
    int ans = int_field1 + int_field1;
    std::cerr << ans << std::endl;
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
