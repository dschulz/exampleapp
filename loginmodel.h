#ifndef LOGINMODEL_H
#define LOGINMODEL_H

#include <QAbstractTableModel>

class LoginModel : public QAbstractTableModel
{
    Q_OBJECT



public:

    struct Login {
        QString handle;
        QString username;
        QString database;
        QString host;
        int port;
    };

    explicit LoginModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool removeRow(int position, const QModelIndex &index=QModelIndex());

    QList<Login> getList() const;

    void addLogin(const QString &handle, const QString &username, const QString &database, const QString &host, const int port);
    bool containsHandle(const QString &) const;


signals:
    void updated();

public slots:
    void save();
    void read();

private:

    QList<Login> logins;

};

#endif // LOGINMODEL_H
