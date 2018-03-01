// wordcounter.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <magictextproc/qmagictextproc.h>

int main(int a_argc, char *a_argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication l_app(a_argc, a_argv);

    QMagicTextProc::initialize();

    QQmlApplicationEngine l_engine;
    l_engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (l_engine.rootObjects().isEmpty())
        return -1;

    return l_app.exec();
}
