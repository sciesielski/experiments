#ifndef _CT_SERIALIZE_HEADER_
#define _CT_SERIALIZE_HEADER_

#include <climits>
#include <cstddef>
#include <cstdint>

#include <utility>
#include <tuple>
#include <array>

namespace ct
{
    namespace util
    {
        template<typename... args>
        constexpr int sum_size()
        {
            return (args::size + ...);
        }

        template<typename Array, std::size_t... I>
        constexpr auto a2t_impl(const Array& a, std::index_sequence<I...>)
        {
            return std::tuple(a[I]...);
        }

        template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
        constexpr auto a2t(const std::array<T, N>& a)
        {
            return a2t_impl(a, Indices{});
        }

        template<typename... args>
        constexpr auto array_cat(args... arrays)
        {
            return apply([](auto... n) {return std::array{ n... }; }, std::tuple_cat(a2t(arrays)...));
        }

        template<typename T, std::size_t N>
        constexpr auto str_to_array(const T(&arg)[N])
        {
            std::array<uint8_t, N> result{};
            for (std::size_t i = 0; i < N; ++i)
            {
                result[i] = (arg[i]);
            }

            return result;
        }

        template<typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
        constexpr auto to_array(T arg)
        {
            std::array<uint8_t, sizeof(T)> result{};
            for (std::size_t i = 0; i < sizeof(T); ++i)
            {
                result[i] = (arg >> (i * CHAR_BIT)) & std::uint8_t(-1);
            }

            return result;
        }

        template<typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
        constexpr auto to_array(T arg)
        {
            union FloatToChar {
                T f;
                uint8_t  c[sizeof(T)];
            };

            FloatToChar buf;
            buf.f = arg;

            std::array<uint8_t, sizeof(T)> result{};
            for (std::size_t i = 0; i < sizeof(T); ++i)
            {
                result[i] = (buf.c[i]);
            }

            return result;
        }
    }

    namespace ser
    {
        template<typename CRTP, size_t s> struct Serialized
        {
            static constexpr size_t size{ s };
            using container = std::array<uint8_t, size>;
            using Serialized_base = Serialized;

            constexpr explicit Serialized(container args) noexcept : data{ args } {}

            template<typename... T>
            constexpr explicit Serialized(T... args) noexcept : data{ static_cast<typename container::value_type>(args)... } {}

            [[nodiscard]] constexpr auto get_data() const noexcept { return data; }

            container data;
        };

        template<typename Type, typename Size = std::integral_constant<size_t, sizeof(Type)>>
        struct Item : Serialized<Item<Type, Size>, Size::value>
        {
        };


        template<typename Type>
        struct Item<Type, std::integral_constant<size_t, sizeof(Type)>> : Serialized<Item<Type, std::integral_constant<size_t, sizeof(Type)>>, std::integral_constant<size_t, sizeof(Type)>::value>
        {
            using Serialized_base = Serialized<Item<Type, std::integral_constant<size_t, sizeof(Type)>>, std::integral_constant<size_t, sizeof(Type)>::value>;
            using Serialized_base::Serialized_base;

            template<typename T>
            constexpr explicit Item(T arg) : Serialized_base(util::to_array<Type>(arg)) {}

            constexpr explicit Item(typename Serialized_base::container arg) : Serialized_base(arg) {}
        };

        template <typename... Types>
        struct Aggregate : Serialized<Aggregate<Types...>, util::sum_size<Types...>()>
        {
            using Serialized_base = Serialized<Aggregate<Types...>, util::sum_size<Types...>()>;
            using Serialized_base::Serialized_base;

            constexpr explicit Aggregate(Types... args) : Serialized_base(util::array_cat(args.get_data()...)) {}
        };

    } // namespace ser

} // namespace ct

#endif // _CT_SERIALIZE_HEADER_
