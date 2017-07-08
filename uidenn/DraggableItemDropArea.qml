/*
 * listviewdragitem
 *
 * An example of reordering items in a ListView via drag'n'drop.
 *
 * Author: Aurélien Gâteau
 * License: BSD
 */
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

DropArea {
    id: root
    property int dropIndex

    Rectangle {
        id: dropIndicator
        anchors {
            left: parent.left
            right: parent.right
            top: dropIndex === 0 ? parent.verticalCenter : undefined
            bottom: dropIndex === 0 ? undefined : parent.verticalCenter
        }
        height: 2
        opacity: root.containsDrag ? 0.8 : 0.1
        color: Material.accent
    }
}
