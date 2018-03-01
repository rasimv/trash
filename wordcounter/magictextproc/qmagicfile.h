#ifndef __QMAGICFILE_H
#define __QMAGICFILE_H

#include <iostream>
#include <memory>
#include <QObject>

class QMagicFile : public QObject
{
    Q_OBJECT
    Q_ENUMS(Encoding)

    friend class QMagicTextProc;

public:
    typedef unsigned char td_uchar;
    enum Encoding { E_LATIN1 = 0, E_LOCAL8, E_UTF8, E_UTF16LE, E_UTF16BE };

    explicit QMagicFile(QObject *a = nullptr);
    virtual ~QMagicFile();

    QMagicFile(const QMagicFile &) = delete;
    QMagicFile &operator =(const QMagicFile &) = delete;

    Q_INVOKABLE bool open(const QString &a_fileUrl);
    Q_INVOKABLE Encoding enc() const;
    Q_INVOKABLE void setEnc(Encoding a);
    Q_INVOKABLE QString preview() const;

private:
    void readInit();

    Encoding m_enc;
    std::vector<char> m_data;
    size_t m_bomSkip;
    std::shared_ptr<std::istream> m_file;
};

#endif //__QMAGICFILE_H
