#ifndef DBINFODIALOG_H
#define DBINFODIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include <QSqlQueryModel>

class QTableView;
class QLabel;
class QSpinBox;
class QTimer;
class QDialogButtonBox;
class ConnectedUserListModel;

class DBInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DBInfoDialog(QWidget *parent = 0);
    
signals:
    
public slots:



private:
    QLabel *lblVersionInfo;
    QLabel *lblUserInfo;
    QLabel *lblUserListTitle;
    QLabel *lblUpdateInterval;
    QDialogButtonBox *buttonBox;
    QSpinBox *spinUpdateInterval;
    QTableView *tvUserList;

    void createWidgets();
    void layoutWidgets();
    void retranslateUi();
    void connectSlots();

    QTimer *updateIntervalTimer;

    ConnectedUserListModel *userListModel;

 private slots:
    void setUpdateInterval(int);


    
};


class ConnectedUserListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ConnectedUserListModel(QObject *parent);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
    void update();

signals:
    void updated();

private:
    QStringList m_horizontalHeaders;
    QSqlQueryModel *queryModel;

};

#endif // DBINFODIALOG_H
