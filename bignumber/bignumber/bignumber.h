#ifndef __BIGNUMBER_H
#define __BIGNUMBER_H

#include <cassert>
#include <QString>
#include <QVector>

class BigNumber
{
public:
    BigNumber() {}
    explicit BigNumber(const QVector<quint8> &a_buf) :m_buf(a_buf) {}
    explicit BigNumber(const QString &a)
    {
        if (a.isEmpty()) return;
        const bool l_pos = a[0] != '-';
        BigNumber l_bn(fromStringPositive(a, int(!l_pos)));
        if (l_bn.m_buf.empty()) return;
        if (!l_pos)
        {
            for (int q = 0; q < l_bn.m_buf.size(); q++)
                l_bn.m_buf[q] = ~l_bn.m_buf[q];
            l_bn = l_bn + BigNumber(QVector<quint8>{1});
        }
        m_buf = std::move(l_bn.m_buf);
        if (l_pos && (0x80u & *m_buf.rbegin()) != 0) m_buf.push_back(0);
        if (!l_pos && (0x80u & *m_buf.rbegin()) == 0) m_buf.push_back(0xffu);
    }

    QString toString() const
    {
        if (m_buf.empty()) return "0";
        const bool l_pos = (0x80u & *m_buf.rbegin()) == 0;
        if (l_pos) return toStringPositive(m_buf);
        BigNumber l_bn(m_buf);
        for (int q = 0; q < l_bn.m_buf.size(); q++)
            l_bn.m_buf[q] = ~l_bn.m_buf[q];
        l_bn = l_bn + BigNumber(QVector<quint8>{1});
        return QString("-%1").arg(toStringPositive(l_bn.m_buf));
    }

    friend BigNumber operator +(const BigNumber &o, const BigNumber &a);

public:
    static void remove0(QVector<quint8> &a);
    static int div2(const QVector<quint8> &a_value, QVector<quint8> &a_integer);
    static QVector<quint8> fromStringPositive(const QString &a_value, int a_start = 0);
    static int div10(const QVector<quint8> &a_value, QVector<quint8> &a_integer);
    static QString toStringPositive(const QVector<quint8> &a);

    QVector<quint8> m_buf;
};

inline void BigNumber::remove0(QVector<quint8> &a)
{
    if (a.size() < 2) return;
    const auto l_found(std::find_if(a.begin(), a.end() - 1, [](quint8 a)->bool { return a != 0; }));
    a.erase(a.begin(), l_found);
}

inline int BigNumber::div2(const QVector<quint8> &a_value, QVector<quint8> &a_integer)
{
    assert(!a_value.empty());
    int u = 0;
    for (int q = 0; q < a_value.size(); q++)
    {
        u *= 10;
        u += a_value[q];
        a_integer.push_back(u / 2);
        u %= 2;
    }
    return u;
}

inline QVector<quint8> BigNumber::fromStringPositive(const QString &a_value, int a_start)
{
    QVector<quint8> l_value;
    for (int q = a_start; q < a_value.size(); q++)
    {
        if (a_value[q] == "0") l_value.push_back(0);
        else if (a_value[q] == "1") l_value.push_back(1);
        else if (a_value[q] == "2") l_value.push_back(2);
        else if (a_value[q] == "3") l_value.push_back(3);
        else if (a_value[q] == "4") l_value.push_back(4);
        else if (a_value[q] == "5") l_value.push_back(5);
        else if (a_value[q] == "6") l_value.push_back(6);
        else if (a_value[q] == "7") l_value.push_back(7);
        else if (a_value[q] == "8") l_value.push_back(8);
        else if (a_value[q] == "9") l_value.push_back(9);
        else return QVector<quint8>();
    }

    if (l_value.empty()) return QVector<quint8>();
    remove0(l_value);

    quint64 w = 0;
    QVector<quint8> l_pattern;
    for (; l_value.size() != 1 || l_value[0] != 0; w++)
    {
        QVector<quint8> l_integer;
        const int l_rem = div2(l_value, l_integer);
        remove0(l_integer);
        l_value = l_integer;
        if (l_rem == 0) continue;
        int l_byteIndex = w / 8, l_bitIndex = w % 8;
        if (l_byteIndex >= l_pattern.size()) l_pattern.resize(l_byteIndex + 1);
        l_pattern[l_byteIndex] |= (1 << l_bitIndex);
    }
    return l_pattern;
}

inline int BigNumber::div10(const QVector<quint8> &a_value, QVector<quint8> &a_integer)
{
    assert(!a_value.empty());
    int u = 0;
    for (int q = 0; q < a_value.size(); q++)
    {
        u *= 16;
        u += a_value[q];
        a_integer.push_back(u / 10);
        u %= 10;
    }
    return u;
}

inline QString BigNumber::toStringPositive(const QVector<quint8> &a)
{
    QVector<quint8> l_hex;
    for (int q = a.size(); q > 0; q--)
    {
        l_hex.push_back(a[q - 1] >> 4);
        l_hex.push_back(0xfu & a[q - 1]);
    }

    if (l_hex.empty()) return QString("0");
    remove0(l_hex);

    QString l_pattern;
    for (; l_hex.size() != 1 || l_hex[0] != 0; )
    {
        QVector<quint8> l_integer;
        const int l_rem = div10(l_hex, l_integer);
        remove0(l_integer);
        l_hex = l_integer;
        l_pattern = ('0' + l_rem) + l_pattern;
    }
    return l_pattern;
}

inline BigNumber operator +(const BigNumber &o, const BigNumber &a)
{
    const bool l_longer = o.m_buf.size() > a.m_buf.size();

    BigNumber l_bn(l_longer ? o.m_buf : a.m_buf);
    QVector<quint8> &l_buf1 = l_bn.m_buf;
    if (l_buf1.empty()) return l_bn;
    const QVector<quint8> &l_buf2 = l_longer ? a.m_buf : o.m_buf;
    if (l_buf2.empty()) return l_bn;

    const bool l_1pos = *l_buf1.rbegin() < 0x80u;
    const bool l_2pos = *l_buf2.rbegin() < 0x80u;

    bool l_carry = false;
    for (int q = 0; q < l_buf1.size(); q++)
    {
        quint16 l_sub = l_buf1[q] + unsigned(l_carry);
        if (q < l_buf2.size()) l_sub += l_buf2[q];
        else if (!l_2pos) l_sub += 0xffu;
        l_buf1[q] = l_sub;
        l_carry = 0xffu < l_sub;
    }

    if (l_1pos && l_2pos)
    {
        if ((0x80u & *l_buf1.rbegin()) != 0) l_buf1.push_back(0);
    }
    else if (!l_1pos && !l_2pos)
    {
        if ((0x80u & *l_buf1.rbegin()) == 0) l_buf1.push_back(0xffu);
    }
    else
    {
        const auto l_last = *l_buf1.rbegin();
        if (l_last != 0 && l_last != 0xffu)
            return l_bn;
        int q = l_buf1.size();
        for (; q > 0 && l_buf1[q - 1] == l_last; q--);
        if (q == 0)
        {
            if (l_last == 0) l_buf1.clear();
            else l_buf1.resize(1);
            return l_bn;
        }

        if (bool(l_last) != bool(0x80u & l_buf1[q - 1]))
            q++;

        l_buf1.erase(l_buf1.begin() + q, l_buf1.end());
    }

    return l_bn;
}

#endif //__BIGNUMBER_H
