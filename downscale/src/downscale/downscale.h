// rasimvaliullin@hotmail.com

#ifndef __RECT_H
#define __RECT_H

#include <cassert>
#include <cmath>
#include <vector>

struct Point
{
    float x = 0, y = 0;
    bool operator ==(const Point &a) const { return x == a.x && y == a.y; }
};

struct Rect
{
    Point p1, p2;
    bool empty() const { return p1 == Point() && p2 == Point(); }
    float area() const { return (p2.x - p1.x) * (p2.y - p1.y); }
};

inline Rect intersection(const Rect &o, const Rect &a)
{
    const bool v1 = a.p1.x >= o.p1.x && a.p1.x <= o.p2.x && a.p1.y >= o.p1.y && a.p1.y <= o.p2.y;
    const bool v2 = a.p2.x >= o.p1.x && a.p2.x <= o.p2.x && a.p1.y >= o.p1.y && a.p1.y <= o.p2.y;
    const bool v3 = a.p2.x >= o.p1.x && a.p2.x <= o.p2.x && a.p2.y >= o.p1.y && a.p2.y <= o.p2.y;
    const bool v4 = a.p1.x >= o.p1.x && a.p1.x <= o.p2.x && a.p2.y >= o.p1.y && a.p2.y <= o.p2.y;

    if (v1 && v2 && v3 && v4) return a;
    if (v1 && v2)
    {
        const Point p2{ a.p2.x, o.p2.y };
        return Rect{ a.p1, p2 };
    }
    if (v3 && v4)
    {
        const Point p1{ a.p1.x, o.p1.y };
        return Rect{ p1, a.p2 };
    }
    if (v1 && v4)
    {
        const Point p2{ o.p2.x, a.p2.y };
        return Rect{ a.p1, p2 };
    }
    if (v2 && v3)
    {
        const Point p1{ o.p1.x, a.p1.y };
        return Rect{ p1, a.p2 };
    }
    if (v1) return Rect{ a.p1, o.p2 };
    if (v2)
    {
        const Point p1{ o.p1.x, a.p1.y }, p2{ a.p2.x, o.p2.y };
        return Rect{ p1, p2 };
    }
    if (v3) return Rect{ o.p1, a.p2 };
    if (v4)
    {
        const Point p1{ a.p1.x, o.p1.y }, p2{ o.p2.x, a.p2.y };
        return Rect{ p1, p2 };
    }

    return Rect();
}

inline Point mapCoords(const Point &a_src, const Point &a_ratio)
{
    return Point{ a_src.x * a_ratio.x, a_src.y * a_ratio.y };
}

template<typename T>
inline void downscale(const std::vector<T> &a_s, size_t a_sW, size_t a_sH,
                        std::vector<T> &a_d, size_t a_dW, size_t a_dH)
{
    assert(a_s.size() == a_sW * a_sH && a_d.size() == a_dW * a_dH && a_sW > a_dW && a_sH > a_dH);

    const Point l_d2sRatio{ float(a_sW) / a_dW, float(a_sH) / a_dH };

    for (size_t q = 0; q < a_d.size(); q++)
    {
        const size_t x = q % a_dW, y = q / a_dW;
        const Point p1(mapCoords(Point{ float(x), float(y) }, l_d2sRatio)),
                    p2(mapCoords(Point{ float(x + 1), float(y) }, l_d2sRatio)),
                    p3(mapCoords(Point{ float(x + 1), float(y + 1) }, l_d2sRatio)),
                    p4(mapCoords(Point{ float(x), float(y + 1) }, l_d2sRatio));

        const size_t l_xStart = size_t(p1.x), l_yStart = size_t(p1.y);
        size_t l_xEnd = size_t(p3.x), l_yEnd = size_t(p3.y);
        if (l_xEnd < a_sW && l_xEnd < p3.x) l_xEnd++;
        if (l_yEnd < a_sH && l_yEnd < p3.y) l_yEnd++;

        float l_s = 0, l_count = 0;
        for (size_t i = l_yStart; i < l_yEnd; i++)
        {
            for (size_t j = l_xStart; j < l_xEnd; j++)
            {
                const Rect l_intersection(intersection(Rect{ p1, p3 },
                                          Rect{ { float(j), float(i) }, { j + 1.0f, i + 1.0f } }));
                const size_t l_si = j + a_sW * i;
                const auto l_area = l_intersection.area();
                l_s += a_s[l_si] * l_area;
                l_count += l_area;
            }
        }

        a_d[q] = T(std::rint(l_s / l_count));
    }
}

#endif //__RECT_H
