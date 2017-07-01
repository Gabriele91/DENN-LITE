import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.0
import QtBasicProcess 1.0
import QtQmlStringUtils 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    width: 1280
    height: 740
    title: qsTr("Denn")

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
    }

    BasicProcess {
        id: processDenn

        function startDenn(type, args) {
            processDenn.start(applicationDirPath + "/DENN-"+type, args);
        }

        function stopDenn() {
            processDenn.forceClose()
        }

        function processDennOutput(fun){
            processDenn.readyReadStandardOutput.connect(fun);
        }

        function processDennError(fun){
            processDenn.readyReadStandardError.connect(fun);
        }

        function processDennTermination(fun){
            processDenn.finished.connect(fun);
        }
    }

    SwipeView {
        id: swipeView
        Layout.fillWidth : true
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PageConfigure {
            id: pageConfigure
            Layout.fillWidth: true
            Component.onCompleted: {
                updateList("float",mutation,mutationItems,"-mlist")
                updateList("float",crossover,crossoverItems,"-colist")
                connectEventOfDatasetSelection()
            }
            function updateList(type, comboBox, comboBoxModel, nameOfList){
                //start
                var process = Qt.createQmlObject(
                     "import QtBasicProcess 1.0; BasicProcess {}"
                    , appWindow
                    , "dynamicPath"
                )
                process.readyRead.connect(function () {
                    //read
                    var colist = process.readAll().trim().split(",");
                    for(var id in colist){
                        comboBoxModel.append({ text: colist[id] })
                    }
                    comboBox.currentIndex = 0;
                })
                process.start(applicationDirPath + "/DENN-"+type, [ nameOfList ]);
            }
            function asArguments(){
                var pathDataset = textPathDataset.text.substring("file://".length)
                var args =
                [
                    "-t", generation.text,
                    "-s", subGeneration.text,
                    "-np", numberParents.text,
                    "-f", defaultF.text,
                    "-cr", defaultCR.text,
                    "-jf", jdeF.text,
                    "-jcr", jdeCR.text,
                    "-cmin", clampMin.text,
                    "-cmax", clampMax.text,
                    "-rmin", randMin.text,
                    "-rmax", randMax.text,
                    "-i", pathDataset,
                    "-o", "output.json",
                    "-omp", "0",
                    "-tp", "4"
                ];
                return args
            }
            function connectEventOfDatasetSelection(){
                openPathDataset.clicked.connect(function(){
                    var dialog = Qt.createQmlObject("import QtQuick.Dialogs 1.0
                                        FileDialog {
                                           title: \"Please choose a database file\"
                                           folder: shortcuts.home
                                           selectMultiple: false
                                           nameFilters: [ \"Dataset file (*.gz)\" ]
                                        }"
                                       , appWindow
                                       , "dynamicPath")
                    dialog.accepted.connect(function(){
                        textPathDataset.text = dialog.fileUrls[0]
                    })
                    dialog.visible = true;
                })
            }
        }

        PageExecute {
            Layout.fillWidth: true
            Component.onCompleted: {
                runAndStop.clicked.connect(function()
                {
                    if(processDenn.isRunning()){
                        processDenn.stopDenn();
                        runAndStop.text = "Run"
                        return;
                    }
                    //else
                    runAndStop.text = "Stop"
                    consoleOutput.text = ""
                    processDenn.processDennOutput(function(){
                        var out_string = processDenn.readAllStandardOutput();
                        out_string = out_string.replace(/\t/g,' ')
                        out_string = out_string.replace(/ +(?= )/g,'');
                        //..
                        out_string = StringUtils.removeEndCarriage(consoleOutput.text+out_string)
                        consoleOutput.text = out_string;
                    })
                    processDenn.processDennError(function(){
                        var out_string = processDenn.readAllStandardError();
                        out_string = out_string.replace(/\t/g,' ')
                        out_string = out_string.replace(/ +(?= )/g,'');
                        //..
                        out_string = StringUtils.removeEndCarriage(consoleOutput.text+out_string)
                        consoleOutput.text = out_string;
                    })
                    processDenn.processDennTermination(function(){
                        runAndStop.text = "Run"
                    })
                    processDenn.startDenn("float",pageConfigure.asArguments());

                })
            }
        }
    }


    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Configure")
        }
        TabButton {
            text: qsTr("Execute")
            onClicked: {
                console.log("Args:" + pageConfigure.asArguments());
            }
        }
    }
}
