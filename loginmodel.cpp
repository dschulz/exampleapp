#include "loginmodel.h"
#include <QSettings>
#include <QSize>
#include <QDebug>

LoginModel::LoginModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    read();
}

int LoginModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return logins.size();
}

int LoginModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;  //  [ handle | username@host:port/database ]
}

QVariant LoginModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= logins.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        Login login = logins.at(index.row());

        int col = index.column();
        switch (col){
        case 0:
            return login.handle;
        case 1:
            return QString("%1@%2:%3/%4").arg(login.username).arg(login.host).arg(login.port).arg(login.database);

        default:
            return QVariant();
        }
    }

    return QVariant();
}

QVariant LoginModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Descripcion");

        case 1:
            return tr("Conexion");

        default:
            return QVariant();
        }
    }

    return QVariant();

}

Qt::ItemFlags LoginModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsSelectable;  // Probar esto
}


bool LoginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}


bool LoginModel::removeRow(int position, const QModelIndex &index)
{
    beginRemoveRows(index, position, 1);

    if((logins.size()>position) && (position >=0)){
        logins.removeAt(position);
    }

    endRemoveRows();
    save();
    return true;
}

void LoginModel::save()
{

    beginResetModel();
    QSettings settings;
    settings.beginWriteArray("logins");
    for (int i = 0; i < logins.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("handle", logins.at(i).handle);
        settings.setValue("host", logins.at(i).host);
        settings.setValue("port", logins.at(i).port);
        settings.setValue("username", logins.at(i).username);
        settings.setValue("database", logins.at(i).database);
    }

    settings.endArray();
    settings.sync();

    endResetModel();
    read();
}

void LoginModel::read()
{
    logins.clear();

    QSettings settings;
    int size = settings.beginReadArray("logins");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Login login;
        login.handle   = settings.value("handle").toString();
        login.username = settings.value("username").toString();
        login.database = settings.value("database").toString();
        login.host = settings.value("host").toString();
        login.port = settings.value("port").toInt();

        logins.append(login);
    }

    settings.endArray();

    emit updated();

}

bool LoginModel::containsHandle(const QString &handle)const
{
    foreach(Login l, logins){
        if(QString::compare(l.handle,handle,Qt::CaseInsensitive)==0){
            return true;
        }
    }

    return false;
}

void LoginModel::addLogin(const QString &handle, const QString &username, const QString &database, const QString &host, const int port)
{
    if(!containsHandle(handle)){
        Login newLogin;
        newLogin.handle=handle;
        newLogin.username=username;
        newLogin.database=database;
        newLogin.host=host;
        newLogin.port=port;

        logins.append(newLogin);
        save();
    } else {
        int id=0;
        foreach(Login l, logins){
            if(QString::compare(handle,l.handle,Qt::CaseInsensitive)==0){
                logins.removeAt(id);
                Login newLogin;
                newLogin.handle=handle;
                newLogin.username=username;
                newLogin.database=database;
                newLogin.host=host;
                newLogin.port=port;
                logins.append(newLogin);
                save();
                return;
            }
            id++;
        }
    }
}

QList<LoginModel::Login> LoginModel::getList() const
{
    return logins;
}
