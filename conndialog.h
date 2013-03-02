#ifndef CONNDIALOG_H
#define CONNDIALOG_H

#include <QDialog>

const int MIN_PASSWORD_LENGTH = 4;

class QLabel;
class QPushButton;
class QToolButton;
class QLineEdit;
class QTableView;
class QSpinBox;
class LoginModel;
class QModelIndex;

class ConnDialog : public QDialog
{
    Q_OBJECT



public:
    explicit ConnDialog(QWidget *parent = 0);



signals:
    void databaseConnected();

public slots:
    void retranslateUi();
    void setIcons();
    void resizeView();

private slots:
    void writeSettings();
    void readSettings();

    void saveLogin();
    void removeLogin();
    void loginItemClicked(const QModelIndex &idx);
    void loginItemActivated(const QModelIndex &idx);
    void copyLoginItem(const QModelIndex &idx);

    void connectDatabase();
    void passwordFieldChanged(const QString &);

private:

    void connectSlots();
    void createWidgets();
    void layoutWidgets();
    bool hasRequiredData()const;



    QTableView *loginsView;

    LoginModel *loginModel;

    QLabel *lblHandle;
    QLabel *lblUser;
    QLabel *lblPassword;
    QLabel *lblDatabase;
    QLabel *lblHost;
    QLabel *lblPort;

    QLineEdit *lineHandle;
    QLineEdit *lineUser;
    QLineEdit *linePassword;
    QLineEdit *lineDatabase;
    QLineEdit *lineHost;
    QSpinBox *spinPort;

    QToolButton *btnSave;
    QToolButton *btnRemove;
    QPushButton *btnConnect;
    QPushButton *btnCancel;

};

#endif // CONNDIALOG_H
