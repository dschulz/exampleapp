#include "strutil.h"
#include <QString>
#include <QStringList>
#include <QDebug>

QString formatNumber(const QString &c )
{
    return QString ( "%L1" ).arg (c.toInt());
}


QString capitalizeName(const QString &text)
{
    QString orig=text.toLower();
    QStringList words = orig.split(QRegExp ( "\\W+" ), QString::SkipEmptyParts );
    QString aux;
    int wc=0;

    foreach(QString w, words){
        // if (wc>0) continue;
        aux=w;
        w[0] = w[0].toUpper();

        if (w.toLower()=="de") w = w.toLower();

        orig.replace ( QRegExp ( QString( "\\b(%1)\\b" ).arg ( aux ) ) , w );
        wc++;
    }

    return orig.replace(QRegExp("^\\s*(\\w\\s*)$"), "\\1");
}


QByteArray unscrambleString(const QByteArray &pwd)
{
    if((pwd.length()>0) && (pwd.length() % 2 == 0) ){
        QByteArray bytes = QByteArray::fromHex(pwd);
        char decbyte = bytes[0];
        bytes.remove(0,1);
        QByteArray pbytes;
        pbytes.append(decbyte);
        foreach(char byte , bytes){
            pbytes.append(byte xor decbyte);
        }
        return pbytes;
    }

    return QByteArray();
}

//qDebug() << unscrambleString("3706075650425e5b524556");
//qDebug() << unscrambleString("721d1e1e1b1c1501061d1c1701");
//qDebug() << unscrambleString(scramblePassword(QObject::tr("á")));

QByteArray scrambleString(const QByteArray &pwd)
{

    if(!pwd.isEmpty()){
        QByteArray encString;
        quint16 encByte = pwd.at(0);

        encString.append(encByte);
        for(int i=1; i<pwd.length(); i++){
            quint16 curByte = pwd.at(i);
            encString.append(curByte ^ encByte);
        }

        return encString.toHex();
    }

    return QByteArray();
}

