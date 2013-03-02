#include "dbutil.h"
#include <QtSql>
#include <QStringList>
#include <QCryptographicHash>
#include <QPixmap>

QString psqlSessionRole()
{
    QString sr = QSqlDatabase::database("default").userName();
    qDebug() << "psqlSessionRole(): " << sr;

    return sr;
}

QString psqlCurrentRole(){
    QSqlQuery query(QSqlDatabase::database("default"));

    query.exec(QString("SELECT current_user"));
    QString result = QString("desconocido");
    if(query.first()){
        result = query.value(0).toString();
    }

    query.finish();

    qDebug() << "psqlCurrentRole(): " << result;
    return result;
}

/**
 * @brief psqlSessionIsAdmin
 * @return true si la sesion es "admin", false en caso contrario
 */
bool psqlSessionIsAdmin(){
    bool sessionIsAdmin = (0==QString::compare(psqlCurrentRole(),"admin"));
    qDebug() << "psqlSessionIsAdmin(): " << sessionIsAdmin;
    return sessionIsAdmin;
}

bool psqlAuthenticate(const QString &user, const QString &pw)
{
    // Evitar inyeccion SQL
    if(!psqlRoleExists(user))
        return false;

    QSqlQuery query(QSqlDatabase::database("default"));

    bool result = false;

    QString encryptedPassword = psqlEncryptPassword(user,pw);
    query.exec(QString("SELECT check_password(%1, %2)")
               .arg(psqlQuoteQueryString(user))
               .arg(psqlQuoteQueryString(encryptedPassword))
               );

    if(query.first()){
        bool ok= false;
        int cnt = query.value(0).toInt(&ok);
        result = ok && cnt == 1;
    }

    query.finish();
    qDebug() << "psqlAuthenticate(): " << result;
    return result;
}



QString psqlEncryptPassword(const QString &role, const QString &pw){

    QCryptographicHash *qch = new QCryptographicHash(QCryptographicHash::Md5);
    qch->addData(QString("%1%2").arg(pw).arg(role).toUtf8());
    QString hash = qch->result().toHex().toLower();
    delete qch;

    return QString("%1%2").arg("md5").arg(hash);
}

QString psqlQuoteQueryString(const QString &str){
    QString s = str;

    s.replace(QString("\\"),QString("\\\\"));
    s.replace(QString("'"),QString("\\'"));
    s.prepend("'");
    s.append("'");

    return s ;
}

bool psqlSetPassword(const QString &role, const QString &pw){

    QString r = role.trimmed();
    QString p = pw.trimmed();

    if ((r.isEmpty())||(p.isEmpty()))
        return false;

    if(!psqlRoleExists(r))
        return false;

    QSqlDatabase::database("default").transaction();
    QSqlQuery query(QSqlDatabase::database("default"));


    QString encryptedPassword = psqlEncryptPassword(r,p);

    query.exec(QString("ALTER ROLE %1 WITH ENCRYPTED PASSWORD '%2'")
               .arg(r)
               .arg(encryptedPassword)
               );

    bool ok = QSqlDatabase::database("default").commit();

    query.finish();

    return ok;
}

QString psqlSessionInfo()
{
    QSqlQuery query(QSqlDatabase::database("default"));
    QString info;

    query.exec(QString("SELECT session_user,"
                       " inet_client_addr(),"
                       " inet_client_port(),"
                       " inet_server_addr(),"
                       " inet_server_port(),"
                       " version();"));

    if (query.first()){
        info = QString("%1@%2:%3 conectado a %4:%5")
                .arg(query.value(0).toString())
                .arg(query.value(1).toString())
                .arg(query.value(2).toString())
                .arg(query.value(3).toString())
                .arg(query.value(4).toString());
    }

    query.finish();
    return info;
}

QString psqlVersion()
{
    QSqlQuery query(QSqlDatabase::database("default"));
    QString version = "Desconocido";

    query.exec(QString("SELECT version();"));

    if (query.first()){
        version = QString("%1").arg(query.value(0).toString());
    }

    query.finish();

    return version;
}

QStringList psqlRoles(){

    QSqlQuery query(QSqlDatabase::database("default"));
    QStringList roles;

    query.exec(QString("SELECT rolname FROM pg_catalog.pg_roles"));

    while (query.next()){
        roles <<  QString("%1").arg(query.value(0).toString());
    }

    query.finish();
    return roles;
}

QString psqlTest(){
    QSqlQuery query(QSqlDatabase::database("default"));
    query.exec(QString("SELECT * FROM nonexistent"));

    return QString("Database text: %1\nDriver text: %2").arg(query.lastError().databaseText()).arg(query.lastError().driverText());

}

bool psqlAddRole(const QString &rolename, const QString &fullname){

    QRegExp reRole(QString("^(\\w+)\\d*$"));

    if(!reRole.exactMatch(rolename))
        return false;

    QSqlDatabase::database("default").transaction();
    QSqlQuery query(QSqlDatabase::database("default"));

    query.prepare(QString("INSERT INTO usuario(role,fullname) VALUES(?,?)"));
    query.bindValue(0,rolename);
    query.bindValue(1,fullname);
    query.exec();

    query.exec(QString("CREATE ROLE %1 LOGIN INHERIT").arg(rolename));

    bool ok = QSqlDatabase::database("default").commit();
    query.finish();

    return  ok;
}

bool psqlDisableRole(const QString &role){
    if(!psqlRoleExists(role))
        return false;

    QSqlDatabase::database("default").transaction();
    QSqlQuery query(QSqlDatabase::database("default"));

    query.prepare(QString("UPDATE usuario SET active=false WHERE role=?"));
    query.bindValue(0,role);
    query.exec();

    query.exec(QString("ALTER ROLE %1 WITH NOLOGIN").arg(role));

    bool ok = QSqlDatabase::database("default").commit();
    query.finish();

    return  ok;
}

bool psqlEnableRole(const QString &role){
    if(!psqlRoleExists(role))
        return false;

    QSqlDatabase::database("default").transaction();
    QSqlQuery query(QSqlDatabase::database("default"));

    query.prepare(QString("UPDATE usuario SET active=true WHERE role=?"));
    query.bindValue(0,role);
    query.exec();

    query.exec(QString("ALTER ROLE %1 WITH LOGIN").arg(role));

    bool ok = QSqlDatabase::database("default").commit();
    query.finish();

    return  ok;
}

bool psqlDelRole(const QString &role){

    if(!psqlRoleExists(role))
        return false;

    QSqlDatabase::database("default").transaction();
    QSqlQuery query(QSqlDatabase::database("default"));

    query.prepare(QString("DELETE FROM usuario WHERE role=?"));
    query.bindValue(0,role);
    query.exec();

    query.exec(QString("DROP ROLE %1").arg(role));

    bool ok = QSqlDatabase::database("default").commit();

    query.finish();

    return ok;

}

bool psqlRoleExists(const QString &role){
    QStringList users = psqlRoles();
    return users.contains(role);
}

bool psqlGrantRole(const QString &grantRole, const QString &role){
    if(!psqlRoleExists(grantRole) || !psqlRoleExists(role))
        return false;

    QSqlQuery query(QSqlDatabase::database("default"));
    query.exec(QString("GRANT %1 TO %2;").arg(grantRole).arg(role));
    bool ok = query.isActive();
    query.finish();

    qDebug() << "psqlGrantRole(" << grantRole << ", " << role << "): " << ok;
///    qDebug() << QString("Database text: %1\nDriver text: %2").arg(query.lastError().databaseText()).arg(query.lastError().driverText());
    return ok;
}

bool psqlRevokeRole(const QString &revokeRole, const QString &role){
    if(!psqlRoleExists(revokeRole) || !psqlRoleExists(role))
        return false;

    QSqlQuery query(QSqlDatabase::database("default"));
    query.exec(QString("REVOKE %1 FROM %2;").arg(revokeRole).arg(role));
    bool ok = query.isActive();
    query.finish();

    qDebug() << "psqlRevokeRole(" << revokeRole << ", " << role << "): " << ok;
///    qDebug() << QString("Database text: %1\nDriver text: %2").arg(query.lastError().databaseText()).arg(query.lastError().driverText());
    return ok;
}

/*
 * Eleva/modifica el rol de la sesion actual
 */
bool psqlSetRole(const QString &role){

    if(!psqlRoleExists(role))
        return false;

    QSqlQuery query(QSqlDatabase::database("default"));
    query.exec(QString("SET ROLE %1;").arg(role));
    bool ok = query.isActive();
    query.finish();
    qDebug() << "psqlSetRole(): " << ok;
    return ok;
}

/*
 * Restaura rol de la sesion
 */
bool psqlResetRole(){
    QSqlQuery query(QSqlDatabase::database("default"));
    query.exec(QString("RESET ROLE;"));
    bool ok = query.isActive();
    query.finish();
    qDebug() << "psqlResetRole(): " << ok;
    return ok;
}



int savePixmap(const QPixmap &pmap){

    if(!pmap.isNull()){
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);

        pmap.save(&buffer,"PNG");

        QString base64 = bytes.toBase64().data();

        QString sha1Hash = QCryptographicHash::hash(bytes, QCryptographicHash::Sha1).toHex();

        QSqlQuery query(QSqlDatabase::database("default"));

        query.prepare("SELECT id FROM pixmap WHERE hash=?");
        query.bindValue(0,sha1Hash);
        query.exec();

        if(query.first()){
            return query.value(0).toInt();
        }

        query.finish();

        query.exec(QString("INSERT INTO pixmap(id,tstamp,userid,base64data,hash) "
                           "VALUES (DEFAULT,DEFAULT,DEFAULT,'%1','%2') RETURNING id").arg(base64).arg(sha1Hash));

        if(query.first()){
            return query.value(0).toInt();
        }
    }

    return -1;
}

QPixmap retrievePixmap(int id){

    QSqlQuery query(QSqlDatabase::database("default"));
    query.exec(QString("SELECT base64data FROM pixmap WHERE id=%1").arg(id));

    if(query.first()){
        QByteArray base64 = query.value(0).toByteArray();
        QByteArray bytes = QByteArray::fromBase64(base64);

        QPixmap pixmap;
        pixmap.loadFromData(bytes,"PNG");
        return pixmap;
    }

    return QPixmap();
}

/*
bool testInsert(){

    QSqlTableModel model(0, QSqlDatabase::database("default"));
    model.setTable("prueba");

    int colId = model.fieldIndex("id");
    model.removeColumn(colId);

    QSqlRecord rec= model.record();

   // rec.setGenerated("id",false);
    rec.setValue("valor", QVariant("algo"));
    rec.setValue("tstamp", QDateTime::currentDateTime().toString(Qt::ISODate));

    bool ok = model.insertRecord(-1,rec);

    qDebug() << model.lastError().databaseText() << "\n" << model.query().lastQuery();

    return ok;
}

*/




