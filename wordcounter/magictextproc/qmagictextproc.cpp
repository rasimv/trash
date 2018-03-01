#include "stdafx.h"
#include "qmagictextproc.h"

QString QBuildConfig::decor(const QString &a)
{
    return a
#ifndef QT_NO_DEBUG
        + " (debug - slow)"
#endif
        ;
}

QMagicTextProc::QMagicTextProc(QObject *a) :
    QObject(a)
{
    QObject::connect(this, SIGNAL(doneIntermed()), SLOT(onDoneIntermed()), Qt::QueuedConnection);
}

QMagicTextProc::~QMagicTextProc()
{}

void QMagicTextProc::go(QMagicFile *a)
{
    assert(m_core == nullptr && a != nullptr && a->m_file != nullptr);
    a->readInit();
    TextProc::EncParams l_params([&]()->TextProc::EncParams
    {
        if (a->m_enc == QMagicFile::E_LOCAL8) return makeLocal8();
        else if (a->m_enc == QMagicFile::E_UTF8) return makeUtf(TextProc::E_UTF8);
        else if (a->m_enc == QMagicFile::E_UTF16LE) return makeUtf(TextProc::E_UTF16LE);
        else if (a->m_enc == QMagicFile::E_UTF16BE) return makeUtf(TextProc::E_UTF16BE);
        else return makeLatin1();
    }());
    TextProc::F_VoidVoidP l_done(std::bind(&QMagicTextProc::callbackDone, this, std::placeholders::_1));
    m_core = std::unique_ptr<TextProc>(new TextProc(2 * 1024 * 1024, 1024, l_params, a->m_file, l_done));
}

template<typename A, bool B> struct QMTP_Fictive
{
    static QStringList notSyncStatSub(const A &a_stat) { return QStringList(); }
};

template<typename A> struct QMTP_Fictive<A, false>
{
    static QStringList notSyncStatSub(const A &a_stat)
    {
        return QStringList{ QString::number(a_stat[TextProc::SI_UNIQ_COUNT]),
                            QString::number(a_stat[TextProc::SI_LINE_COUNT]),
                            QString::number(a_stat[TextProc::SI_WORD_COUNT]) };
    }
};

template<typename A> struct QMTP_Fictive<A, true>
{
    static QStringList notSyncStatSub(const A &a_stat)
    {
        const auto l_lineCount = (uint64_t(a_stat[TextProc::SI_LINE_COUNT_OVERFLOW]) << 8 * sizeof(size_t)) + a_stat[TextProc::SI_LINE_COUNT];
        const auto l_wordCount = (uint64_t(a_stat[TextProc::SI_WORD_COUNT_OVERFLOW]) << 8 * sizeof(size_t)) + a_stat[TextProc::SI_WORD_COUNT];
        return QStringList{ QString::number(a_stat[TextProc::SI_UNIQ_COUNT]),
                            QString::number(l_lineCount),
                            QString::number(l_wordCount) };
    }
};

QStringList QMagicTextProc::notSyncStat()
{
    assert(m_core != nullptr);
    const auto l_stat(m_core->notSyncStat());
    return QMTP_Fictive<decltype(l_stat), sizeof(size_t) < sizeof(uint64_t) > ::notSyncStatSub(l_stat);
}

void QMagicTextProc::callbackDone(void *a)
{
    emit doneIntermed();
}

namespace
{
    QString fromLatin1(const QByteArray &a) { return QString::fromLatin1(a); }
    QByteArray toLatin1(const QString &a) { return a.toLatin1(); }
    QString fromLocal8(const QByteArray &a) { return QString::fromLocal8Bit(a); }
    QByteArray toLocal8(const QString &a) { return a.toLocal8Bit(); }
}

void QMagicTextProc::initialize()
{
    aux8bit::initialize(s_latin1, fromLatin1, toLatin1);
    aux8bit::initialize(s_local8, fromLocal8, toLocal8);

    qmlRegisterType<QBuildConfig>("io.qt.examples.magictextproc", 1, 0, "QBuildConfig");
    qmlRegisterType<QMagicFile>("io.qt.examples.magictextproc", 1, 0, "QMagicFile");
    qmlRegisterType<QMagicTextProc>("io.qt.examples.magictextproc", 1, 0, "QMagicTextProc");
}

void QMagicTextProc::onDoneIntermed()
{
    emit done();
}

TextProc::EncParams QMagicTextProc::makeLatin1()
{
    return TextProc::EncParams
        {
            TextProc::E_8BIT,
            [&](uint32_t a)->bool { assert(a < s_latin1.m_letter.size()); return s_latin1.m_letter[a]; },
            [&](uint32_t a)->bool { assert(a < s_latin1.m_lower.size()); return s_latin1.m_lower[a]; },
            [&](uint32_t a)->uint32_t { assert(a < s_latin1.m_toLower.size()); return s_latin1.m_toLower[a]; }
        };
}

TextProc::EncParams QMagicTextProc::makeLocal8()
{
    return TextProc::EncParams
        {
            TextProc::E_8BIT,
            [&](uint32_t a)->bool { assert(a < s_local8.m_letter.size()); return s_local8.m_letter[a]; },
            [&](uint32_t a)->bool { assert(a < s_local8.m_lower.size()); return s_local8.m_lower[a]; },
            [&](uint32_t a)->uint32_t { assert(a < s_local8.m_toLower.size()); return s_local8.m_toLower[a]; }
        };
}

TextProc::EncParams QMagicTextProc::makeUtf(TextProc::Encoding a_enc)
{
    return TextProc::EncParams
        {
            a_enc,
            [&](uint32_t a)->bool { return QChar::isLetter(a); },
            [&](uint32_t a)->bool { return QChar::isLower(a); },
            [&](uint32_t a)->uint32_t { return QChar::toLower(a); }
        };
}

aux8bit::S8bit QMagicTextProc::s_latin1, QMagicTextProc::s_local8;
