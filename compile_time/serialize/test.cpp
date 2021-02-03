#include <cstring>
#include "ct_serialize.h"

using SessionId = ct::ser::Item<char[4]>;
using Count = ct::ser::Item<uint16_t>;
using Timestamp = ct::ser::Item<uint32_t>;
using Checksum = ct::ser::Item<uint8_t>;

template <typename... Types>
struct Frame : ct::ser::Aggregate<Types...> {
    using Aggregate_base = ct::ser::Aggregate<Types...>;
    using Aggregate_base::Aggregate_base;

    constexpr explicit Frame(Types... args) : Aggregate_base(args...) {}
};

constexpr auto test{
    Frame {
        SessionId {
            'a','b','c','d'
        },
        Count {
            42
        },
        Timestamp {
            0xDEADBABE
        },
        Checksum {
            0xD
        }
    }
};

size_t fn(uint8_t* ptr, size_t max_size)
{
    if (test.get_data().size() > max_size)
        return 0;

    std::memcpy(ptr, test.get_data().data(), test.get_data().size());
    return test.get_data().size();
}