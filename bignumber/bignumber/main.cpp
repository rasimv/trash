#include <QCoreApplication>
#include "bignumber.h"
#include <iostream>

int main(int argc, char *argv[])
{
    const BigNumber l_bnX("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    const BigNumber l_bnY("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    const BigNumber l_bnZ = l_bnX + l_bnY;
    auto l_asByteArray((l_bnZ.toString() + "\0").toLocal8Bit());
    std::cout << l_asByteArray.data() << std::endl;
    return 0;
}
