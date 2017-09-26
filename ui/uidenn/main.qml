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
    height: 760
    title: qsTr("Denn")

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Configure")
        }
        TabButton {
            text: qsTr("Neural Network")
        }
        TabButton {
            text: qsTr("Execute")
            onClicked: {
                console.log("DENN-" + getDennType() + " " + getDennArguments().join(" "));
            }
        }
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

    function getDennArguments(){
        //configuration args
        var args = pageConfigure.asArguments()
        //network args
        var layers = pageNeuralNetwork.getHiddenLayer()
        //to args
        if(layers.length > 0)
        {
            args.push("-hl")
            args.push(layers.join(" "))
        }
        //return
        return args
    }

    function getDennType(){
        return pageNeuralNetwork.getDennType()
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
                    "-i", textPathDataset.text,
                    "-o", "output.json",
                    "-omp", "0",
                    "-tp", "4"
                ];
                //
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
                        // get path
                        var path = StringUtils.toLocalFile(dialog.fileUrl.toString());
                        // unescape html codes like '%23' for '#'
                        textPathDataset.text = decodeURIComponent(path);
                    })
                    dialog.visible = true;
                })
            }
        }

        PageNeuralNetwork {
            //id
            id: pageNeuralNetwork
        }

        PageExecute {
            //id
            id: pageExecute
            //Context
            property var consoleOutputLastContent: 0
            //Info
            Layout.fillWidth: true
            //init
            Component.onCompleted: {
                //update function
                function updateConsoleContent(){
                    //compute
                    var newConsoleOutputLastContent = consoleOutput.height - consoleOutputFlickable.height;
                    //must to be positive
                    newConsoleOutputLastContent = newConsoleOutputLastContent < 0 ? 0 : newConsoleOutputLastContent;
                    //
                    if(newConsoleOutputLastContent != consoleOutputLastContent)
                    if(consoleOutputFlickable.contentY >= consoleOutputLastContent)
                    {
                        consoleOutputLastContent = newConsoleOutputLastContent;
                        consoleOutputFlickable.contentY = consoleOutputLastContent;
                    }
                    //get value
                    return newConsoleOutputLastContent;
                }
                //event run
                runAndStop.clicked.connect(function()
                {
                    if(processDenn.isRunning()){
                        processDenn.stopDenn();
                        runAndStop.text = "Run"
                        return;
                    }
                    //else init context
                    runAndStop.text = "Stop"
                    consoleOutput.text = ""
                    consoleOutputLastContent = updateConsoleContent();
                    //events
                    processDenn.processDennOutput(function(){
                        var out_string = processDenn.readAllStandardOutput();
                        out_string = out_string.replace(/\r\n/g,'\n') //window
                        out_string = out_string.replace(/\t/g,' ')
                        out_string = out_string.replace(/ +(?= )/g,'');
                        //..
                        out_string = StringUtils.removeEndCarriage(consoleOutput.text+out_string)
                        consoleOutput.text = out_string;
                        updateConsoleContent()
                    })
                    processDenn.processDennError(function(){
                        var out_string = processDenn.readAllStandardError();
                        out_string = out_string.replace(/\r\n/g,'\n') //window
                        out_string = out_string.replace(/\t/g,' ')
                        out_string = out_string.replace(/ +(?= )/g,'');
                        //..
                        out_string = StringUtils.removeEndCarriage(consoleOutput.text+out_string)
                        consoleOutput.text = out_string;
                        updateConsoleContent()
                    })
                    processDenn.processDennTermination(function(){
                        runAndStop.text = "Run"
                    })
                    processDenn.startDenn(getDennType(),getDennArguments());

                })
                //event hover/click
                var in_color = Material.accent
                var in_width = 2
                var out_color = Material.foreground
                var out_width = 1
                consoleOutputBackGround.border.color = out_color
                //
                consoleOutputMouseArea.dynamicPressed.connect(function(mouse){
                    consoleOutputBackGround.border.color = in_color
                    consoleOutputBackGround.border.width = in_width
                    mouse.accepted = false
                })
                consoleOutputMouseArea.exited.connect(function(){
                    consoleOutputBackGround.border.color = out_color
                    consoleOutputBackGround.border.width = out_width
                })
            }
        }
    }


}
