#ifndef NERU_INT_FIELD_H
#define NERU_INT_FIELD_H

#include "field.h"

namespace Neru {

    class IntField : public Field {
    public:
        // constructors
        IntField() = default;
        explicit IntField(int32_t data);
        explicit IntField(const uint8_t *src);
        explicit IntField(std::string data);

        // io
        uint8_t *store(uint8_t *dst) const override;
        const uint8_t *load(const uint8_t *src) override;

        // info
        FieldType type() const override;
        size_t size() const override;
        std::string info() const override;
        size_t hash() const override;

        // operators
        operator int32_t() const;
        friend std::ostream &operator<<(std::ostream &os, const IntField &field);
        bool operator==(const IntField &field) const;
        bool operator!=(const IntField &field) const;
        bool operator<(const IntField &field) const;
        bool operator<=(const IntField &field) const;
        bool operator>(const IntField &field) const;
        bool operator>=(const IntField &field) const;
        IntField operator+(const IntField &field) const;
        IntField operator-(const IntField &field) const;
        IntField operator*(const IntField &field) const;
        IntField operator/(const IntField &field) const;

    private:
        int32_t _data{};
    };

}// namespace Neru

#endif//NERU_INT_FIELD_H
