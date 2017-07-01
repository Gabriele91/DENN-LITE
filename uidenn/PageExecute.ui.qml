import QtQuick 2.7
import QtQuick.Controls 2.0
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

                Frame {
                    id: consoleFrame
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.fill: parent
                    clip: true

                    Text {
                        font.family: "Verdana"
                        font.italic: true
                        font.bold: true
                        id: consoleOutput
                        text: qsTr("")
                        wrapMode: Text.WrapAnywhere
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        font.pixelSize: 12
                        x: -hbar.position * width
                        y: -vbar.position * height
                    }

                    ScrollBar {
                        id: vbar
                        hoverEnabled: true
                        active: hovered || pressed
                        orientation: Qt.Vertical
                        size: consoleFrame.height / consoleOutput.height
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                    }

                    ScrollBar {
                        id: hbar
                        hoverEnabled: true
                        active: hovered || pressed
                        orientation: Qt.Horizontal
                        size: consoleFrame.width / consoleOutput.width
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
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
