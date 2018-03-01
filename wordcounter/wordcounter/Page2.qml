import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import io.qt.examples.magictextproc 1.0

Item
{
    id: id_i_2nd

    property var m_magicTextProc;

    function go(a_magicFile)
    {
        m_magicTextProc = Qt.createQmlObject(
            'import io.qt.examples.magictextproc 1.0
            QMagicTextProc
            {
                onDone:
                {
                    id_t_main.running = false;
                    statistics();
                    id_b_okOrCancel.text = "ok";
                }
            }'
            , id_i_2nd);
        m_magicTextProc.go(a_magicFile);
        id_b_okOrCancel.enabled = true;
        id_t_main.running = true;
    }

    function statistics()
    {
        if (typeof m_magicTextProc === "undefined") return;
        var l_list = m_magicTextProc.notSyncStat();
        id_l_lineCount.text = l_list[QMagicTextProc.SI_LINE_COUNT];
        id_l_uniqCount.text = l_list[QMagicTextProc.SI_UNIQ_COUNT];
        id_l_wordCount.text = l_list[QMagicTextProc.SI_WORD_COUNT];
    }

    function zeroStat()
    {
        id_l_lineCount.text = "0";
        id_l_uniqCount.text = "0";
        id_l_wordCount.text = "0";
    }

    Timer
    {
        id: id_t_main
        interval: 30; running: false; repeat: true
        onTriggered: { statistics(); }
    }

    ColumnLayout
    {
        anchors.fill: parent

        Item { height: 0.1 }

        RowLayout
        {
            Item { width: 0.1 } Item { width: 0.1 }

            Label
            {
                text: "Statistics"
            }
        }

        ToolSeparator { orientation: Qt.Horizontal; Layout.fillWidth: true }

        RowLayout
        {
            Item { width: 0.1 } Item { width: 0.1 }

            ColumnLayout
            {
                RowLayout
                {
                    ColumnLayout
                    {
                        Label { text: "Line count:" }
                        Label { text: "Word count:" }
                        Label { text: "Total word count:" }
                    }

                    ColumnLayout
                    {
                        Label { id: id_l_lineCount; text: "0" }
                        Label { id: id_l_uniqCount; text: "0" }
                        Label { id: id_l_wordCount; text: "0" }
                    }
                }

                Item
                {
                    Layout.fillHeight: true
                }
            }

            Item { width: 0.1 }
        }

        RowLayout
        {
            Item
            {
                Layout.fillWidth: true
            }

            Button
            {
                id: id_b_okOrCancel
                enabled: false
                text: "cancel"

                onClicked:
                {
                    enabled = false;
                    text = "cancel";
                    zeroStat();

                    var l_null;
                    var l_magicTextProc = m_magicTextProc;
                    m_magicTextProc = l_null;
                    l_magicTextProc.destroy();

                    id_sw_main.currentIndex = 0;
                    m_page1.procOk();
                }
            }

            Item { width: 0.1 }
        }

        Item { height: 0.1 }
    }

    Component.onCompleted: { m_page2 = id_i_2nd; }
}
