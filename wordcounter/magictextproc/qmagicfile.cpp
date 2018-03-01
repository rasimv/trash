#include "stdafx.h"
#include "qmagicfile.h"

QMagicFile::QMagicFile(QObject *a) :
    QObject(a),
    m_enc(E_LATIN1), m_data(2 * 1024), m_bomSkip(0)
{}

QMagicFile::~QMagicFile()
{}

bool QMagicFile::open(const QString &a_fileUrl)
{
    if (a_fileUrl.isEmpty()) return false;
    const QUrl l_fileUrl(a_fileUrl);
    const QString l_filepath(l_fileUrl.toLocalFile());
    auto l_file(std::make_shared<std::ifstream>(
#ifdef _WIN32
        l_filepath.utf16()
#else
        QByteArray(l_filepath.toUtf8()).data()
#endif
        , std::ios::in | std::ios::binary));
    if (!l_file->is_open())
        return false;
    l_file->read(m_data.data(), m_data.size());
    m_data.resize(l_file->gcount());
    if (m_data.size() >= 3 && m_data[0] == char(td_uchar(0xef)) &&
        m_data[1] == char(td_uchar(0xbb)) && m_data[2] == char(td_uchar(0xbf)))
    {
        m_bomSkip = 3;
        m_enc = E_UTF8;
    }
    else if (m_data.size() >= 2 && m_data[0] == char(td_uchar(0xff)) && m_data[1] == char(td_uchar(0xfe)))
    {
        m_bomSkip = 2;
        m_enc = E_UTF16LE;
    }
    else if (m_data.size() >= 2 && m_data[0] == char(td_uchar(0xfe)) && m_data[1] == char(td_uchar(0xff)))
    {
        m_bomSkip = 2;
        m_enc = E_UTF16BE;
    }
    m_file = std::move(l_file);
    return true;
}

QMagicFile::Encoding QMagicFile::enc() const { return m_enc; }
void QMagicFile::setEnc(Encoding a) { m_enc = a; }

QString QMagicFile::preview() const
{
    if (m_enc == E_LOCAL8)
    {
        QString l_local8;
        for (auto &a : m_data) l_local8 += QString::fromLocal8Bit(&a, 1);
        return l_local8;
    }
    else if (m_enc == E_UTF8)
        return QString::fromUtf8(m_bomSkip + m_data.data(), int(m_data.size() - m_bomSkip));
    else if (m_enc == E_UTF16LE || m_enc == E_UTF16BE)
    {
        decltype(m_data) l_data = { char(td_uchar(0xff)), char(td_uchar(0xfe)) };
        if (m_enc == E_UTF16BE) std::swap(l_data[0], l_data[1]);
        l_data.insert(l_data.end(), m_data.begin() + m_bomSkip, m_data.end());
        return QString::fromUtf16(reinterpret_cast<ushort *>(l_data.data()), int(l_data.size() / 2));
    }
    return QString::fromLatin1(m_data.data(), int(m_data.size()));
}

void QMagicFile::readInit()
{
    assert(m_file != nullptr);
    m_file->clear();
    m_file->seekg(m_enc == E_UTF8 || m_enc == E_UTF16LE || m_enc == E_UTF16BE ? m_bomSkip : 0);
}
