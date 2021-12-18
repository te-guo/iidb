#include "tools.h"

namespace Neru {

    namespace utils {

        // FieldType
        FieldType from(std::string data) {
            if (data == "INT")
                return FieldType::INT;
            else if (data == "DOUBLE")
                return FieldType::DOUBLE;
            else
                throw std::runtime_error("FieldType: Type unknown!");
        }
        std::string info(const FieldType &type) {
            switch (type) {
                case FieldType::INT:
                    return std::to_string(Field::size(type)) + " INT";
                case FieldType::DOUBLE:
                    return std::to_string(Field::size(type)) + " DOUBLE";
                default:
                    throw std::runtime_error("FieldType: Type unknown!");
            }
        }

        // Header
        Header from(std::vector<std::string> data) {
            std::vector<FieldType> _head;
            for (auto type : data)
                _head.push_back(from(type));
            return _head;
        }
        size_t size(const Header &head) {
            size_t _size = 0;
            for (auto type : head)
                _size += Field::size(type);
            return _size;
        }
        std::string info(const Header &head) {
            std::string _info;
            for (auto type : head)
                _info += info(type) + ' ';
            _info.pop_back();
            return _info;
        }

    }// namespace utils

    // operators
    std::ostream &operator<<(std::ostream &os, const FieldType &type) { return os << utils::info(type); }
    std::ostream &operator<<(std::ostream &os, const Header &head) { return os << utils::info(head); }

}// namespace Neru
