import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Item {

    property alias consoleOutputMouseArea: consoleOutputMouseArea
    property alias consoleOutputBackGround: consoleOutputBackGround
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

                ScrollView {
                    id: consoleOutputScrollView
                    anchors.fill: parent
                    ScrollBar.horizontal.interactive: true
                    ScrollBar.vertical.interactive: true
                    clip: true

                    Flickable {
                        id: consoleOutputFlickable
                        anchors.fill: parent
                        Label {
                            id: consoleOutput
                            font.family: "Verdana"
                            font.italic: true
                            font.bold: true
                            text: qsTr("")
                            rightPadding: 4
                            bottomPadding: 4
                            leftPadding: 4
                            topPadding: 4
                            anchors.fill: parent
                            verticalAlignment: Text.AlignTop
                            font.pixelSize: 14
                        }
                    }

                    background: Rectangle {
                        id: consoleOutputBackGround
                        color: "#00000000"
                        border.width: 1
                        radius: 4
                    }
                }

                MouseAreaDynamicPress {
                    anchors.fill: parent
                    hoverEnabled: true
                    id: consoleOutputMouseArea
                    propagateComposedEvents: true

                    //onPressed: mouse.accepted = false //overload
                    onClicked: mouse.accepted = false
                    onReleased: mouse.accepted = false
                    onDoubleClicked: mouse.accepted = false
                    onPositionChanged: mouse.accepted = false
                    onPressAndHold: mouse.accepted = false
                }
            }

            Button {
                id: runAndStop
                text: qsTr("Run")
            }
        }
    }
}
