# compile_time serialize
Single-header library which allows to prepare `std::array<uint8_t, ...>` filled with predefined data at compile time.

## Example

```cpp

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
```
would be equivalent to 

```cpp
std::array<uint8_t, 11> test {'a', 'b', 'c', 'd', 42, 0, 0xBE, 0xBA, 0xDA, 0xDE, 0xD}
```

[Try it on CompilerExplorer](https://godbolt.org/z/aM4z51)

## Build test example
```bash
$ mkdir build
$ cd build
$ cmake -G "Ninja" ..
$ cmake --build .
```