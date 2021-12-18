#ifndef NERU_FILE_H
#define NERU_FILE_H

#include "page/page.h"

#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

namespace Neru {

    constexpr std::string_view FILE_PREFIX = "data";
    constexpr size_t BLOCK_SIZE = 1024;

    enum class FileType { ANY,
                          DATA,
                          INDEX };

    class File {
    public:
        // constructors
        File() = default;
        explicit File(std::string name, bool create = false);
        ~File();

        // info
        static FileType type();
        size_t size() const;
        std::string info() const;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const File &file);
        std::shared_ptr<Page> &operator[](size_t idx);

        // tools
        static bool init(std::filesystem::path path = FILE_PREFIX);
        bool save() const;
        std::shared_ptr<Page> allocate();
        File &push(std::shared_ptr<Page> page);
        File &pop();
        std::shared_ptr<Page> get(size_t idx) const;

    protected:
        std::string _name;
        std::string _path;
        std::bitset<BLOCK_SIZE> _flag;
        std::vector<std::shared_ptr<Page>> _pages;
    };

}// namespace Neru

#endif//NERU_FILE_H
