#include "string_field.h"

namespace Neru {

// TODO

void store_string(uint8_t* &dst, std::string data){
  std::copy_n((const uint8_t*)data.c_str(), data.length(), dst);
  dst[data.length()] = '\0';
  dst += data.length() + 1;
}
std::string load_string(uint8_t* &src){
  std::string data;
  while(*src != '\0') data += (char)*src, src++;
  src++;
  return data;
}
std::string read_string(uint8_t* src){
  std::string data;
  while(*src != '\0') data += (char)*src, src++;
  return data;
}

StringField::StringField(std::string data) : _data(data) {}

StringField::StringField(const uint8_t *src) { this->load(src); }

uint8_t *StringField::store(uint8_t *dst) const {
  store_string(dst, _data);
  return dst;
}

const uint8_t *StringField::load(const uint8_t *src) {
  uint8_t* pos = (uint8_t*)src;
  _data = load_string(pos);
  return pos;
}

FieldType StringField::type() const { return FieldType::STRING; }

size_t StringField::size() const { return _data.length() + 1; }

std::string StringField::info() const {
  return std::to_string(this->size()) +
         "\tStringField: " + _data;
}

bool StringField::operator==(const StringField &field) const {
  return _data == field._data;
}

bool StringField::operator!=(const StringField &field) const {
  return _data != field._data;
}

bool StringField::operator<(const StringField &field) const {
  return _data < field._data;
}

bool StringField::operator<=(const StringField &field) const {
  return _data <= field._data;
}

bool StringField::operator>(const StringField &field) const {
  return _data > field._data;
}

bool StringField::operator>=(const StringField &field) const {
  return _data >= field._data;
}

std::ostream &operator<<(std::ostream &os, const StringField &field) {
  return os << field.info();
}


};
