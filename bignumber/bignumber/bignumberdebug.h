#ifndef __BIGNUMBERDEBUG_H
#define __BIGNUMBERDEBUG_H

#include "bignumber.h"

qint64 bufToInt64(const QVector<quint8> &a_buf)
{
    quint64 v = 0;
    for (int q = 0; q < a_buf.size(); q++)
        v |= quint64(a_buf[q]) << (8 * q);
    return v;   //Not perfect (unsigned -> signed)
}

class BigNumberDebug : public BigNumber
{
public:
    BigNumberDebug(const BigNumber &a) { *static_cast<BigNumber *>(this) = a; }

    qint64 asInt64() const
    {
        if (m_buf.empty()) return 0;
        QVector<quint8> l_buf(m_buf);
        for (int q = 0; 8 > l_buf.size(); q++)
            l_buf.push_back((0x80u & *m_buf.rbegin()) == 0 ? 0 : 0xffu);
        l_buf.resize(8);
        return bufToInt64(l_buf);
    }
};

#endif //__BIGNUMBERDEBUG_H
