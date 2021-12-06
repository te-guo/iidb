#ifndef NERU_PAGE_H
#define NERU_PAGE_H

#include <algorithm>
#include <fstream>
#include <memory>

namespace Neru {

    constexpr size_t PAGE_SIZE = 8192;

    class Page {
    public:
        // constructors
        Page();

        // tools
        Page &read(uint8_t *dst, size_t size, size_t offset);
        Page &write(const uint8_t *src, size_t size, size_t offset);
        char *raw() const;
        size_t read_int(size_t offset);
        void write_int(size_t src, size_t offset);

    protected:
        std::shared_ptr<uint8_t[]> _data;
    };

}// namespace Neru


#endif//NERU_PAGE_H
