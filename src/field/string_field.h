#pragma once

#include "field.h"

namespace Neru {

typedef uint16_t str_length;

void store_string(uint8_t* &dst, std::string data);
std::string load_string(uint8_t* &src);
std::string read_string(uint8_t* src);

class StringField : public Field {
  // TODO
public:
  StringField() = default;

  explicit StringField(std::string data);

  explicit StringField(const uint8_t *src);

  uint8_t *store(uint8_t *dst) const override;

  const uint8_t *load(const uint8_t *src) override;

  FieldType type() const override;

  size_t size() const override;

  std::string info() const override;

  bool operator==(const StringField &field) const;

  bool operator!=(const StringField &field) const;

  bool operator<(const StringField &field) const;

  bool operator<=(const StringField &field) const;

  bool operator>(const StringField &field) const;

  bool operator>=(const StringField &field) const;

  friend std::ostream &operator<<(std::ostream &os, const StringField &field);

private:
  std::string _data{};
};

} // namespace Neru
