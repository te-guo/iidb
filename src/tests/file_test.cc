#include "file/file.h"
#include "utils/utils.h"

using namespace Neru;

bool file_test() {
    std::cerr << "---File---" << std::endl;
    std::cerr << File::init() << std::endl;
    {
        File file1("foo", true);
        auto page1 = file1.allocate();
        std::shared_ptr<uint8_t[]> pool(new uint8_t[12800]);
        std::copy_n("Hello, world!\0", 14, pool.get());
        page1->write(pool.get(), 14, 0);
        auto page2 = std::make_shared<Page>();
        page2->write(pool.get(), 14, 14);
        file1.push(page2);
        std::cerr << file1 << std::endl;
        //        file1.save();
    }
    File file2("foo");
    std::cerr << file2 << std::endl;
    std::cerr << file2[0]->raw() << std::endl;
    std::cerr << file2[1]->raw() + 14 << std::endl;
    return true;
}
