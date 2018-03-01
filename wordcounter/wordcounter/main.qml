import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import io.qt.examples.magictextproc 1.0

ApplicationWindow
{
    width: 640
    height: 480
    visible: true

    QBuildConfig { id: id_qbc }
    title: id_qbc.decor("Word counter")

    property var m_page1;
    property var m_page2;

    SwipeView
    {
        id: id_sw_main
        anchors.fill: parent

        Page1 {}
        Page2 {}
    }
}
