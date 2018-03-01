import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import io.qt.examples.magictextproc 1.0

Item
{
    id: id_i_1st

    property var m_magicFile;

    function procOk()
    {
        id_b_chooseFile.enabled = true;
        id_cb_encoding.enabled = true;
        id_b_go.enabled = true;
    }

    ColumnLayout
    {
        anchors.fill: parent

        Item { height: 0.1 }

        RowLayout
        {
            Item { width: 0.1 }

            TextField
            {
                id: id_tf_filepath
                Layout.fillWidth: true
                readOnly: true
            }

            Button
            {
                id: id_b_chooseFile
                text: "Choose file..."

                onClicked: { id_fd_chooseFile.visible = true; }
            }

            Item { width: 0.1 }
        }

        RowLayout
        {
            Item { width: 0.1 }

            TextArea
            {
                id: id_ta_preview

                Layout.fillWidth: true
                Layout.fillHeight: true

                readOnly: true
                cursorVisible: false
                font.family: "Courier"

                background: Rectangle
                {
                    border.color: "lightgray"
                    border.width: 1
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

            ComboBox
            {
                id: id_cb_encoding
                enabled: false
                model: ["latin1", "local8", "utf8", "utf16le", "utf16be"]
                currentIndex: 1
                onActivated:
                {
                    if (currentIndex == 1) m_magicFile.setEnc(QMagicFile.E_LOCAL8);
                    else if (currentIndex == 2) m_magicFile.setEnc(QMagicFile.E_UTF8);
                    else if (currentIndex == 3) m_magicFile.setEnc(QMagicFile.E_UTF16LE);
                    else if (currentIndex == 4) m_magicFile.setEnc(QMagicFile.E_UTF16BE);
                    else m_magicFile.setEnc(QMagicFile.E_LATIN1);
                    id_ta_preview.text = m_magicFile.preview();
                }
            }

            Button
            {
                id: id_b_go
                enabled: false
                text: "go"

                onClicked:
                {
                    id_b_chooseFile.enabled = false;
                    id_cb_encoding.enabled = false;
                    enabled = false;
                    id_sw_main.currentIndex = 1;
                    m_page2.go(m_magicFile);
                }
            }

            Item { width: 0.1 }
        }

        Item { height: 0.1 }
    }

    FileDialog
    {
        id: id_fd_chooseFile
        title: "Please choose a file..."

        onAccepted:
        {
            id_tf_filepath.text = fileUrl;
            if (typeof m_magicFile !== "undefined") m_magicFile.destroy();
            m_magicFile = Qt.createQmlObject('import io.qt.examples.magictextproc 1.0; QMagicFile {}', id_i_1st);
            if (!m_magicFile.open(fileUrl))
            {
                id_cb_encoding.enabled = false;
                id_b_go.enabled = false;
                id_ta_preview.text = "";
                return;
            }
            var l_enc = m_magicFile.enc();
            if (l_enc === QMagicFile.E_LOCAL8) id_cb_encoding.currentIndex = 1;
            else if (l_enc === QMagicFile.E_UTF8) id_cb_encoding.currentIndex = 2;
            else if (l_enc === QMagicFile.E_UTF16LE) id_cb_encoding.currentIndex = 3;
            else if (l_enc === QMagicFile.E_UTF16BE) id_cb_encoding.currentIndex = 4;
            else id_cb_encoding.currentIndex = 0;
            id_cb_encoding.enabled = true;
            id_b_go.enabled = true;
            id_ta_preview.text = m_magicFile.preview();
        }

        Component.onCompleted:
        {
            folder = shortcuts.home;
        }
    }

    Component.onCompleted: { m_page1 = id_i_1st; }
}
