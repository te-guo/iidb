#include "record.h"

namespace Neru {
    
    // constructors
    Record::Record(std::vector<std::shared_ptr<Field>> fields) : _fields(fields) {}
    Record::Record(const uint8_t *src, Header head) { this->load(src, head); }
    Record::Record(std::vector<std::string> data, Header head) {
        if (data.size() != head.size())
            throw std::runtime_error("Record: Size not match!");
        for (size_t i = 0; i < head.size(); ++i)
            switch (head[i]) {
                case FieldType::INT:
                    _fields.push_back(std::make_shared<IntField>(data[i]));
                    break;
                case FieldType::DOUBLE:
                    _fields.push_back(std::make_shared<DoubleField>(data[i]));
                    break;
                default:
                    throw std::runtime_error("FieldType: Type unknown!");
            }
    }

    // io
    uint8_t *Record::store(uint8_t *dst) const {
        uint8_t *pos = dst;
        for (auto field : _fields) {
            switch (field->type()) {
                case FieldType::INT:
                    std::static_pointer_cast<IntField>(field)->store(pos);
                    break;
                case FieldType::DOUBLE:
                    std::static_pointer_cast<DoubleField>(field)->store(pos);
                    break;
                default:
                    throw std::runtime_error("FieldType: Type unknown!");
            }
            pos += field->size();
        }
        return pos;
    }
    const uint8_t *Record::load(const uint8_t *src, Header head) {
        _fields.clear();
        auto *pos = const_cast<uint8_t *>(src);
        for (auto type : head) {
            switch (type) {
                case FieldType::INT:
                    _fields.push_back(std::static_pointer_cast<Field>(std::make_shared<IntField>(pos)));
                    break;
                case FieldType::DOUBLE:
                    _fields.push_back(std::static_pointer_cast<Field>(std::make_shared<DoubleField>(pos)));
                    break;
                default:
                    throw std::runtime_error("FieldType: Type unknown!");
            }
            pos += Field::size(type);
        }
        return pos;
    }

    // info
    Header Record::type() const {
        Header head;
        for (auto field : _fields)
            head.push_back(field->type());
        return head;
    }
    size_t Record::size() const {
        size_t size = 0;
        for (auto field : _fields)
            size += field->size();
        return size;
    }
    size_t Record::length() const { return _fields.size(); }
    std::string Record::info() const {
        std::string _info = "Record: " + std::to_string(this->length()) + "\n";
        for (auto field : _fields)
            _info += field->info() + '\n';
        _info.pop_back();
        return _info;
    }

    // operators
    std::ostream &operator<<(std::ostream &os, const Record &record) { return os << record.info(); }
    bool Record::operator==(const Record &record) const {
        if (this->length() != record.length())
            return false;
        for (size_t i = 0; i < this->length(); ++i)
            if (*this->get(i) != *record.get(i))
                return false;
        return true;
    }
    bool Record::operator!=(const Record &record) const { return !(*this == record); }
    std::shared_ptr<Field> &Record::operator[](size_t idx) {
        if (idx >= this->length())
            throw std::runtime_error("Record: Index out of range!");
        return _fields[idx];
    }

    // tools
    bool Record::match(Header head) const {
        if (this->length() != head.size())
            return false;
        for (size_t i = 0; i < this->length(); ++i)
            if (this->get(i)->type() != head[i])
                return false;
        return true;
    }
    std::shared_ptr<Field> Record::get(size_t idx) const {
        if (idx >= this->length())
            throw std::runtime_error("Record: Index out of range!");
        return _fields[idx];
    }
    Record &Record::push(std::shared_ptr<Field> field) {
        _fields.push_back(field);
        return *this;
    }

}// namespace Neru
