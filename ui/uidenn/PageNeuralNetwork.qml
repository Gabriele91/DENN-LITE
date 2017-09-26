import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Material 2.2


Item {

    ListModel {
        id: layerModel
    }
    property int sizeBlockLayer: 80
    property int sizePageFooter: 48
    property int sizeFontInputLayer: 15
    property int sizeFontHiddenLayer: 15
    property int sizeFontOutputLayer: 15

    function getHiddenLayer(){
       var outarray = []
       for(var i=0; i!=layerModel.count; ++i )
       {
           outarray.push(layerModel.get(i).text)
       }
       return outarray
    }

    function getDennType(){
        var name = dennType.model[dennType.currentIndex]
        var outname = name.toLowerCase().replace(" ", "-");
        return outname
    }

    Component {
        id: nnFooter
        Column {
            width: parent.width
            Rectangle {
                height: sizeBlockLayer
                width: parent.width
                color: "transparent"
                Button {
                    anchors.centerIn: parent
                    text: "+"
                    onClicked: {
                        var hiddenLayer = { 'text': '100' };
                        layerModel.append(hiddenLayer)
                    }
                }
            }
            Rectangle{
                height: sizeBlockLayer
                width: parent.width
                color: "transparent"
                border.color: Material.accent
                Label {
                    font.pointSize: sizeFontOutputLayer
                    anchors.centerIn: parent
                    text: "Output layer"
                }
            }
        }
    }

    Component {
        id: nnHeader
        Column {
            width: parent.width
            Rectangle{
                height: sizeBlockLayer
                width: parent.width
                color: "transparent"
                border.color: Material.accent
                Label {
                    font.pointSize: sizeFontInputLayer
                    anchors.centerIn: parent
                    text: "Input layer"
                }
            }
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

            Pane {
                id: layerMainPane
                Layout.fillHeight: true
                Layout.fillWidth: true

                ListView {
                    id: layerListView
                    model: layerModel
                    anchors.fill: parent
                    header: nnHeader
                    footer: nnFooter
                    clip: true
                    ScrollBar.vertical: ScrollBar{}

                    delegate: DraggableItem {
                        Rectangle {
                            id: rectangleOfDragItem
                            color: "transparent"
                            height: sizeBlockLayer
                            width: layerListView.width
                            Row {
                                id: textRow
                                anchors.fill: parent
                                Rectangle {
                                    color: "transparent"
                                    width: parent.width / 3
                                    anchors {
                                        top: parent.top
                                        bottom: parent.bottom
                                    }
                                    Label {
                                        anchors.leftMargin: 35
                                        anchors.left: parent.left
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "Hidden Layer"
                                        font.pointSize: sizeFontHiddenLayer
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
                                    TextInput {
                                        anchors.centerIn: parent
                                        text: model.text
                                        font.pointSize: sizeFontHiddenLayer
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        validator: IntValidator{}
                                        onTextChanged: {
                                            model.text = text
                                        }
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
                                        anchors.rightMargin: 35
                                        anchors.right: parent.right
                                        anchors.verticalCenter: parent.verticalCenter
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

            Row {
                height: sizePageFooter
                Layout.fillWidth: true

                Rectangle {
                    color: "transparent"
                    width: parent.width / 3
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                    ComboBox {
                        id: dennType
                        width: 220

                        model:[
                            "Float",
                            "Double",
                            "Long Double"
                        ]

                        anchors {
                            leftMargin: 10
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }

                Rectangle {
                    color: "transparent"
                    width: parent.width / 3
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                }

                Rectangle {
                    color: "transparent"
                    width: parent.width / 3
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                    /*
                    Label {
                        horizontalAlignment: Text.AlignRight
                        id: weightsAmount
                        text: qsTr("Label")
                        anchors {
                            rightMargin: 10
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                    }
                    */
                }
            }
        }
    }
}
