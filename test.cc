#include "tlvset.hh"
#include <stdint.h>

using tset = tlv::set<uint32_t, uint32_t>;

#define S  \
        "\x01\x00\x00\x00" \
        "\x02\x00\x00\x00" \
        "ab"

int main() {
    auto s = std::string{S, sizeof(S)-1};
    tset t{s};
    t.repr(std::cout);
    return 0;
}
