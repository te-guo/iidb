#include <filesystem>
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
void write_int(uint8_t* dst, uint32_t data){
  std::copy_n((const uint8_t*)&data, sizeof(uint32_t), dst);
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
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, 0, buf_index);
  this->store(buf);
  bpm.markDirty(buf_index);
}

Table::Table(const uint8_t *src) {
  this->load(src);
  fm->createFile(_name.c_str());
	fm->openFile(_name.c_str(), fileID);
}

Table::Table(const std::filesystem::path path){
  int fileID;
  fm->createFile(path.c_str());
  fm->openFile(path.c_str(), fileID);
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, 0, buf_index);
  this->load(buf);
  bpm.access(buf_index);
}

uint8_t *Table::store(uint8_t *dst) const {
  // TODO
  uint8_t* pos = dst;
  store_string(pos, _name);
  store_int(pos, _head.size());
  for (const auto &type : _head)
    switch(type){
      case FieldType::INT:
        *(pos++) = 0;
        break;
      case FieldType::FLOAT:
        *(pos++) = 1;
        break;
      case FieldType::DOUBLE:
        *(pos++) = 2;
        break;
      case FieldType::STRING:
        *(pos++) = 3;
        break;
      default:
        __builtin_unreachable();
    }
  return dst + PAGE_SIZE;
}

const uint8_t *Table::load(const uint8_t *src) {
  // TODO
  uint8_t* pos = (uint8_t*)src;
  _name = load_string(pos);
  int32_t header_size = load_int(pos);
  _head.resize(header_size);
  for(int i = 0; i < header_size; i++)
    switch(pos[i]){
      case 0:
        _head[i] = FieldType::INT;
        break;
      case 1:
        _head[i] = FieldType::FLOAT;
        break;
      case 2:
        _head[i] = FieldType::DOUBLE;
        break;
      case 3:
        _head[i] = FieldType::STRING;
        break;
      default:
        __builtin_unreachable();
    }
  return src + PAGE_SIZE;
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
  uint8_t* buf;
  int32_t pageID = 1;

  for(int record_size = record->size(); ; pageID++){
    buf = (uint8_t*)bpm.getPage(fileID, pageID, buf_index);
    uint8_t* pos = buf + 1;
    int slotID = load_int(pos);
    int available = load_int(pos);
    if(slotID == 0)
      available = PAGE_SIZE - 2 * sizeof(uint32_t) - 1;
    if(available >= record_size + 1){
      for(int i = 0; i < slotID; i++)
        if(*(pos++) == 1)
          pos += Record(pos, _head).size();
      *(pos++) = 1;
      record->store(pos);
      *buf = 1;
      write_int(buf + 1, slotID + 1);
      write_int(buf + 5, available - record_size - 1);
      bpm.markDirty(buf_index);
      return {pageID, slotID};
    }
    else
      bpm.access(buf_index);
  }
}

Entry Table::update(Entry dst, std::shared_ptr<Record> record) {
  // TODO
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, dst.first, buf_index);
  uint8_t* pos = buf + 1;
  int slotID = load_int(pos);
  if(dst.second >= slotID){
      bpm.access(buf_index);
      return insert(record);
  }
  int available = load_int(pos);
  if(slotID == 0)
    available = PAGE_SIZE - 2 * sizeof(uint32_t) - 1;
  bool exception = false;
  int record_size = record->size();
  
  uint8_t* page = new uint8_t[PAGE_SIZE], *ppos = page + 2 * sizeof(uint32_t) + 1;
  for(int i = 0; i < dst.second; i++)
    if(*(pos++) == 1){
      Record tmp;
      pos = (uint8_t*)tmp.load(pos, _head);
      *(ppos++) = 1;
      ppos = tmp.store(ppos);
    }
    else
      *(ppos++) = 0;
  if(*(pos++) == 1){
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    if(tmp.size() + available < record_size){
      available += tmp.size();
      *(ppos++) = 0;
      exception = true;
    }
    else{
      available += tmp.size() - record_size;
      *(ppos++) = 1;
      ppos = record->store(ppos);
    }
  }
  else{
    *(ppos++) = 0;
    exception = true;
  }
  for(int i = dst.second + 1; i < slotID; i++)
    if(*(pos++) == 1){
      Record tmp;
      pos = (uint8_t*)tmp.load(pos, _head);
      *(ppos++) = 1;
      ppos = tmp.store(ppos);
    }
    else
      *(ppos++) = 0;
  *page = 1;
  write_int(page + 1, slotID);
  write_int(page + 5, available);
  copy_n(page, PAGE_SIZE, buf);
  delete page;
  bpm.markDirty(buf_index);
  if(exception)
    return insert(record);
  else
    return dst;
}

bool Table::remove(Entry dst) {
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
    available = PAGE_SIZE - 2 * sizeof(uint32_t) - 1;
  bool exception = false;
  
  uint8_t* page = new uint8_t[PAGE_SIZE], *ppos = page + 2 * sizeof(uint32_t) + 1;
  for(int i = 0; i < dst.second; i++)
    if(*(pos++) == 1){
      Record tmp;
      pos = (uint8_t*)tmp.load(pos, _head);
      *(ppos++) = 1;
      ppos = tmp.store(ppos);
    }
    else
      *(ppos++) = 0;
  if(*(pos++) == 1){
    Record tmp;
    pos = (uint8_t*)tmp.load(pos, _head);
    available += tmp.size();
    *(ppos++) = 0;
  }
  else{
    *(ppos++) = 0;
    exception = true;
  }
  for(int i = dst.second + 1; i < slotID; i++)
    if(*(pos++) == 1){
      Record tmp;
      pos = (uint8_t*)tmp.load(pos, _head);
      *(ppos++) = 1;
      ppos = tmp.store(ppos);
    }
    else
      *(ppos++) = 0;
  *page = 1;
  write_int(page + 1, slotID);
  write_int(page + 5, available);
  copy_n(page, PAGE_SIZE, buf);
  delete page;
  bpm.markDirty(buf_index);
  return !exception;
}

std::vector<std::pair<Entry, std::shared_ptr<Record>>> Table::select() {
  // TODO
  std::vector<std::shared_ptr<Record>> selected;
  int pageID = 1;
  while(true){
    int buf_index;
    uint8_t* buf = (uint8_t*)bpm.getPage(fileID, pageID, buf_index), *pos = buf;
    if(*(pos++) != 1)
      break;
    int slotID = load_int(pos);
    pos += 4;
    for(int i = 0; i < slotID; i++)
      if(*(pos++) == 1){
        selected.push_back(std::shared_ptr<Record>(new Record(pos, _head)));
        pos += selected.back()->size();
      }
    bpm.access(buf_index);
  }
  return selected;
}

} // namespace Neru
