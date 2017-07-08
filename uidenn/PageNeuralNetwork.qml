import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2


Item {

    ListModel {
        id: layerModel
        ListElement {
            text: "10"
        }
        ListElement {
            text: "20"
        }
        ListElement {
            text: "8"
        }
    }

    Pane {

        id: mainPane
        anchors.fill: parent
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 20
        Material.elevation: 6

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Pane {
                id: layerMainPane
                font.weight: Font.Light
                font.pointSize: 14
                Layout.fillHeight: true
                Layout.fillWidth: true

                ScrollView {
                    anchors.fill: parent
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        id: layerListView
                        model: layerModel
                        anchors.fill: parent

                        delegate: DraggableItem {
                            Rectangle {
                                id: rectangleOfDragItem
                                color: "transparent"
                                height: 80
                                width: layerListView.width
                                Row {
                                    id: textRow
                                    anchors.fill: parent
                                    spacing: 6

                                    Rectangle {
                                        color: "transparent"
                                        width: parent.width / 3
                                        anchors {
                                            top: parent.top
                                            bottom: parent.bottom
                                        }
                                        Label {
                                            anchors.left: parent.left
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.leftMargin: 10
                                            text: "Hidden Layer"
                                            font.bold: true
                                            font.pointSize: 15
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                    }

                                    Rectangle {
                                        color: "transparent"
                                        width: parent.width / 3
                                        anchors {
                                            top: parent.top
                                            bottom: parent.bottom
                                        }
                                        TextEdit {
                                            anchors.centerIn: parent
                                            text: model.text
                                            font.bold: true
                                            font.pointSize: 15
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                    }

                                    Rectangle {
                                        color: "transparent"
                                        width: parent.width / 3
                                        anchors {
                                            top: parent.top
                                            bottom: parent.bottom
                                        }
                                        Button {
                                            anchors.right: parent.right
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.rightMargin: 10
                                            onClicked: {
                                                layerModel.remove(index,1);
                                            }
                                            text: "Remove"
                                        }
                                    }
                                }
                            }

                            draggedItemParent: layerMainPane

                            onMoveItemRequested: {
                                layerModel.move(from, to, 1);
                            }
                        }
                    }
                }
           }

            RowLayout {
                width: 100
                height: 100

                ComboBox {
                    id: dennType
                    model:[
                        "Float",
                        "Double",
                        "Long Double"
                    ]
                }

                Label {
                    id: weightsAmount
                    text: qsTr("Label")
                    horizontalAlignment: Text.AlignRight
                    Layout.fillHeight: false
                    Layout.fillWidth: true
                }

            }

        }
    }
}
