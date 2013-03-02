#include "paramsdialog.h"

#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTimer>
#include <QPushButton>
#include <QSqlRecord>
#include <QMessageBox>

#include <QDebug>

ParamsDialog::ParamsDialog(QWidget* parent) :
    QDialog(parent)
{

    tModel = new QSqlTableModel(this, QSqlDatabase::database("default"));
    tModel->setTable("parameter");
    //tModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    createWidgets();
    layoutWidgets();
    retranslateUi();
    connectSlots();

    QTimer::singleShot(0,this,SLOT(refreshModels()));
}

void ParamsDialog::refreshModels(){
    tModel->select();

    tModel->setHeaderData(0,Qt::Horizontal,tr("Id"),Qt::DisplayRole);
    tModel->setHeaderData(1,Qt::Horizontal,tr("Key"),Qt::DisplayRole);
    tModel->setHeaderData(2,Qt::Horizontal,tr("Value"),Qt::DisplayRole);
    tModel->setHeaderData(3,Qt::Horizontal,tr("Group"),Qt::DisplayRole);
    tModel->setHeaderData(4,Qt::Horizontal,tr("Description"),Qt::DisplayRole);

  //  tView->hideColumn(0);
    tView->resizeColumnsToContents();
    tView->resizeRowsToContents();

}



void ParamsDialog::createWidgets()
{
    tView = new QTableView(this);
    tView->horizontalHeader()->setStretchLastSection(true);
    tView->setSelectionBehavior(QTableView::SelectionBehavior(QTableView::SelectRows));
    tView->setSelectionMode(QTableView::SelectionMode(QTableView::SingleSelection));
//    tView->setEditTriggers(QTableView::NoEditTriggers);

    tView->setModel(tModel);

    btnNew = new QPushButton(this);
    btnEdit = new QPushButton(this);
    btnEdit->setEnabled(false);
    btnDelete = new QPushButton(this);
    btnDelete->setEnabled(false);
    btnClose = new QPushButton(this);

}

void ParamsDialog::layoutWidgets()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *btnLayout = new QVBoxLayout;

    btnLayout->addWidget(btnNew);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDelete);
    btnLayout->addStretch();
    btnLayout->addWidget(btnClose);

    mainLayout->addWidget(tView);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);
}

void ParamsDialog::retranslateUi()
{
    setWindowTitle(tr("Parameters"));
    btnNew->setText(tr("&New"));
    btnEdit->setText(tr("&Edit"));
    btnDelete->setText(tr("&Remove"));
    btnClose->setText(tr("Close"));
}

void ParamsDialog::connectSlots()
{

    connect(btnNew, SIGNAL(clicked()), this, SLOT(newRecord()));
    connect(btnDelete, SIGNAL(clicked()), this, SLOT(deleteRecord()));
    connect(btnEdit, SIGNAL(clicked()), this, SLOT(editRecord()));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(reject()));
    connect(tView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_tView_activated()) );



    connect(tModel, SIGNAL(beforeDelete(int)), this, SLOT(beforeDelete(int)));
    connect(tModel, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(beforeInsert(QSqlRecord&)));
    connect(tModel, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(beforeUpdate(int,QSqlRecord&)));
    connect(tModel, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(primeInsert(int,QSqlRecord&)));

}

void ParamsDialog::on_tView_activated()
{
    QModelIndex index = tView->currentIndex();

    btnEdit->setEnabled(index.isValid());
    btnDelete->setEnabled(index.isValid());

}

void ParamsDialog::newRecord()
{
    int rowCount = tModel->rowCount();
    tModel->insertRow(rowCount);
    QModelIndex index = tModel->index(rowCount, 1);
    tView->setCurrentIndex(index);
    tView->edit(index);
}

void ParamsDialog::deleteRecord()
{
    QModelIndex index = tView->currentIndex();

    if(!index.isValid()){
        return;
    }

    int row = index.row();

    QSqlRecord rec = tModel->record(row);
    QString clave = rec.value("clave").toString();
    QString valor = rec.value("valor").toString();

    if(clave.isEmpty()) clave = tr("(en blanco)");
    if(valor.isEmpty()) valor = tr("(en blanco)");

    if(QMessageBox::Yes == QMessageBox::question(this,tr("Confirmacion"), tr("Eliminar el registro?\n%1 : %2").arg(clave, valor), QMessageBox::Yes|QMessageBox::No)){
        tModel->removeRow(index.row());
        tModel->submit();
    }
}

void ParamsDialog::editRecord()
{

    QModelIndex index = tView->currentIndex();

    if(!index.isValid()){
        return;
    }

    tView->setCurrentIndex(index);
    tView->edit(index);
}



void ParamsDialog::beforeDelete(int i)
{
    qDebug() << "Before delete: " << i;
}

void ParamsDialog::beforeInsert( QSqlRecord& r)
{
    qDebug() << "Before insert: " << r.value(1).toString() << r.value(2).toString() <<  r.value(3).toString() <<  r.value(4).toString();
}

void ParamsDialog::primeInsert(int i, QSqlRecord& r)
{
    qDebug() << "Prime insert: [" << i << "] " << r.value(1).toString() << r.value(2).toString() <<  r.value(3).toString() <<  r.value(4).toString();
}

void ParamsDialog::beforeUpdate(int i, QSqlRecord& r)
{
    qDebug() << "Before update: [" << i << "] " << r.value(1).toString() << r.value(2).toString() <<  r.value(3).toString() <<  r.value(4).toString();
}

