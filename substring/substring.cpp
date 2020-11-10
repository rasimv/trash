#include <iostream>
#include <unordered_set>
#include <string_view>
#include <chrono>

// straightforward
long substringCalculator(const std::string &s)
{
    long q = 1;

    std::unordered_set<std::string> distinct;
    for (size_t l = s.size() - 1; l > 0; l--)
    {
        distinct.clear();
        for (size_t i = 0; i + l <= s.size(); i++)
            distinct.insert(s.substr(i, l));
        q += distinct.size();
    }

    return q;
}

// alternative
long substringCalculator2(const std::string &s)
{
    long q = 1;

    std::unordered_set<std::string_view> distinct;
    for (size_t l = s.size() - 1; l > 0; l--)
    {
        distinct.clear();
        for (size_t i = 0; i + l <= s.size(); i++)
            distinct.emplace(&s[i], l);
        q += distinct.size();
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
    return std::rint(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() / 100.0) / 10;
}

int main()
{
    while (true)
    {
        auto s(generateRandomString(3000));
        const auto t1 = std::chrono::high_resolution_clock::now();
        const auto c1 = substringCalculator(s);
        const auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "v1: " << elapsed(t1, t2) << " s" << std::flush;
        const auto c2 = substringCalculator2(s);
        const auto t3 = std::chrono::high_resolution_clock::now();
        std::cout << ", v2: " << elapsed(t2, t3) << " s" << std::endl << std::endl;
    }

    return 0;
}
