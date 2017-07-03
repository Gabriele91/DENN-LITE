import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Item {

    property alias consoleOutput: consoleOutput
    property alias runAndStop: runAndStop

    Pane {

        id: executeFrame
        anchors.fill: parent
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 20
        Material.elevation: 6

        ColumnLayout {
            anchors.fill: parent

            Pane {
                font.weight: Font.Light
                font.pointSize: 14
                Layout.fillHeight: true
                Layout.fillWidth: true
                Material.theme: Material.Dark
                Material.accent: Material.Purple

                ScrollView {
                    anchors.fill: parent
                    ScrollBar.horizontal.interactive: true
                    ScrollBar.vertical.interactive: true
                    clip: true

                    TextEdit {
                        font.family: "Verdana"
                        font.italic: true
                        font.bold: true
                        color: "#fff"
                        id: consoleOutput
                        text: qsTr("")
                        readOnly: true
                        cursorVisible: true
                        verticalAlignment: Text.AlignTop
                        anchors.fill: parent
                        font.pixelSize: 12
                    }
                }
            }

            Button {
                id: runAndStop
                text: qsTr("Run")
            }
        }
    }
}
