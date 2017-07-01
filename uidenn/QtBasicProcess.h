#ifndef QTBASICPROCESS_H
#define QTBASICPROCESS_H

#include <QProcess>
#include <QVariant>
#ifdef _WIN32
    //todo
#else
    #include <signal.h>
    #include <sys/wait.h>
    #include <sys/types.h>
#endif

class QtBasicProcess : public QProcess
{

    Q_OBJECT

public:
    QtBasicProcess(QObject *parent = 0) : QProcess(parent) { }

    Q_INVOKABLE void start(const QString &program, const QVariantList &arguments)
    {
        QStringList args;
        // convert QVariantList from QML to QStringList for QProcess
        for (int i = 0; i != arguments.length(); ++i)
        {
            args << arguments[i].toString();
        }
        // start
        QProcess::start(program, args);
    }

    Q_INVOKABLE bool isRunning(){
        return state() != QProcess::NotRunning;
    }

    Q_INVOKABLE void forceClose(){
        //Specify the process is no longer running
        setProcessState(ProcessState::NotRunning);
        //kill
#ifdef _WIN32
        //todo
        assert(0);
#else
        ::kill(pid(), SIGKILL);
        ::waitpid(pid(),NULL,WNOHANG);
#endif
    }

    Q_INVOKABLE QString readAll()
    {
        return QString::fromUtf8( QProcess::readAll() );
    }
    Q_INVOKABLE QString readAllStandardOutput()
    {
        return QString::fromUtf8( QProcess::readAllStandardOutput() );
    }
    Q_INVOKABLE QString readAllStandardError()
    {
        return QString::fromUtf8( QProcess::readAllStandardError() );
    }
};

#endif // QTBASICPROCESS_H
