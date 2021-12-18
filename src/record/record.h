#ifndef NERU_RECORD_H
#define NERU_RECORD_H

#include "field/fields.h"
#include <vector>

namespace Neru {

    typedef std::vector<FieldType> Header;

    class Record {
    public:
        // constructors
        Record() = default;
        explicit Record(std::vector<std::shared_ptr<Field>> fields);
        Record(const uint8_t *src, Header head);
        Record(std::vector<std::string> data, Header head);

        // io
        uint8_t *store(uint8_t *dst) const;
        const uint8_t *load(const uint8_t *src, Header head);

        // info
        Header type() const;
        size_t size() const;
        size_t length() const;
        std::string info() const;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const Record &record);
        bool operator==(const Record &record) const;
        bool operator!=(const Record &record) const;
        std::shared_ptr<Field> &operator[](size_t idx);

        // tools
        bool match(Header head) const;
        std::shared_ptr<Field const> get(size_t idx) const;
        Record &push(std::shared_ptr<Field> field);

    private:
        std::vector<std::shared_ptr<Field>> _fields;
    };

}// namespace Neru


#endif//NERU_RECORD_H
