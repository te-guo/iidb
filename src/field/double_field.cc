#include "double_field.h"

namespace Neru {

    // constructors
    DoubleField::DoubleField(double data) : _data(data) {}
    DoubleField::DoubleField(const uint8_t *src) { this->load(src); }
    DoubleField::DoubleField(std::string data) { _data = std::stod(data); }

    // io
    uint8_t *DoubleField::store(uint8_t *dst) const {
        const auto *src = (const uint8_t *) &_data;
        std::copy_n(src, this->size(), dst);
        return dst + this->size();
    }
    const uint8_t *DoubleField::load(const uint8_t *src) {
        auto *dst = (uint8_t *) &_data;
        std::copy_n(src, this->size(), dst);
        return src + this->size();
    }

    // info
    FieldType DoubleField::type() const { return FieldType::DOUBLE; }
    size_t DoubleField::size() const { return Field::size(this->type()); }
    std::string DoubleField::info() const { return std::to_string(this->size()) + "\tDoubleField: " + std::to_string(_data); }

    // operators
    DoubleField::operator double() const { return _data; }
    std::ostream &operator<<(std::ostream &os, const DoubleField &field) { return os << field.info(); }
    bool DoubleField::operator==(const DoubleField &field) const { return _data == field._data; }
    bool DoubleField::operator!=(const DoubleField &field) const { return _data != field._data; }
    bool DoubleField::operator<(const DoubleField &field) const { return _data < field._data; }
    bool DoubleField::operator<=(const DoubleField &field) const { return _data <= field._data; }
    bool DoubleField::operator>(const DoubleField &field) const { return _data > field._data; }
    bool DoubleField::operator>=(const DoubleField &field) const { return _data >= field._data; }

}// namespace Neru
