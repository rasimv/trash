#include <iostream>
#include <unordered_set>
#include <set>
#include <string_view>
#include <chrono>
#include <cassert>
#include <functional>

// v1
template <typename C>
long substringCalculator1(const std::string &s)
{
    assert(!s.empty());
    long q = 1;

    C distinct;
    for (size_t l = s.size() - 1; l > 0; l--)
    {
        distinct.clear();
        for (size_t i = 0; i + l <= s.size(); i++)
            distinct.emplace(&s[i], l);
        q += long(distinct.size());
    }

    return q;
}

// v2
template <typename C>
long substringCalculator2(const std::string &s)
{
    assert(!s.empty());
    C distinct1, distinct2;
    distinct1.emplace(s.data(), s.size());
    long q = 1;

    for (size_t w = 0; w < s.size() - 1; w++)
    {
        for (decltype(std::begin(distinct1)) i(std::begin(distinct1)); i != std::end(distinct1); )
        {
            for (size_t j = 0; j + i->size() - 1 <= i->size(); j++) distinct2.emplace(i->data() + j, i->size() - 1);
            const auto k(i++);
            //distinct1.erase(k);     // To save some memory
        }
        q += long(distinct2.size());
        distinct1 = std::move(distinct2);
        distinct2.clear();
    }

    return q;
}

//=======================================================
std::string generateRandomString(size_t length)
{
    std::string s(length, '\0');
    for (auto &q : s) q = 'a' + rand() % ('z' - 'a' + 1);
    return s;
}

template <typename T>
double elapsed(T t1, T t2)
{
    return std::rint(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() / 10.0) / 100;
}

int main()
{
    while (true)
    {
        auto s(generateRandomString(4000));

        // v1
        std::cout << "v1 (set, string): " << std::flush;
        const auto t1b = std::chrono::high_resolution_clock::now();
        const auto c1 = substringCalculator1<std::set<std::string>>(s);
        const auto t1e = std::chrono::high_resolution_clock::now();
        std::cout << c1 << " - " <<  elapsed(t1b, t1e) << " s" << std::endl;

        std::cout << "v1 (set, string_view): " << std::flush;
        const auto t2b = std::chrono::high_resolution_clock::now();
        const auto c2 = substringCalculator1<std::set<std::string_view>>(s);
        const auto t2e = std::chrono::high_resolution_clock::now();
        std::cout << c2 << " - " << elapsed(t2b, t2e) << " s" << std::endl;

        std::cout << "v1 (unordered_set, string): " << std::flush;
        const auto t3b = std::chrono::high_resolution_clock::now();
        const auto c3 = substringCalculator1<std::unordered_set<std::string>>(s);
        const auto t3e = std::chrono::high_resolution_clock::now();
        std::cout << c3 << " - " << elapsed(t3b, t3e) << " s" << std::endl;

        std::cout << "v1 (unordered_set, string_view): " << std::flush;
        const auto t4b = std::chrono::high_resolution_clock::now();
        const auto c4 = substringCalculator1<std::unordered_set<std::string_view>>(s);
        const auto t4e = std::chrono::high_resolution_clock::now();
        std::cout << c4 << " - " << elapsed(t4b, t4e) << " s" << std::endl;

        // v2
        std::cout << "v2 (set, string): " << std::flush;
        const auto t5b = std::chrono::high_resolution_clock::now();
        const auto c5 = substringCalculator2<std::set<std::string>>(s);
        const auto t5e = std::chrono::high_resolution_clock::now();
        std::cout << c5 << " - " << elapsed(t5b, t5e) << " s" << std::endl;

        std::cout << "v2 (set, string_view): " << std::flush;
        const auto t6b = std::chrono::high_resolution_clock::now();
        const auto c6 = substringCalculator2<std::set<std::string_view>>(s);
        const auto t6e = std::chrono::high_resolution_clock::now();
        std::cout << c6 << " - " << elapsed(t6b, t6e) << " s" << std::endl;

        std::cout << "v2 (unordered_set, string): " << std::flush;
        const auto t7b = std::chrono::high_resolution_clock::now();
        const auto c7 = substringCalculator2<std::unordered_set<std::string>>(s);
        const auto t7e = std::chrono::high_resolution_clock::now();
        std::cout << c7 << " - " << elapsed(t7b, t7e) << " s" << std::endl;

        std::cout << "v2 (unordered_set, string_view): " << std::flush;
        const auto t8b = std::chrono::high_resolution_clock::now();
        const auto c8 = substringCalculator2<std::unordered_set<std::string_view>>(s);
        const auto t8e = std::chrono::high_resolution_clock::now();
        std::cout << c8 << " - " << elapsed(t8b, t8e) << " s" << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
