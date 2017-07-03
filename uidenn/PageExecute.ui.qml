import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Item {

    property alias consoleOutputScrollView: consoleOutputScrollView
    property alias consoleOutputFlickable: consoleOutputFlickable
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
                    id: consoleOutputScrollView
                    anchors.fill: parent
                    ScrollBar.horizontal.interactive: true
                    ScrollBar.vertical.interactive: true
                    clip: true

                    Flickable {
                        id: consoleOutputFlickable
                        anchors.fill: parent
                        Text {
                            id: consoleOutput
                            font.family: "Verdana"
                            font.italic: true
                            font.bold: true
                            color: "#fff"
                            text: qsTr("")
                            anchors.fill: parent
                            verticalAlignment: Text.AlignTop
                            font.pixelSize: 14
                        }
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
