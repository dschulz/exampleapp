#include "conndialog.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>
#include <QtWidgets>

#include "loginmodel.h"
#include <QDebug>

ConnDialog::ConnDialog(QWidget *parent) :
    QDialog(parent)
{    
    setWindowIcon(QIcon(":/resources/pixmaps/application.png"));

    loginModel = new LoginModel(this);

    createWidgets();
    connectSlots();
    retranslateUi();
    setIcons();
    layoutWidgets();
    readSettings();

    loginsView->setModel(loginModel);
    loginsView->verticalHeader()->setVisible(false);
    loginsView->resizeColumnsToContents();
    loginsView->resizeRowsToContents();

    linePassword->setFocus();
}


void ConnDialog::writeSettings(){
    QSettings settings;
    settings.beginGroup("ConnDialog");
    settings.setValue("size",size());
    settings.setValue("lastHost",lineHost->text());
    settings.setValue("lastPort",spinPort->text());
    settings.setValue("lastUser",lineUser->text());
    settings.setValue("lastDatabase",lineDatabase->text());
    settings.setValue("lastHandle",lineHandle->text());
    settings.endGroup();
}


void ConnDialog::readSettings(){
    QSettings settings;
    settings.beginGroup("ConnDialog");
    resize(settings.value("size",QSize(420,350)).toSize());
    // Centrar en pantalla
    move(QApplication::desktop()->screen()->rect().center() - rect().center());
    lineHost->setText(settings.value("lastHost","localhost").toString());
    spinPort->setValue(settings.value("lastPort","5432").toInt());
    lineUser->setText(settings.value("lastUser", "usuario1").toString());
    lineDatabase->setText(settings.value("lastDatabase","exampleapp").toString());
    lineHandle->setText(settings.value("lastHandle",tr("Connection 1")).toString());

    settings.endGroup();

}

void ConnDialog::connectSlots(){
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(btnConnect,SIGNAL(clicked()),this,SLOT(connectDatabase()));
    connect(this,SIGNAL(finished(int)),this,SLOT(writeSettings()));

    connect(btnSave,SIGNAL(clicked()),this,SLOT(saveLogin()));
    connect(btnRemove,SIGNAL(clicked()),this,SLOT(removeLogin()));
    connect(loginsView,SIGNAL(clicked(QModelIndex)),this,SLOT(loginItemClicked(QModelIndex)));
    connect(loginsView,SIGNAL(activated(QModelIndex)),this,SLOT(loginItemActivated(QModelIndex)));
    connect(linePassword,SIGNAL(textChanged(QString)),this,SLOT(passwordFieldChanged(QString)));

    connect(loginModel,SIGNAL(updated()),this,SLOT(resizeView()));
}

void ConnDialog::resizeView(){
    loginsView->resizeRowsToContents();
    loginsView->resizeColumnsToContents();
}


void ConnDialog::loginItemClicked(const QModelIndex &idx){
    if(idx.isValid()){
        btnRemove->setEnabled(true);
        copyLoginItem(idx);
        linePassword->setFocus();
        linePassword->selectAll();
        return;
    }
    btnRemove->setEnabled(false);
}


void ConnDialog::copyLoginItem(const QModelIndex &idx){

    QList<LoginModel::Login> logins = loginModel->getList();

    if(logins.size()>0){
        LoginModel::Login l = logins.at(idx.row());

        lineHandle->setText(l.handle);
        lineHost->setText(l.host);
        lineUser->setText(l.username);
        lineDatabase->setText(l.database);
        spinPort->setValue(l.port);
    }
}

void ConnDialog::loginItemActivated(const QModelIndex &idx)
{
    copyLoginItem(idx);

    btnConnect->animateClick(100);

    if(linePassword->text().trimmed().length()>MIN_PASSWORD_LENGTH)
        connectDatabase();
}

void ConnDialog::passwordFieldChanged(const QString &pw)
{
    btnConnect->setEnabled(false);

    if(pw.trimmed().length()>1){
        btnConnect->setEnabled(hasRequiredData());
        return;
    }
}

bool ConnDialog::hasRequiredData() const {
    return ( ( lineHost->text().trimmed().length()>0 )
             && ( lineUser->text().trimmed().length()>0 )
             && ( lineDatabase->text().trimmed().length()>0 )
             ) ;
}

void ConnDialog::connectDatabase(){

    bool ok = false;
    QString dbError;

    btnConnect->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    qApp->processEvents();

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL","default");
        db.setHostName(lineHost->text().trimmed());
        db.setDatabaseName(lineDatabase->text().trimmed());
        db.setUserName(lineUser->text().trimmed());
        db.setPassword(linePassword->text().trimmed());
        db.setPort(spinPort->value());
        db.setConnectOptions();
        db.setConnectOptions(QString("sslmode=require;application_name=%1").arg(qApp->applicationName()));

        ok = db.open();

        dbError = db.lastError().databaseText().trimmed();
    }

    QApplication::restoreOverrideCursor();

    if(ok){
        emit databaseConnected();
        accept();
    }else{

        // Mostrar un diálogo con el error del driver PostgreSQL
        QMessageBox errmsg;
        errmsg.setDetailedText(dbError);
        errmsg.setWindowTitle(tr("Error"));
        errmsg.setTextFormat(Qt::RichText);
        errmsg.setText(tr("<p>Fallo el intento de conexion a la base de datos.</p><b>¿Clave incorrecta?</b>"));
        errmsg.setIcon(QMessageBox::Warning);
        errmsg.exec();

        QSqlDatabase::removeDatabase("default");
    }

    btnConnect->setEnabled(true);
    linePassword->setFocus();
    linePassword->selectAll();
}


void ConnDialog::retranslateUi(){

    lblHandle->setText(QString("<html><b>") + tr("&Descripcion")+ QString("</b>"));
    lblUser->setText(tr("&Usuario"));
    lblPassword->setText(tr("C&ontrasena"));
    lblDatabase->setText(tr("&Base de datos"));
    lblHost->setText(tr("&Servidor"));
    lblPort->setText("&Puerto");

    btnConnect->setText(tr("&Conectar"));
    btnCancel->setText(tr("C&ancelar"));

    btnSave->setToolTip(tr("Guardar"));
    btnSave->setShortcut(QKeySequence("Ctrl+g"));
    btnRemove->setToolTip(tr("Suprimir"));
    btnRemove->setShortcut(QKeySequence("Del"));
}

void ConnDialog::setIcons(){
    btnConnect->setIcon(QIcon(":/resources/pixmaps/connect.png"));
    btnCancel->setIcon(QIcon(":/resources/pixmaps/cancel.png"));
    btnSave->setIcon(QIcon(":/resources/pixmaps/save.png"));
    btnRemove->setIcon(QIcon(":/resources/pixmaps/remove.png"));

}

void ConnDialog::createWidgets(){

    /// Labels
    lblHandle = new QLabel(this);
    lblUser  = new QLabel(this);
    lblPassword = new QLabel(this);
    lblDatabase = new QLabel(this);
    lblHost = new QLabel(this);
    lblPort = new QLabel(this);

    /// Editors
    lineUser = new QLineEdit(this);
    linePassword = new QLineEdit(this);
    linePassword->setEchoMode(QLineEdit::Password);
    linePassword->setAttribute(Qt::WA_InputMethodEnabled,true);
    linePassword->setInputMethodHints(Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

    lineHost = new QLineEdit(this);
    spinPort = new QSpinBox (this); spinPort->setRange(1025,65535);
    lineDatabase = new QLineEdit(this);
    lineHandle = new QLineEdit(this);

    /// ToolButtons
    btnSave = new QToolButton(this);
    btnRemove = new QToolButton(this); btnRemove->setEnabled(false);

    /// Views
    loginsView = new QTableView(this);
    loginsView->setSelectionBehavior(QTableView::SelectRows);
    loginsView->setSelectionMode(QTableView::SingleSelection);
    loginsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    loginsView->setShowGrid(false);
    loginsView->horizontalHeader()->setStretchLastSection(true);

    /// PushButtons
    btnCancel = new QPushButton(this);
    btnConnect = new QPushButton(this); btnConnect->setDefault(true); btnConnect->setEnabled(false);

    lblHandle->setBuddy(lineHandle);
    lblUser->setBuddy(lineUser);
    lblPassword->setBuddy(linePassword);
    lblDatabase->setBuddy(lineDatabase);
    lblHost->setBuddy(lineHost);
    lblPort->setBuddy(spinPort);
}

void ConnDialog::saveLogin(){
    if(loginModel){
        loginModel->addLogin(lineHandle->text(),
                             lineUser->text(),
                             lineDatabase->text(),
                             lineHost->text(),
                             spinPort->value()
                             );
    }
}

void ConnDialog::removeLogin(){
    if(loginModel){
        QModelIndex cur = loginsView->currentIndex();
        loginModel->removeRow(cur.row(),cur);

        loginItemClicked(QModelIndex());
    }
}

void ConnDialog::layoutWidgets()
{

    QFrame * topFrame =new QFrame(this);
    topFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    topFrame->setWindowTitle(tr("Detalles de la conexion"));

    QGridLayout *topLayout = new QGridLayout;
    QVBoxLayout *midLayout = new QVBoxLayout;
    QHBoxLayout *tbLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    int row = 0;
    const int col1 = 0;
    const int col2 = 1;
    const int col3 = 2;
    const int col4 = 3;


    topLayout->addWidget(lblUser,row,col1,1,1,Qt::AlignRight);
    topLayout->addWidget(lineUser,row,col2,1,3);

    row++;
    topLayout->addWidget(lblPassword,row,col1,1,1,Qt::AlignRight);
    topLayout->addWidget(linePassword,row,col2,1,3);

    row++;
    topLayout->addWidget(lblHost,row,col1,1,1,Qt::AlignRight);
    topLayout->addWidget(lineHost,row,col2,1,1);

    topLayout->addWidget(lblPort,row,col3,1,1,Qt::AlignRight);
    topLayout->addWidget(spinPort,row,col4,1,1,Qt::AlignLeft);

    row++;
    topLayout->addWidget(lblDatabase,row,col1,1,1,Qt::AlignRight);
    topLayout->addWidget(lineDatabase,row,col2,1,3);

    row++;
    topLayout->addWidget(lblHandle,row,col1,1,1,Qt::AlignRight);
    topLayout->addWidget(lineHandle,row,col2,1,3);

    tbLayout->addWidget(btnSave);
    tbLayout->addWidget(btnRemove);
    tbLayout->addStretch();

    midLayout->addLayout(tbLayout);
    midLayout->addWidget(loginsView);

    bottomLayout->addStretch(0);
    bottomLayout->addWidget(btnCancel);
    bottomLayout->addWidget(btnConnect);

    topFrame->setLayout(topLayout);
    mainLayout->addWidget(topFrame);
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(bottomLayout);

    setLayout(mainLayout);
}
