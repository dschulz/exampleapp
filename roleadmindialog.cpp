#include "roleadmindialog.h"
#include "dbutil.h"
#include <QVBoxLayout>
#include <QTreeView>
#include <QStringListModel>
#include <QSortFilterProxyModel>

#include <QDebug>

RoleAdminDialog::RoleAdminDialog(QWidget *parent) :
    QDialog(parent)
{
    createModels();
    createWidgets();
    setupUi();
    layoutWidgets();
    connectSlots();
    retranslateUi();
}

void RoleAdminDialog::retranslateUi()
{
    setWindowTitle(tr("Administracion de roles"));
}


void RoleAdminDialog::createModels()
{

}

void RoleAdminDialog::createWidgets()
{

}

void RoleAdminDialog::layoutWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(new QTreeView(this));

    setLayout(layout);
}

void RoleAdminDialog::setupUi()
{
}

void RoleAdminDialog::connectSlots()
{

}
