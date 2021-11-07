#pragma once

#include "bufmanager/bufmanager.h"
#include "record/record.h"

namespace Neru {

typedef std::pair<size_t, size_t> Entry;

void store_int(uint8_t* &dst, uint32_t data);
uint32_t load_int(uint8_t* &src);
uint32_t read_int(uint8_t* src);

class Table {
public:
  Table() = delete;

  Table(std::string name, Header head);

  Table(const uint8_t *src);

  Entry insert(std::shared_ptr<Record> record);

  Entry update(Entry dst, std::shared_ptr<Record> record);

  bool remove(Entry dst);

  std::vector<std::shared_ptr<Record>> select();

  Header head() const;

  size_t size() const;

  std::string info() const;

  friend std::ostream &operator<<(std::ostream &os, const Table &table);

private:
  std::string _name;

  Header _head;

  int fileID;

  const uint8_t *load(const uint8_t *src);

  uint8_t *store(uint8_t *dst) const;
};

} // namespace Neru
