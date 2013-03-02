#include "dbinfodialog.h"
#include "dbutil.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QSpinBox>
#include <QTimer>
#include <QHeaderView>
#include <QDialogButtonBox>

#include <QDebug>

DBInfoDialog::DBInfoDialog(QWidget *parent) :
    QDialog(parent)
{
    updateIntervalTimer = new QTimer(this);
    updateIntervalTimer->setInterval(1000);
    userListModel = new ConnectedUserListModel(this);

    createWidgets();
    layoutWidgets();
    retranslateUi();
    connectSlots();

    updateIntervalTimer->start();

}


void DBInfoDialog::createWidgets()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal,this);

    lblUserInfo = new QLabel(this);
    lblVersionInfo = new QLabel(this);

    tvUserList = new QTableView(this);
    tvUserList->setModel(userListModel);
    tvUserList->horizontalHeader()->setStretchLastSection(true);
    tvUserList->setSelectionMode(QAbstractItemView::NoSelection);
    tvUserList->setSelectionBehavior(QAbstractItemView::SelectRows);

    lblUserListTitle = new QLabel(this);
    spinUpdateInterval= new QSpinBox(this);
    spinUpdateInterval->setRange(0,300);
    spinUpdateInterval->setValue(1);
    spinUpdateInterval->setSingleStep(1);
    spinUpdateInterval->setSpecialValueText(tr("nunca"));
    spinUpdateInterval->setKeyboardTracking(true);
    lblUpdateInterval = new QLabel(this);
    lblUpdateInterval->setBuddy(spinUpdateInterval);
}

void DBInfoDialog::layoutWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(lblUserInfo);
    mainLayout->addWidget(lblVersionInfo);

    QHBoxLayout *hLayout1 = new QHBoxLayout;

    hLayout1->addWidget(lblUserListTitle);
    hLayout1->addStretch();
    hLayout1->addWidget(lblUpdateInterval);
    hLayout1->addWidget(spinUpdateInterval);


    mainLayout->addLayout(hLayout1);
    mainLayout->addWidget(tvUserList);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

void DBInfoDialog::retranslateUi()
{
    setWindowTitle(tr("Informacion sobre la conexion"));
    lblUserInfo->setText(tr("Conexion:\n%1\n").arg(psqlSessionInfo()));
    lblVersionInfo->setText(tr("Servidor PostgreSQL:\n%1\n").arg(psqlVersion()));
    lblUserListTitle->setText(tr("Usuarios conectados:"));
    lblUpdateInterval->setText(tr("Actualizar:"));
    spinUpdateInterval->setSuffix(tr(" seg."));
}

void DBInfoDialog::connectSlots()
{
    connect(spinUpdateInterval,SIGNAL(valueChanged(int)),this,SLOT(setUpdateInterval(int)));
    connect(updateIntervalTimer,SIGNAL(timeout()),userListModel, SLOT(update()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(userListModel,SIGNAL(updated()),tvUserList,SLOT(resizeColumnsToContents()));
    connect(userListModel,SIGNAL(updated()),tvUserList,SLOT(resizeRowsToContents()));

}

void DBInfoDialog::setUpdateInterval(int t)
{
    if(t==0){
        updateIntervalTimer->stop();
        return;
    }

    updateIntervalTimer->setInterval(t*1000);
    updateIntervalTimer->start();

}

void ConnectedUserListModel::update()
{
    QString q("SELECT pid,"
              " usename,"
              " datname,"
              " date_trunc('second', now()-backend_start) as conectado,"
              " application_name,"
              " client_addr,"
              " CASE"
              " WHEN state ILIKE 'idle' THEN 'Ocioso'"
              " WHEN state ILIKE 'active' THEN 'Activo'"
              " ELSE state END"
              " FROM pg_stat_activity");

    queryModel->setQuery(q, QSqlDatabase::database("default"));

    emit updated();
}

QVariant ConnectedUserListModel::data(const QModelIndex &idx, int role) const
{

    if (!idx.isValid()){
        return QVariant();
    }

    int col = idx.column();

    if((Qt::TextAlignmentRole==role)){

        switch(col){
        case 6: // estado
            return QVariant(Qt::AlignLeft|Qt::AlignVCenter);
        default:
            return QVariant(Qt::AlignCenter|Qt::AlignVCenter);
        }
    }

    if((Qt::DisplayRole==role)){
        return  queryModel->data(idx,role);
    }

    return QVariant();
}

QVariant ConnectedUserListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((Qt::DisplayRole==role) && (Qt::Horizontal==orientation)){
        if(section<m_horizontalHeaders.length())
            return QVariant(m_horizontalHeaders.at(section));
    }

    if((Qt::TextAlignmentRole==role)){
        return QVariant(Qt::AlignVCenter|Qt::AlignHCenter);
    }

    if((Qt::DecorationRole==role)&&(Qt::Horizontal==orientation)){

        switch(section){
        case 0:
            return QVariant(QIcon(":/resources/pixmaps/params.png"));
        case 1:
            return QVariant(QIcon(":/resources/pixmaps/user.png"));

        default:
            ;
        }
    }

    return QAbstractTableModel::headerData(section,orientation,role);
}


int ConnectedUserListModel::columnCount(const QModelIndex &parent) const
{
    return queryModel->columnCount(parent);
}

int ConnectedUserListModel::rowCount(const QModelIndex &parent) const
{
    return queryModel->rowCount(parent);
}

ConnectedUserListModel::ConnectedUserListModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_horizontalHeaders << tr("PID") << tr("Usuario") << tr("Base de datos") << tr("Duracion") << tr("Aplicacion") << tr("Desde") << tr("Estado") ;
    queryModel = new QSqlQueryModel(this);
}


