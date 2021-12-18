#include "int_field.h"

namespace Neru {

    // constructors
    IntField::IntField(int32_t data) : _data(data) {}
    IntField::IntField(const uint8_t *src) { this->load(src); }
    IntField::IntField(std::string data) { _data = std::stoi(data); }

    // io
    uint8_t *IntField::store(uint8_t *dst) const {
        const auto *src = (const uint8_t *) &_data;
        std::copy_n(src, this->size(), dst);
        return dst + this->size();
    }
    const uint8_t *IntField::load(const uint8_t *src) {
        auto *dst = (uint8_t *) &_data;
        std::copy_n(src, this->size(), dst);
        return src + this->size();
    }

    // info
    FieldType IntField::type() const { return FieldType::INT; }
    size_t IntField::size() const { return Field::size(this->type()); }
    std::string IntField::info() const { return std::to_string(this->size()) + "\tIntField: " + std::to_string(_data); }

    // operators
    IntField::operator int32_t() const { return _data; }
    std::ostream &operator<<(std::ostream &os, const IntField &field) { return os << field.info(); }
    bool IntField::operator==(const IntField &field) const { return _data == field._data; }
    bool IntField::operator!=(const IntField &field) const { return _data != field._data; }
    bool IntField::operator<(const IntField &field) const { return _data < field._data; }
    bool IntField::operator<=(const IntField &field) const { return _data <= field._data; }
    bool IntField::operator>(const IntField &field) const { return _data > field._data; }
    bool IntField::operator>=(const IntField &field) const { return _data >= field._data; }
    IntField IntField::operator+(const IntField &field) const { return IntField(_data + field._data); }
    IntField IntField::operator-(const IntField &field) const { return IntField(_data - field._data); }
    IntField IntField::operator*(const IntField &field) const { return IntField(_data * field._data); }
    IntField IntField::operator/(const IntField &field) const { return IntField(_data / field._data); }

}// namespace Neru
