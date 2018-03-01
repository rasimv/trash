#include "stdafx.h"
#include "textproc.h"

uint32_t TextProc::read8bit()
{
    assert(m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    const unsigned char q = nextByte();
    if (m_endOfData) return S_READ_END;
    if (m_wordSkip < m_wordBufSkip) { *m_wordSkip = q; m_wordSkip++; }
    return q;
}

uint32_t TextProc::readUtf8()
{
    assert(m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    const unsigned char q1 = nextByte();
    if (m_endOfData) return S_READ_END;
    if ((q1 & 0x80) == 0)
    {
        if (m_wordSkip + 1 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { *m_wordSkip = q1; m_wordSkip++; }
        return q1;
    }
    if ((q1 & 0xe0) == 0xc0)
    {
        const unsigned char q2 = nextByte();
        if (m_endOfData || (q2 & 0xc0) != 0x80) return S_INVALID;
        if (m_wordSkip + 2 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip += 2; }
        return q2 & ~0xc0 | uint32_t(q1 & ~0xe0) << 6;
    }
    if ((q1 & 0xf0) == 0xe0)
    {
        const unsigned char q2 = nextByte();
        if (m_endOfData || (q2 & 0xc0) != 0x80) return S_INVALID;
        const unsigned char q3 = nextByte();
        if (m_endOfData || (q3 & 0xc0) != 0x80) return S_INVALID;
        if (m_wordSkip + 3 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip[2] = q3; m_wordSkip += 3; }
        return q3 & ~0xc0 | uint32_t(q2 & ~0xc0) << 6 | uint32_t(q1 & ~0xf0) << 12;
    }
    if ((q1 & 0xf8) == 0xf0)
    {
        const unsigned char q2 = nextByte();
        if (m_endOfData || (q2 & 0xc0) != 0x80) return S_INVALID;
        const unsigned char q3 = nextByte();
        if (m_endOfData || (q3 & 0xc0) != 0x80) return S_INVALID;
        const unsigned char q4 = nextByte();
        if (m_endOfData || (q4 & 0xc0) != 0x80) return S_INVALID;
        if (m_wordSkip + 4 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip[2] = q3; m_wordSkip[3] = q4; m_wordSkip += 4; }
        return q4 & ~0xc0 | uint32_t(q3 & ~0xc0) << 6 | uint32_t(q2 & ~0xc0) << 12 | uint32_t(q1 & ~0xf8) << 18;
    }
    return S_INVALID;
}

uint32_t TextProc::readUtf16le()
{
    assert(m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    const unsigned char q1 = nextByte();
    if (m_endOfData) return S_READ_END;
    const unsigned char q2 = nextByte();
    if (m_endOfData) return S_INVALID;
    if ((q2 & 0xf8) != 0xd8)
    {
        if (m_wordSkip + 2 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip += 2; }
        return uint32_t(q2) << 8 | q1;
    }
    const unsigned char q3 = nextByte();
    if (m_endOfData) return S_INVALID;
    const unsigned char q4 = nextByte();
    if (m_endOfData) return S_INVALID;
    if ((q2 & 0xfc) != 0xd8 || (q4 & 0xfc) != 0xdc)
        return S_INVALID;
    if (m_wordSkip + 4 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
    else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip[2] = q3; m_wordSkip[3] = q4; m_wordSkip += 4; }
    return 0x10000u + (q3 | uint32_t(q4 & ~0xfc) << 8 | uint32_t(q1) << 10 | uint32_t(q2 & ~0xfc) << 18);
}

uint32_t TextProc::readUtf16be()
{
    assert(m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    const unsigned char q1 = nextByte();
    if (m_endOfData) return S_READ_END;
    const unsigned char q2 = nextByte();
    if (m_endOfData) return S_INVALID;
    if ((q1 & 0xf8) != 0xd8)
    {
        if (m_wordSkip + 2 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip += 2; }
        return uint32_t(q1) << 8 | q2;
    }
    const unsigned char q3 = nextByte();
    if (m_endOfData) return S_INVALID;
    const unsigned char q4 = nextByte();
    if (m_endOfData) return S_INVALID;
    if ((q1 & 0xfc) != 0xd8 || (q3 & 0xfc) != 0xdc)
        return S_INVALID;
    if (m_wordSkip + 4 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
    else { m_wordSkip[0] = q1; m_wordSkip[1] = q2; m_wordSkip[2] = q3; m_wordSkip[3] = q4; m_wordSkip += 4; }
    return 0x10000u + (q4 | uint32_t(q3 & ~0xfc) << 8 | uint32_t(q2) << 10 | uint32_t(q1 & ~0xfc) << 18);
}

void TextProc::fix8bit()
{
    assert(m_wordFix != nullptr && m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    if (m_wordFix >= m_wordSkip) return;
    if (!m_encParams.m_isLower(m_sym)) *m_wordFix = m_encParams.m_toLower(m_sym);
    m_wordFix = m_wordSkip;
}

void TextProc::fixUtf8()
{
    assert(m_wordFix != nullptr && m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    if (m_wordFix >= m_wordSkip) return;
    if (m_encParams.m_isLower(m_sym)) { m_wordFix = m_wordSkip; return; }
    m_wordSkip = m_wordFix;
    const uint32_t q = m_encParams.m_toLower(m_sym);
    if (q < 128)
    {
        if (m_wordSkip + 1 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { *m_wordSkip = td_uchar(q); m_wordSkip++; }
    }
    else if (q < 0x800)
    {
        if (m_wordSkip + 2 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = td_uchar(0xc0 | q >> 6); m_wordSkip[1] = td_uchar(0x80 | 0x3f & q); m_wordSkip += 2; }
    }
    else if (q < 0x10000)
    {
        if (m_wordSkip + 3 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else
        {
            m_wordSkip[0] = td_uchar(0xe0 | q >> 12);
            m_wordSkip[1] = td_uchar(0x80 | 0x3f & q >> 6); m_wordSkip[2] = td_uchar(0x80 | 0x3f & q);
            m_wordSkip += 3;
        }
    }
    else
    {
        if (m_wordSkip + 4 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else
        {
            m_wordSkip[0] = td_uchar(0xf0 | 0x7 & q >> 18); m_wordSkip[1] = td_uchar(0x80 | 0x3f & q >> 12);
            m_wordSkip[2] = td_uchar(0x80 | 0x3f & q >> 6); m_wordSkip[3] = td_uchar(0x80 | 0x3f & q);
            m_wordSkip += 4;
        }
    }
    m_wordFix = m_wordSkip;
}

void TextProc::fixUtf16le()
{
    assert(m_wordFix != nullptr && m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    if (m_wordFix >= m_wordSkip) return;
    if (m_encParams.m_isLower(m_sym)) { m_wordFix = m_wordSkip; return; }
    m_wordSkip = m_wordFix;
    const uint32_t q = m_encParams.m_toLower(m_sym);
    if (q < 0x10000)
    {
        if (m_wordSkip + 2 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[0] = td_uchar(q); m_wordSkip[1] = td_uchar(q >> 8); m_wordSkip += 2; }
    }
    else
    {
        const uint32_t u = q - 0x10000;
        if (m_wordSkip + 4 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else
        {
            m_wordSkip[0] = td_uchar(u >> 10); m_wordSkip[1] = td_uchar(0xd8 | ~0xfc & (u >> 18));
            m_wordSkip[2] = td_uchar(u); m_wordSkip[3] = td_uchar(0xdc | ~0xfc & (u >> 8));
            m_wordSkip += 4;
        }
    }
    m_wordFix = m_wordSkip;
}

void TextProc::fixUtf16be()
{
    assert(m_wordFix != nullptr && m_wordSkip != nullptr && m_wordBufSkip != nullptr);
    if (m_wordFix >= m_wordSkip) return;
    if (m_encParams.m_isLower(m_sym)) { m_wordFix = m_wordSkip; return; }
    m_wordSkip = m_wordFix;
    const uint32_t q = m_encParams.m_toLower(m_sym);
    if (q < 0x10000)
    {
        if (m_wordSkip + 2 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else { m_wordSkip[1] = td_uchar(q); m_wordSkip[0] = td_uchar(q >> 8); m_wordSkip += 2; }
    }
    else
    {
        const uint32_t u = q - 0x10000;
        if (m_wordSkip + 4 > m_wordBufSkip) m_wordBufSkip = m_wordSkip;
        else
        {
            m_wordSkip[1] = td_uchar(u >> 10); m_wordSkip[0] = td_uchar(0xd8 | ~0xfc & (u >> 18));
            m_wordSkip[3] = td_uchar(u); m_wordSkip[2] = td_uchar(0xdc | ~0xfc & (u >> 8));
            m_wordSkip += 4;
        }
    }
    m_wordFix = m_wordSkip;
}
