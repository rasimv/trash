#include <QString>
#include <QtTest>
#include <bignumberdebug.h>

class BigTest : public QObject
{
    Q_OBJECT

public:
    BigTest();

private Q_SLOTS:
    void testCase1();

private:
    static QVector<quint8> gen()
    {
        QVector<quint8> l_buf(8, rand() % 100 < 50 ? 0 : 0xffu);
        if (rand() % 100 == 40)
            return QVector<quint8>(8, 0);
        if (rand() % 100 == 50)
            return QVector<quint8>(8, 0xffu);
        if (rand() % 100 == 60)
            return QVector<quint8>{ 1, 0, 0, 0, 0, 0, 0, 0 };
        int z = rand() % 9;
        for (int q = 0; q < z; q++)
            l_buf[q] = quint8(quint64(rand() % 256));
        return l_buf;
    }
};

BigTest::BigTest()
{
}

void BigTest::testCase1()
{
    for (int t = 0; t < 1000000; t++)
    {
        //Not perfect -- machine & compiler dependant
        //Works fine on x86-64 + vs2017
        const QVector<quint8> l_z1(gen()), l_z2(gen());
        const BigNumber l_bn1(l_z1), l_bn2(l_z2), l_bn3(l_bn1 + l_bn2);
        const qint64 v1 = bufToInt64(l_z1);
        const qint64 v2 = bufToInt64(l_z2);
        const qint64 v3 = v1 + v2;
        const qint64 bnv = BigNumberDebug(l_bn3).asInt64();
        //std::cout << t++ << "\t" << v3 << "\t" << bnv << std::endl;
        QVERIFY2(v3 == bnv, "Failure");
    }
}

QTEST_APPLESS_MAIN(BigTest)

#include "tst_bigtest.moc"
