QT += qml quick

CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../magictextproc/release/ -lmagictextproc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../magictextproc/debug/ -lmagictextproc
else:unix: LIBS += -L$$OUT_PWD/../magictextproc/ -lmagictextproc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../magictextproc/release/libmagictextproc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../magictextproc/debug/libmagictextproc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../magictextproc/release/magictextproc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../magictextproc/debug/magictextproc.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../magictextproc/libmagictextproc.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../textproccore/release/ -ltextproccore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../textproccore/debug/ -ltextproccore
else:unix: LIBS += -L$$OUT_PWD/../textproccore/ -ltextproccore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../textproccore/release/libtextproccore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../textproccore/debug/libtextproccore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../textproccore/release/textproccore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../textproccore/debug/textproccore.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../textproccore/libtextproccore.a
