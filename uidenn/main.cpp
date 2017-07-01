#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include "QtBasicProcess.h"
#include "QtQmlStringUtils.h"


int main(int argc, char *argv[])
{
    //Add QtBasicProcess
    qmlRegisterType<QtBasicProcess>("QtBasicProcess", 1, 0, "BasicProcess");
    qmlRegisterSingletonType<QtQmlStringUtils>("QtQmlStringUtils" , 1, 0,"StringUtils",
    [](QQmlEngine *engine, QJSEngine *scriptEngine)->QObject*
    {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new QtQmlStringUtils();
    });

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    engine.rootContext()->setContextProperty("applicationFilePath", QGuiApplication::applicationFilePath());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
