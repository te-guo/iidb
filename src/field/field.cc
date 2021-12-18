#include "fields.h"

namespace Neru {

    // operators
    std::ostream &operator<<(std::ostream &os, const Field &field) { return os << field.info(); }
    bool Field::operator==(const Field &field) const {
        FieldType type = this->type();
        if (type != field.type())
            return false;
        switch (type) {
            case FieldType::INT:
                return static_cast<const IntField &>(*this) == static_cast<const IntField &>(field);
            case FieldType::DOUBLE:
                return static_cast<const DoubleField &>(*this) == static_cast<const DoubleField &>(field);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }
    bool Field::operator!=(const Field &field) const {
        FieldType type = this->type();
        if (type != field.type())
            return true;
        switch (type) {
            case FieldType::INT:
                return static_cast<const IntField &>(*this) != static_cast<const IntField &>(field);
            case FieldType::DOUBLE:
                return static_cast<const DoubleField &>(*this) != static_cast<const DoubleField &>(field);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }
    bool Field::operator<(const Field &field) const {
        FieldType type = this->type();
        if (type != field.type())
            throw std::runtime_error("Field: Type not match!");
        switch (type) {
            case FieldType::INT:
                return static_cast<const IntField &>(*this) < static_cast<const IntField &>(field);
            case FieldType::DOUBLE:
                return static_cast<const DoubleField &>(*this) < static_cast<const DoubleField &>(field);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }
    bool Field::operator<=(const Field &field) const {
        FieldType type = this->type();
        if (type != field.type())
            throw std::runtime_error("Field: Type not match!");
        switch (type) {
            case FieldType::INT:
                return static_cast<const IntField &>(*this) <= static_cast<const IntField &>(field);
            case FieldType::DOUBLE:
                return static_cast<const DoubleField &>(*this) <= static_cast<const DoubleField &>(field);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }
    bool Field::operator>(const Field &field) const {
        FieldType type = this->type();
        if (type != field.type())
            throw std::runtime_error("Field: Type not match!");
        switch (type) {
            case FieldType::INT:
                return static_cast<const IntField &>(*this) > static_cast<const IntField &>(field);
            case FieldType::DOUBLE:
                return static_cast<const DoubleField &>(*this) > static_cast<const DoubleField &>(field);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }
    bool Field::operator>=(const Field &field) const {
        FieldType type = this->type();
        if (type != field.type())
            throw std::runtime_error("Field: Type not match!");
        switch (type) {
            case FieldType::INT:
                return static_cast<const IntField &>(*this) >= static_cast<const IntField &>(field);
            case FieldType::DOUBLE:
                return static_cast<const DoubleField &>(*this) >= static_cast<const DoubleField &>(field);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }

    // tools
    size_t Field::size(FieldType type) {
        switch (type) {
            case FieldType::INT:
                return sizeof(int32_t);
            case FieldType::DOUBLE:
                return sizeof(double);
            default:
                throw std::runtime_error("FieldType: Type unknown!");
        }
    }

}// namespace Neru
