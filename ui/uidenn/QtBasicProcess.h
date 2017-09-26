#ifndef QTBASICPROCESS_H
#define QTBASICPROCESS_H

#include <QProcess>
#include <QVariant>
#ifdef _WIN32
    #include <Windows.h>
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
#ifdef _WIN32
        //kill
        TerminateProcess(pid()->hProcess,0);

        // Wait until child process exits.
        WaitForSingleObject( pid()->hProcess, INFINITE );

        // Get exit code
        // GetExitCodeProcess( pid()->hProcess, &exit_code );
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
