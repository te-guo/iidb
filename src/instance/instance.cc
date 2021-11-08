#include <cassert>
#include <iostream>
#include "instance.h"

namespace Neru {

Instance::Instance(std::string path) : _dir(path) {
  std::cout << _dir << std::endl;
  if (!std::filesystem::exists(_dir))
    std::filesystem::create_directories(_dir);
  for (const auto &dir_entry : std::filesystem::directory_iterator(_dir)){
    // TODO
    std::cout << dir_entry.path() << std::endl;
  }
}

bool Instance::create(std::string tbName, Header header) {
  if (std::filesystem::exists(_dir / tbName))
    return false;
  std::filesystem::create_directories(_dir / tbName);
  // TODO
  _tables[tbName] = std::shared_ptr<Table>(new Table((_dir / tbName).string(), header));
  return true;
}

bool Instance::drop(std::string tbName) {
  if (!std::filesystem::exists(_dir / tbName))
    return false;
  std::filesystem::remove_all(_dir / tbName);
  _tables.erase(tbName);
  return true;
}

std::string Instance::info() const {
  std::string info = "Instance:";
  for (const auto &kv : _tables)
    info += "\n\t" + kv.first;
  return info;
}

std::ostream &operator<<(std::ostream &os, const Instance &instance) {
  return os << instance.info();
}

bool Instance::test(size_t seed) {
  constexpr size_t n = 1e5;
  constexpr size_t m = 1e4;
  srand(seed);
  Header head1 = {FieldType::INT, FieldType::FLOAT, FieldType::DOUBLE};
  Header head2 = {FieldType::INT, FieldType::STRING};
  assert(create("foo", head1));
  assert(create("bar", head2));
  // assert(_tables["foo"]);
  // assert(_tables["bar"]);
  std::vector<std::shared_ptr<Record>> records;
  for (size_t i = 0; i < n; ++i) {
    records.push_back(std::make_shared<Record>());
    for (size_t j = 0; j < head1.size(); ++j) {
      switch (head1[j]) {
      case FieldType::INT:
        records[i]->append(std::make_shared<IntField>(rand()));
        break;
      case FieldType::FLOAT:
        records[i]->append(
            std::make_shared<FloatField>(rand() / (INT_MAX + 1.0)));
        break;
      case FieldType::DOUBLE:
        records[i]->append(
            std::make_shared<DoubleField>(rand() / (INT_MAX + 1.0)));
        break;
      default:
        __builtin_unreachable();
      }
    }
  }
  std::cerr << *records[0] << std::endl;
  std::vector<Entry> entries;
  std::map<Entry, std::shared_ptr<Record>> entry_map;
  for (size_t i = 0; i < n; ++i) {
    Entry entry = _tables["foo"]->insert(records[i]);
    entries.push_back(entry);
    entry_map[entry] = records[i];
  }
  for (auto kv : entry_map)
    std::cerr << *kv.second << std::endl;

  for (size_t i = 0; i < m; ++i) {
    size_t idx = rand() % n;
    std::shared_ptr<Record> record(std::make_shared<Record>());
    for (size_t j = 0; j < head1.size(); ++j) {
      switch (head1[j]) {
      case FieldType::INT:
        record->append(std::make_shared<IntField>(rand()));
        break;
      case FieldType::FLOAT:
        record->append(std::make_shared<FloatField>(rand() / (INT_MAX + 1.0)));
        break;
      case FieldType::DOUBLE:
        record->append(std::make_shared<DoubleField>(rand() / (INT_MAX + 1.0)));
        break;
      default:
        __builtin_unreachable();
      }
    }
    Entry entry = _tables["foo"]->update(entries[idx], record);
    entry_map.erase(entries[idx]);
    entries[idx] = entry;
    entry_map[entry] = record;
  }
  std::vector<std::pair<Entry, std::shared_ptr<Record>>> results =
      _tables["foo"]->select();
  assert(results.size() == n);
  for (size_t i = 0; i < n; ++i) {
    assert(entry_map.count(results[i].first) == 1);
    assert(*results[i].second == *entry_map[results[i].first]);
  }
  std::cerr << "Test Foo passed!" << std::endl;
  // for (size_t i = 0; i < n; ++i) {
  //   records.push_back(std::make_shared<Record>());
  //   for (size_t j = 0; j < head2.size(); ++j) {
  //     switch (head2[j]) {
  //     case FieldType::INT:
  //       records[i]->append(std::make_shared<IntField>(rand()));
  //       break;
  //     case FieldType::STRING:
  //       records[i]->append(std::make_shared<StringField>(
  //           std::string(rand() % 10, 'a' + rand() % 26)));
  //       break;
  //     default:
  //       __builtin_unreachable();
  //     }
  //   }
  // }
  assert(drop("foo"));
  assert(drop("bar"));
  return true;
}

} // namespace Neru
