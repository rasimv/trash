#include <cstdint>
#include <cassert>
#include <type_traits>
#include <utility>

template<typename T, uint64_t Mask>
class SparseArray {
public:
    using ElementType = T;

    constexpr SparseArray() :
        values{ T() } {
    }

    template<typename ... E>
    constexpr SparseArray(E&& ... e) :
        values{ T(), std::forward<E>(e)... } {
    }

    template<uint8_t Index>
    constexpr const T& get() const {
        if (Mask >> Index == 0) return values[0];
        std::size_t s = 0;
        for (std::size_t q = 0; ; q++) {
            const auto b = 1u & Mask >> q;
            if (q == Index) return values[b ? 1 + s : 0];
            s += b;
        }
    }

    template<typename TOther, uint64_t MaskOther>
    constexpr auto operator +(const SparseArray<TOther, MaskOther>& other) const
        ->SparseArray<decltype(T() + TOther()), Mask | MaskOther> {
        const uint64_t merged = Mask | MaskOther;
        const std::size_t number = calcNumber(merged);
        return Internal<number, number, merged,
            SparseArray<T, Mask>, SparseArray<TOther, MaskOther>>::add(*this, other);
    }

private:
    template<std::size_t I, std::size_t N, uint64_t M, typename O, typename A>
    struct Internal {
        template<typename ... E>
        static constexpr auto add(const O &o, const A &a, E &&... e) {
            const std::size_t index = findIndex(M, N - I);
            return Internal<I - 1, N, M, O, A>::
                add(o, a, e..., o.template get<index>() + a.template get<index>());
        }
    };

    template<std::size_t N, uint64_t M, typename O, typename A>
    struct Internal<0, N, M, O, A> {
        template<typename ... E>
        static constexpr auto add(const O &o, const A &a, E &&... e) {
            return SparseArray<decltype(typename O::ElementType() +
                                        typename A::ElementType()), M>
                                                (std::forward<E>(e)...);
        }
    };

    static constexpr std::size_t findIndex(uint64_t mask, std::size_t count) {
        std::size_t q = 0, c = 0;
        for (; q < 64; q++)
            if ((1u & (mask >> q)) != 0) {
                if (c == count) return q;
                c++;
            }
        return q;
    }

    static constexpr std::size_t calcNumber(uint64_t mask) {
        std::size_t s = 0;
        for (uint64_t q = mask; q != 0; q >>= 1) s += 1u & q;
        return s;
    }

    T values[1 + calcNumber(Mask)];
};

int main() {
    constexpr SparseArray<float, 3 > array0(1.0f, 2.0f);
    constexpr SparseArray<double, 10> array1(4.0, 7.0);

    constexpr auto sum(array0 + array1);

    static_assert(sizeof(sum) == sizeof(double) * (3 + 1), "Invalid sum array size");
    static_assert(sizeof(array0) == sizeof(float) * (2 + 1), "Invalid array size");
    static_assert(sizeof(array1) == sizeof(double) * (2 + 1), "Invalid array size");

    static_assert((std::is_same_v<typename decltype(sum)::ElementType, double> == true), "Invalid sum element type");

    static_assert(sum.get<0>() == 1.0, "Invalid sum element value");
    static_assert(sum.get<1>() == 6.0, "Invalid sum element value");
    static_assert(sum.get<2>() == 0.0, "Invalid sum element value");
    static_assert(sum.get<3>() == 7.0, "Invalid sum element value");

    SparseArray<float, 3> array2;
    assert(array2.get<0>() == 0.0f);
    assert(array2.get<1>() == 0.0f);

    return 0;
}
