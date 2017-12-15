#include "NRamNodes.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <nodes/FlowScene.hpp>

//using QtNodes::FlowScene;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Set name of application (setting files)
    a.setApplicationName("nram_view_execution");
    a.setOrganizationName("unipg");
    a.setOrganizationDomain("unipg.com");

    //Load style
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    //Open window
    NRamNodes w;
    w.show();

    return a.exec();
}
