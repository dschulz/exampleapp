#ifndef STRUTIL_H
#define STRUTIL_H

class QString;
class QByteArray;

QString formatNumber(const QString &c );
QString capitalizeName(const QString &text);


QByteArray unscrambleString(const QByteArray &);
QByteArray scrambleString(const QByteArray &);


#endif // STRUTIL_H
