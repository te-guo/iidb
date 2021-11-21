#ifndef NERU_FIELD_H
#define NERU_FIELD_H

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>

namespace Neru {

    enum class FieldType { INT,
                           FLOAT,
                           DOUBLE,
                           STRING };

    class Field {
    public:
        // io
        virtual uint8_t *store(uint8_t *dst) const = 0;
        virtual const uint8_t *load(const uint8_t *src) = 0;

        // info
        virtual FieldType type() const = 0;
        virtual size_t size() const = 0;
        virtual std::string info() const = 0;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const Field &field);
        bool operator==(const Field &field);
        bool operator!=(const Field &field);
        bool operator<(const Field &field);
        bool operator<=(const Field &field);
        bool operator>(const Field &field);
        bool operator>=(const Field &field);

        // tools
        size_t static size(FieldType type);
    };

}// namespace Neru

#endif//NERU_FIELD_H
