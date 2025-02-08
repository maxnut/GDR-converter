#include "gdr_convert.hpp"

#include <gdr/gdr.hpp>
#include <iostream>

void assert_(bool value, const char* message) {
    if (value) return;
    std::cerr << "Assert: '" << message << "' failed" <<  std::endl;
    std::exit(1);
}

#undef assert
#define assert(expr) assert_(expr, #expr)

struct MyReplay : gdr::Replay<MyReplay, gdr::Input<>> {
    MyReplay() : Replay("TestBot", 1) {}
};

int main() {
    std::ifstream file("./test.gdr", std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto res = gdr::convert<MyReplay, gdr::Input<>>(data);

    if(res.isErr()) {
        std::cout << res.unwrapErr() << std::endl;
        return 1;
    }

    assert(res.unwrap().inputs.size() == 20);

    return 0;
}