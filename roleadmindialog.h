#ifndef ROLEADMINDIALOG_H
#define ROLEADMINDIALOG_H

#include <QDialog>

class QStringListModel;
class QSortFilterProxyModel;
class QListView;
class QModelIndex;

class RoleAdminDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RoleAdminDialog(QWidget *parent = 0);
    
signals:
    
public slots:
    void retranslateUi();

private:
    void createModels();
    void createWidgets();
    void layoutWidgets();
    void setupUi();
    void connectSlots();





};

#endif // ROLEADMINDIALOG_H
