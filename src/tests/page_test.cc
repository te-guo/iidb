#include "page/page.h"
#include "utils/utils.h"

using namespace Neru;

bool page_test() {
    std::cerr << "---Page---" << std::endl;
    Page page1;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[640]);
    std::copy_n("Hello, world!\0", 14, pool.get());
    std::cerr << pool.get() << std::endl;
    page1.write(pool.get(), 14, 0);
    page1.read(pool.get() + 13, 14, 0);
    std::cerr << pool.get() << std::endl;
    page1.write(pool.get(), 27, 13);
    page1.read(pool.get(), 40, 0);
    std::cerr << pool.get() << std::endl;
    return true;
}
