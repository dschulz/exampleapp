#ifndef PWCHANGEDIALOG_H
#define PWCHANGEDIALOG_H

#include <QDialog>
#include "dbutil.h"


class QPushButton;
class QLabel;
class QLineEdit;

class PasswordChangeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordChangeDialog(QWidget *parent = 0, const QString &role = psqlSessionRole());

signals:

public slots:
    void retranslateUi();

protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void checkPasswords();
    void changePassword();
    void setControlsEnabled(bool);

private:
    void createWidgets();
    void doLayout();
    void connectSlots();

    QLabel *lblCurPas;
    QLabel *lblPass1;
    QLabel *lblPass2;

    QString m_role;

    QLineEdit *lineCurPas;
    QLineEdit *linePass1;
    QLineEdit *linePass2;

    QPushButton *btnConfirm;
    QPushButton *btnCancel;

};

#endif // PWCHANGEDIALOG_H
