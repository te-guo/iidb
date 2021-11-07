#include "field/string_field.h"
#include "record/record.h"
#include "table.h"
#include "bufmanager/bufmanager.h"

typedef unsigned int T;

namespace Neru {

void store_int(uint8_t* &dst, uint32_t data){
  std::copy_n((const uint8_t*)&data, sizeof(uint32_t), dst);
  dst += sizeof(uint32_t);
}
uint32_t load_int(uint8_t* &src){
  uint32_t data;
  std::copy_n(src, sizeof(uint32_t), &data);
  src += sizeof(uint32_t);
  return data;
}
uint32_t read_int(uint8_t* src){
  uint32_t data;
  std::copy_n(src, sizeof(uint32_t), &data);
  return data;
}

FileManager* fm = new FileManager();
BufPageManager<T> bpm(fm);

Table::Table(std::string name, Header head) : _name(name), _head(head) {
  fm->createFile(_name.c_str());
	fm->openFile(_name.c_str(), fileID);
}

Table::Table(const uint8_t *src) {
  this->load(src);
  fm->createFile(_name.c_str());
	fm->openFile(_name.c_str(), fileID);
}

uint8_t *Table::store(uint8_t *dst) const {
  // TODO
  uint8_t* pos = dst;
  store_int(pos, 0);
  store_string(pos, _name);
  store_int(pos, _head.size());
  for (const auto &type : _head)
    switch(type){
      case FieldType::INT:
        *(pos++) = 0;
        break;
      case FieldType::DOUBLE:
        *(pos++) = 1;
        break;
      case FieldType::FLOAT:
        *(pos++) = 2;
        break;
      case FieldType::STRING:
        *(pos++) = 3;
        break;
      default:
        __builtin_unreachable();
    }
  int32_t page_n = pos - dst + PAGE_SIZE - 1 >> PAGE_SIZE_IDX;
  store_int(dst, page_n);
  return dst + (page_n << PAGE_SIZE_IDX);
}

const uint8_t *Table::load(const uint8_t *src) {
  // TODO
  uint8_t* pos = (uint8_t*)src;
  int32_t page_n = load_int(pos);
  _name = load_string(pos);
  int32_t header_size = load_int(pos);
  _head.resize(header_size);
  for(int i = 0; i < header_size; i++)
    switch(pos[i]){
      case 0:
        _head[i] = FieldType::INT;
        break;
      case 1:
        _head[i] = FieldType::DOUBLE;
        break;
      case 2:
        _head[i] = FieldType::FLOAT;
        break;
      case 3:
        _head[i] = FieldType::STRING;
        break;
      default:
        __builtin_unreachable();
    }

  return src + (page_n << PAGE_SIZE_IDX);
}

Header Table::head() const { return _head; }

size_t Table::size() const { return _head.size(); }

std::string Table::info() const {
  std::string info = "Table:\n\t" + _name + "\n\tFields:";
  for (const auto &head : _head)
    switch (head) {
    case FieldType::INT:
      info += "\n\t\tIntField";
      break;
    case FieldType::FLOAT:
      info += "\n\t\tFloatField";
      break;
    case FieldType::DOUBLE:
      info += "\n\t\tDoubleField";
      break;
    case FieldType::STRING:
      info += "\n\t\tStringField";
    default:
      __builtin_unreachable();
    }
  return info;
}

std::ostream &operator<<(std::ostream &os, const Table &table) {
  return os << table.info();
}

Entry Table::insert(std::shared_ptr<Record> record) {
  // TODO
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, 0, buf_index);
  int32_t pageID = read_int(buf);
  bpm.access(buf_index);

  for(int record_size = record->size(); ; pageID++){
    buf = (uint8_t*)bpm.getPage(fileID, pageID, buf_index);
    uint8_t* pos = buf + 1;
    int slotID = load_int(pos);
    int available = load_int(pos);
    if(slotID == 0)
      available = PAGE_SIZE - 2 * sizeof(uint32_t) - 1;
    if(available >= record_size){
      for(int i = 0; i < slotID; i++)
        pos += Record(pos, _head).size();
      record->store(pos);
      *(buf++) = 1;
      store_int(buf, slotID + 1);
      store_int(buf, available - record_size);
      bpm.markDirty(buf_index);
      return {pageID, slotID};
    }
    else
      bpm.access(buf_index);
  }
}

bool Table::update(Entry dst, std::shared_ptr<Record> record) {
  // TODO
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, dst.first, buf_index);
  uint8_t* pos = buf + 1;
  int slotID = load_int(pos);
  if(dst.second >= slotID){
      bpm.access(buf_index);
      return false;
  }
  int available = load_int(pos);
  if(slotID == 0)
    available = PAGE_SIZE - 2 * sizeof(uint32_t);
  int record_size = record->size();
  
  uint8_t* page = new uint8_t[PAGE_SIZE], *ppos = page + 2 * sizeof(uint32_t) + 1;
  for(int i = 0; i < dst.second; i++){
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    ppos = tmp.store(ppos);
  }
  {
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    if(tmp.size() + available < record_size){
      bpm.access(buf_index);
      return false;
    }
    else
      available += tmp.size() - record_size;
    ppos = record->store(ppos);
  }
  for(int i = dst.second + 1; i < slotID; i++){
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    ppos = tmp.store(ppos);
  }
  *(ppos++) = 1;
  store_int(ppos, slotID);
  store_int(ppos, available);
  bpm.markDirty(buf_index);
  return true;
}

bool Table::remove(Entry dst) {
  // TODO
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, dst.first, buf_index);
  uint8_t* pos = buf;
  int slotID = load_int(pos);
  if(dst.second >= slotID){
      bpm.access(buf_index);
      return false;
  }
  int available = load_int(pos);
  if(slotID == 0)
    available = PAGE_SIZE - 2 * sizeof(uint32_t);
  
  uint8_t* page = new uint8_t[PAGE_SIZE], *ppos = page + 2 * sizeof(uint32_t);
  for(int i = 0; i < dst.second; i++){
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    ppos = tmp.store(ppos);
  }
  {
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    available += tmp.size();
  }
  for(int i = dst.second + 1; i < slotID; i++){
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    ppos = tmp.store(ppos);
  }
  store_int(buf, slotID - 1);
  store_int(buf, available);
  bpm.markDirty(buf_index);
  return true;
}

std::vector<std::shared_ptr<Record>> Table::select() {
  // TODO
  std::vector<std::shared_ptr<Record>> selected;
  return {};
}

} // namespace Neru
