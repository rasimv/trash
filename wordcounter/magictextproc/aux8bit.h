#ifndef __AUX8BIT_H
#define __AUX8BIT_H

#include <QString>
#include <QVector>

namespace aux8bit
{
    struct S8bit
    {
        std::array<char, 256> m_letter, m_lower;
        std::array<char, 256> m_toLower;
    };

    template<typename FROM>
    bool letter(char a_char, FROM a_from)
    {
        const QByteArray l_8bit(QByteArray(&a_char, 1));
        const QVector<uint> l_utf32(a_from(l_8bit).toUcs4());
        return !l_utf32.isEmpty() && QChar(l_utf32[0]).isLetter();  //Only BMP
    }

    template<typename FROM>
    bool lower(char a_char, FROM a_from)
    {
        const QByteArray l_8bit(QByteArray(&a_char, 1));
        const QVector<uint> l_utf32(a_from(l_8bit).toUcs4());
        return !l_utf32.isEmpty() && QChar(l_utf32[0]).isLower();   //Only BMP
    }

    template<typename FROM, typename TO>
    char toLower(char a_char, FROM a_from, TO a_to)
    {
        const QByteArray l_8bit1(QByteArray(&a_char, 1));
        const QVector<uint> l_utf32_1(a_from(l_8bit1).toUcs4());
        if (l_utf32_1.isEmpty()) return a_char;
        const QChar l_sym(l_utf32_1[0]);                            //Only BMP
        if (!l_sym.isLetter() || l_sym.isLower()) return a_char;
        const QByteArray l_8bit2(a_to(QString(l_sym.toLower())));
        const QVector<uint> l_utf32_2(a_from(l_8bit2).toUcs4());
        if (l_utf32_2.isEmpty()) return a_char;
        const QChar l_sym2(l_utf32_2[0]);                           //Only BMP
        return l_sym2.isLetter() && l_sym2.isLower() ? l_8bit2[0] : a_char;
    }

    template<typename FROM, typename TO>
    void initialize(S8bit &a_one, FROM a_from, TO a_to)
    {
        for (size_t i = 0; i < 256; i++)
        {
            const int q = int(i); const char x = q < 128 ? q : q - 256;
            a_one.m_letter[i] = letter(x, a_from);
            a_one.m_lower[i] = lower(x, a_from);
            a_one.m_toLower[i] = toLower(x, a_from, a_to);
        }
    }
}

#endif //__AUX8BIT_H
