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
  std::copy_n(src, sizeof(uint32_t), (uint8_t*)&data);
  src += sizeof(uint32_t);
  return data;
}
void write_int(uint8_t* dst, uint32_t data){
  std::copy_n((const uint8_t*)&data, sizeof(uint32_t), dst);
}
uint32_t read_int(uint8_t* src){
  uint32_t data;
  std::copy_n(src, sizeof(uint32_t), (uint8_t*)&data);
  return data;
}

FileManager* fm = new FileManager();
BufPageManager<T> bpm(fm);

Table::Table(std::string name, Header head) : _name(name), _head(head) { this->fileID = -1; }

Table::Table(const uint8_t *src) { this->load(src); this->fileID = -1; }

Table::Table(const std::filesystem::path path){
  fm->createFile(path.c_str());
  fm->openFile(path.c_str(), this->fileID);
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(this->fileID, 0, buf_index);
  this->load(buf);
  bpm.access(buf_index);
}

void Table::document(){
  fm->createFile(_name.c_str());
	fm->openFile(_name.c_str(), fileID);
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, 0, buf_index);
  this->store(buf);
  bpm.markDirty(buf_index);
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
  return pos;
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
  return pos + header_size;
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

int father_page(int pageID){
  return (pageID - 1 >> 11 << 11) + 1;
}
int page_index(int pageID){
  return pageID - father_page(pageID);
}

Entry Table::insert(std::shared_ptr<Record> record) {
  // TODO
  if(fileID == -1) this->document();
  int buf_index;
  uint8_t* buf;
  int record_size = record->size();
  for(int32_t pageID = 1; ; pageID += 2048){
    buf = (uint8_t*)bpm.getPage(fileID, pageID, buf_index);
    if(read_int(buf) == 0){
      write_int(buf, 1);
      for(int i = 4; i < PAGE_SIZE; i += 4)
        write_int(buf + i, PAGE_SIZE - 5);
      bpm.markDirty(buf_index);
    }
    for(int i = 4, available; i < PAGE_SIZE; i += 4)
      if((available = read_int(buf + i)) >= record_size + 1){
        bpm.access(buf_index);
        pageID += i >> 2;
        buf = (uint8_t*)bpm.getPage(fileID, pageID, buf_index);
        uint8_t* pos = buf + 1;
        int slotID = load_int(pos);
        pos = buf + PAGE_SIZE - available;
        *(pos++) = 1;
        record->store(pos);
        *buf = 1;
        write_int(buf + 1, slotID + 1);
        bpm.markDirty(buf_index);
        buf = (uint8_t*)bpm.getPage(fileID, father_page(pageID), buf_index);
        write_int(buf + i, available - record_size - 1);
        bpm.markDirty(buf_index);
        return {pageID, slotID};
      }
    bpm.access(buf_index);
  }
}

Entry Table::update(Entry dst, std::shared_ptr<Record> record) {
  // TODO
  if(fileID == -1) this->document();
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, father_page(dst.first), buf_index);
  int available = read_int(buf + (page_index(dst.first) << 2));
  bpm.access(father_page(dst.first));

  buf = (uint8_t*)bpm.getPage(fileID, dst.first, buf_index);
  uint8_t* pos = buf + 1;
  int slotID = load_int(pos);
  if(dst.second >= slotID){
      bpm.access(buf_index);
      return insert(record);
  }
  bool exception = false;
  int record_size = record->size();
  
  uint8_t* page = new uint8_t[PAGE_SIZE], *ppos = page + 5;
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
  copy_n(page, PAGE_SIZE, buf);
  delete page;
  bpm.markDirty(buf_index);
  buf = (uint8_t*)bpm.getPage(fileID, father_page(dst.first), buf_index);
  write_int(buf + (page_index(dst.first) << 2), available);
  bpm.markDirty(father_page(dst.first));
  if(exception)
    return insert(record);
  else
    return dst;
}

bool Table::remove(Entry dst) {
  // TODO
  if(fileID == -1) this->document();
  int buf_index;
  uint8_t* buf = (uint8_t*)bpm.getPage(fileID, father_page(dst.first), buf_index);
  int available = read_int(buf + (page_index(dst.first) << 2));
  bpm.access(father_page(dst.first));

  buf = (uint8_t*)bpm.getPage(fileID, dst.first, buf_index);
  uint8_t* pos = buf + 1;
  int slotID = load_int(pos);
  if(dst.second >= slotID){
      bpm.access(buf_index);
      return false;
  }
  bool exception = false;
  
  uint8_t* page = new uint8_t[PAGE_SIZE], *ppos = page + 5;
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
  copy_n(page, PAGE_SIZE, buf);
  delete page;
  bpm.markDirty(buf_index);
  buf = (uint8_t*)bpm.getPage(fileID, father_page(dst.first), buf_index);
  write_int(buf + (page_index(dst.first) << 2), available);
  bpm.markDirty(father_page(dst.first));
  return !exception;
}

std::vector<std::pair<Entry, std::shared_ptr<Record>>> Table::select() {
  // TODO
  if(fileID == -1) this->document();
  std::vector<std::pair<Entry, std::shared_ptr<Record>>> selected;
  for(int pageID = 1; ; pageID++){
    if(pageID == father_page(pageID))
      continue;
    int buf_index;
    uint8_t* buf = (uint8_t*)bpm.getPage(fileID, pageID, buf_index), *pos = buf;
    if(*(pos++) != 1)
      break;
    int slotID = load_int(pos);
    for(int i = 0; i < slotID; i++)
      if(*(pos++) == 1){
        selected.push_back(make_pair((Entry){pageID, i}, std::shared_ptr<Record>(new Record(pos, _head))));
        pos += selected.back().second->size();
      }
    bpm.access(buf_index);
  }
  return selected;
}

} // namespace Neru
