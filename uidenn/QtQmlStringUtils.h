#ifndef QTQMLSTRINGUTILS_H
#define QTQMLSTRINGUTILS_H
#include <QString>
#include <QObject>
#include <QVariant>

class QtQmlStringUtils : public QObject
{

    Q_OBJECT

public:

    explicit  QtQmlStringUtils(QObject* parent = nullptr)
    :QObject(parent)
    {
    }

    Q_INVOKABLE QString simplified(const QString& str)
    {
        return str.simplified();
    }

    Q_INVOKABLE QString removeEndCarriage(const QString& istr)
    {
        QString ostr;
        ostr.resize(istr.size(), QChar(' '));
        //alloc
        int  i=0
           , o = 0
           , n = 0;
        //remove \n -> \r
        for(; i!=istr.size()  ; ++i,++o)
        {
            auto ic = istr[i];
            auto il = i!=istr.size()-1; //<it is not last
            if(ic == QChar('\r') && il){ o = n; continue; }
            if(ic == QChar('\n')      ){ n = i;           }
            ostr[o] = ic;
        }
        //reset size of string
        ostr.resize(o);
        //return ostr
        return ostr;
    }


    Q_INVOKABLE QString trimmed(const QString& str)
    {
        return str.trimmed();
    }
};


#endif // QTQMLSTRINGUTILS_H
