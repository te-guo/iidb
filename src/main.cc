#include "field/fields.h"
#include "record/record.h"
#include "table/table.h"
#include "instance/instance.h"

using namespace Neru;

void field_test() {
  std::cerr << "---Field---" << std::endl;
  std::shared_ptr<uint8_t[]> buf(new uint8_t[640]);
  IntField int_field1(3);
  int_field1.store(buf.get());
  IntField int_field2(buf.get());
  std::cerr << int_field1 << std::endl << int_field2 << std::endl;
  std::cerr << std::boolalpha << (int_field1 == int_field2) << ' '
            << (int_field1 != int_field2) << ' ' << (int_field1 < int_field2)
            << ' ' << (int_field1 <= int_field2) << ' '
            << (int_field1 > int_field2) << ' ' << (int_field1 >= int_field2)
            << ' ' << std::endl;
  Field &field1 = static_cast<Field &>(int_field1);
  Field &field2 = static_cast<Field &>(int_field2);
  std::cerr << field1 << std::endl << field2 << std::endl;
  std::cerr << std::boolalpha << (field1 == field2) << std::endl;
  int_field2 = IntField(4);
  std::cerr << std::boolalpha << (int_field1 == int_field2) << std::endl;
  FloatField float_field(3.14);
  DoubleField double_field(3.1415926);
  std::cerr << float_field << std::endl << double_field << std::endl;
}

void record_test() {
  std::cerr << "---Record---" << std::endl;
  std::shared_ptr<uint8_t[]> buf(new uint8_t[640]);
  Record record1({std::make_shared<IntField>(3),
                  std::make_shared<FloatField>(3.14),
                  std::make_shared<DoubleField>(3.1415926)});
  std::cerr << record1 << std::endl;
  record1.store(buf.get());
  Record record2(buf.get(),
                 {FieldType::INT, FieldType::FLOAT, FieldType::DOUBLE});
  std::cerr << record2 << std::endl;
  Record record3(
      {std::make_shared<IntField>(3), std::make_shared<FloatField>(3.14)});
  Record record4({std::make_shared<IntField>(3),
                  std::make_shared<FloatField>(3.14),
                  std::make_shared<DoubleField>(3.14)});
  std::cerr << (record1 == record2) << ' ' << (record1 == record3) << ' '
            << (record1 == record4) << std::endl;
  std::cerr << (record1 != record2) << ' ' << (record1 != record3) << ' '
            << (record1 != record4) << std::endl;
}

void table_test() {
  std::cerr << "---Table---" << std::endl;
  std::shared_ptr<Table> table(
      new Table("foo", {FieldType::INT, FieldType::FLOAT, FieldType::DOUBLE,
                        FieldType::STRING}));
  std::cerr << *table << std::endl;
}

void instance_test() {
  std::cerr << "--Instance--" << std::endl;
  std::shared_ptr<Instance> ins(new Instance);
  std::cerr << ins->create("foo",
                           {FieldType::INT, FieldType::INT, FieldType::FLOAT})
            << std::endl;
  std::cerr << ins->create("foo",
                           {FieldType::INT, FieldType::INT, FieldType::FLOAT})
            << std::endl;
  std::cerr << ins->create("bar", {FieldType::INT, FieldType::STRING})
            << std::endl;
  std::cerr << *ins << std::endl;
  std::cerr << ins->drop("foo") << std::endl;
  std::cerr << ins->drop("bar") << std::endl;
  std::cerr << ins->drop("bar") << std::endl;
  std::cerr << ins->test(1) << std::endl;
}

int main() {
  field_test();
  record_test();
  table_test();
  instance_test();
  std::cerr << "---Exit---" << std::endl;
  return 0;
}
