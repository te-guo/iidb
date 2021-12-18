#ifndef NERU_DOUBLE_FIELD_H
#define NERU_DOUBLE_FIELD_H

#include "field.h"

namespace Neru {

    class DoubleField : public Field {
    public:
        // constructors
        DoubleField() = default;
        explicit DoubleField(double data);
        explicit DoubleField(const uint8_t *src);
        explicit DoubleField(std::string data);

        // io
        uint8_t *store(uint8_t *dst) const override;
        const uint8_t *load(const uint8_t *src) override;

        // info
        FieldType type() const override;
        size_t size() const override;
        std::string info() const override;

        // operators
        operator double() const;
        friend std::ostream &operator<<(std::ostream &os, const DoubleField &field);
        bool operator==(const DoubleField &field) const;
        bool operator!=(const DoubleField &field) const;
        bool operator<(const DoubleField &field) const;
        bool operator<=(const DoubleField &field) const;
        bool operator>(const DoubleField &field) const;
        bool operator>=(const DoubleField &field) const;

    private:
        double _data{};
    };

}// namespace Neru

#endif//NERU_DOUBLE_FIELD_H
