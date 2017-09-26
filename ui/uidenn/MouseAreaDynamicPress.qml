import QtQuick 2.0

//QTBUG-24477
//released.connect(fun) works as expected,
//but the pressed signal is shadowed by a Boolean property with the same name

MouseArea {
    //overload event
    signal dynamicPressed(var mouse)
    //on press
    onPressed: {
        dynamicPressed(mouse)
    }
}
