#include "testwidget.h"
#include <QDebug>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QSqlTableModel>


TestWidget::TestWidget(QWidget *parent) :
    QWidget(parent)
{

    qDebug() << "hello testwidget" ;
    setWindowTitle(tr("Widget de prueba"));
    setMinimumSize(250,250);

    createWidgets();
    layoutWidgets();
    connectSlots();
    retranslateUi();

}

TestWidget::~TestWidget()
{
    qDebug() << "bye testwidget" ;
}

void TestWidget::retranslateUi()
{
    btnOk->setText("Guardar");
    btnNew->setText("Nuevo");
}

void TestWidget::createWidgets()
{
    btnNew=new QPushButton(this);
    btnOk=new QPushButton(this);
    tbl1=new QTableView(this);



    model = new QSqlTableModel(this);

    model->setTable("usuario");


    tbl1->setModel(model);
    model->select();

}

void TestWidget::layoutWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(tbl1);
    mainLayout->addWidget(btnOk);
    mainLayout->addWidget(btnNew);

    setLayout(mainLayout);

}

void TestWidget::connectSlots()
{

    connect(btnOk, SIGNAL(clicked()), model, SLOT(submitAll()));
    //connect(btnNew, SIGNAL(clicked()), tbl1, SLOT()

}
