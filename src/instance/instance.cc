#include <iostream>
#include <climits>
#include <cassert>
#include "instance.h"
#include "bufmanager/bufmanager.h"

typedef unsigned int T;

namespace Neru {

extern FileManager* fm;
extern BufPageManager<T> bpm;

Instance::Instance(std::string path) : _dir(path) {
  std::cout << _dir << std::endl;
  if (!std::filesystem::exists(_dir))
    std::filesystem::create_directories(_dir);
  for (const auto &dir_entry : std::filesystem::directory_iterator(_dir)){
    // TODO
    std::cout << dir_entry.path() << std::endl;
    _tables[dir_entry.path().filename()] = std::shared_ptr<Table>(new Table(dir_entry.path()));
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
  for (size_t i = 0; i < n; ++i)
    entries.push_back(_tables["foo"]->insert(records[i]));
  for (size_t i = 0; i < m; ++i) {
    size_t idx = rand() % n;
    records[idx] = std::make_shared<Record>();
    for (size_t j = 0; j < head1.size(); ++j) {
      switch (head1[j]) {
      case FieldType::INT:
        records[idx]->append(std::make_shared<IntField>(rand()));
        break;
      case FieldType::FLOAT:
        records[idx]->append(
            std::make_shared<FloatField>(rand() / (INT_MAX + 1.0)));
        break;
      case FieldType::DOUBLE:
        records[idx]->append(
            std::make_shared<DoubleField>(rand() / (INT_MAX + 1.0)));
        break;
      default:
        __builtin_unreachable();
      }
    }
    entries[idx] = _tables["foo"]->update(entries[idx], records[idx]);
  }
  std::vector<std::shared_ptr<Record>> results = _tables["foo"]->select();
  assert(results.size() == n);
  for (size_t i = 0; i < n; ++i)
    assert(*records[i] == *results[i]);
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
