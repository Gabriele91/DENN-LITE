import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

Item {
    property alias generation: generation
    property alias subGeneration: subGeneration
    property alias numberParents: numberParents

    property alias defaultF: defaultF
    property alias defaultCR: defaultCR

    property alias jdeF: jdeF
    property alias jdeCR: jdeCR

    property alias clampMin: clampMin
    property alias clampMax: clampMax

    property alias randMin: randMin
    property alias randMax: randMax

    property alias mutation: mutation
    property alias crossover: crossover

    property alias mutationItems: mutationItems
    property alias crossoverItems: crossoverItems

    property alias textPathDataset: textPathDataset
    property alias openPathDataset: openPathDataset

    height: 720

    Pane {
        id: configureFrame
        anchors.fill: parent
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 20
        Material.elevation: 6

        ColumnLayout {
            anchors.fill: parent
            ColumnLayout {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    id: textGeneration
                    text: qsTr("Generation")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    Layout.minimumWidth: 200
                    Layout.maximumWidth: 200
                    Layout.fillWidth: true
                }

                TextField {
                    id: generation
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("1000")
                    Layout.fillWidth: true
                    validator: IntValidator {
                    }
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    id: textSubGeneration
                    text: qsTr("Sub Generation")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    Layout.maximumWidth: 200
                    Layout.minimumWidth: 200
                    Layout.fillWidth: true
                }

                TextField {
                    id: subGeneration
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("100")
                    Layout.fillWidth: true
                    validator: IntValidator {
                    }
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    id: textNumberParents
                    text: qsTr("Number Parents")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    Layout.maximumWidth: 200
                    Layout.minimumWidth: 200
                    Layout.fillWidth: true
                }

                TextField {
                    id: numberParents
                    text: qsTr("40")
                    Layout.fillWidth: true
                    validator: IntValidator {
                    }
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            RowLayout {

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                    Label {
                        id: textDefaultF
                        text: qsTr("Default F")
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        Layout.minimumWidth: 200
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: defaultF
                        text: qsTr("0.5")
                        Layout.fillWidth: true
                        validator: DoubleValidator {
                        }
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                    Label {
                        id: textDefaultCR
                        text: qsTr("Default CR")
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        Layout.minimumWidth: 200
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: defaultCR
                        text: qsTr("0.8")
                        Layout.fillWidth: true
                        validator: DoubleValidator {
                        }
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Label {
                        id: textJdeF
                        text: qsTr("JDE F")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: jdeF
                        text: qsTr("0.1")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        validator: DoubleValidator {
                        }
                        Layout.fillWidth: true
                    }
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Label {
                        id: textJdeCR
                        text: qsTr("JDE CR")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: jdeCR
                        text: qsTr("0.1")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        validator: DoubleValidator {
                        }
                        Layout.fillWidth: true
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Label {
                        id: textClampMin
                        text: qsTr("Clamp min")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: clampMin
                        text: qsTr("-0.5")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        validator: DoubleValidator {
                        }
                        Layout.fillWidth: true
                    }
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Label {
                        id: textClampMax
                        text: qsTr("Clamp max")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: clampMax
                        text: qsTr("0.5")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        validator: DoubleValidator {
                        }
                        Layout.fillWidth: true
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Label {
                        id: textRandMin
                        text: qsTr("Random initialization range min")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        Layout.maximumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: randMin
                        text: qsTr("-1.0")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        validator: DoubleValidator {
                        }
                        Layout.fillWidth: true
                    }
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Label {
                        id: textRandMax
                        text: qsTr("Random initialization range max")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        Layout.maximumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: randMax
                        text: qsTr("1.0")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        validator: DoubleValidator {
                        }
                        Layout.fillWidth: true
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                    Label {
                        id: textMutation
                        text: qsTr("Mutation")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        verticalAlignment: Text.AlignVCenter
                    }

                    ComboBox {
                        id: mutation
                        Layout.fillWidth: true
                        model: ListModel {
                            id: mutationItems
                        }
                    }
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                    Label {
                        id: textCrossover
                        text: qsTr("Crossover")
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        horizontalAlignment: Text.AlignLeft
                        Layout.maximumWidth: 200
                        verticalAlignment: Text.AlignVCenter
                    }

                    ComboBox {
                        id: crossover
                        Layout.fillWidth: true
                        model: ListModel {
                            id: crossoverItems
                        }
                    }
                }
            }

            GroupBox {
                id: groupDataset
                width: 200
                height: 200
                padding: 5
                bottomPadding: 5
                rightPadding: 5
                leftPadding: 5
                topPadding: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
                title: qsTr("")

                RowLayout {
                    spacing: 5
                    anchors.fill: parent

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Label {
                        id: textPathDataset
                        width: 80
                        height: 20
                        Layout.fillWidth: true
                        font.pixelSize: 16
                    }

                    Button {
                        id: openPathDataset
                        text: qsTr("Dataset")
                        Layout.fillWidth: false
                    }
                }
            }

            Pane {
                id: paneVoidArea
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
