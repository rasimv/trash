#ifndef __QMAGICTEXTPROC_H
#define __QMAGICTEXTPROC_H

#include <array>
#include <functional>
#include <QObject>
#include "aux8bit.h"
#include "qmagicfile.h"
#include <textproccore/textproc.h>

class QBuildConfig : public QObject
{
    Q_OBJECT
public: Q_INVOKABLE QString decor(const QString &a);
};

class QMagicTextProc : public QObject
{
    Q_OBJECT
    Q_ENUMS(StatIndex)

public:
    enum StatIndex { SI_UNIQ_COUNT = 0, SI_LINE_COUNT, SI_WORD_COUNT };

    QMagicTextProc(QObject *a = nullptr);
    ~QMagicTextProc();

    QMagicTextProc(const QMagicTextProc &) = delete;
    QMagicTextProc &operator =(const QMagicTextProc &) = delete;

    Q_INVOKABLE void go(QMagicFile *a);
    Q_INVOKABLE QStringList notSyncStat();

    static void initialize();

signals:
    void doneIntermed();
    void done();

    private slots:
    void onDoneIntermed();

private:
    TextProc::EncParams makeLatin1();
    TextProc::EncParams makeLocal8();
    TextProc::EncParams makeUtf(TextProc::Encoding a_enc);

    void callbackDone(void *a);

    std::unique_ptr<TextProc> m_core;
    static aux8bit::S8bit s_latin1, s_local8;
};

#endif //__QMAGICTEXTPROC_H
