#ifndef DBUTIL_H
#define DBUTIL_H

#include <QString>
class QStringList;
class QPixmap;

QString psqlSessionRole();
QString psqlCurrentRole();
bool psqlSessionIsAdmin();
bool psqlRoleExists(const QString &);
QString psqlSessionInfo();
QString psqlQuoteQueryString(const QString &);
QString psqlEncryptPassword(const QString &role, const QString &pw);
QString psqlVersion();
bool psqlAddRole(const QString &,const QString &);
bool psqlDelRole(const QString &);
bool psqlDisableRole(const QString &);
bool psqlEnableRole(const QString &);

bool psqlAuthenticate(const QString &, const QString &);
bool psqlSetPassword(const QString &, const QString &);

bool psqlGrantRole(const QString &, const QString &);
bool psqlRevokeRole(const QString &, const QString &);

bool psqlSetRole(const QString &);
bool psqlResetRole();

QStringList psqlRoles();

int savePixmap(const QPixmap &);
QPixmap retrievePixmap(int);

/// bool testInsert();

QString psqlTest();

#endif // DBUTIL_H
