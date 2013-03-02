#include "mainwindow.h"
#include "pwchangedialog.h"
#include "dbinfodialog.h"
#include "dbutil.h"
#include "strutil.h"
#include "paramsdialog.h"
#include "roleadmindialog.h"
#include "conndialog.h"
#include "testwidget.h"

#include <QMdiSubWindow>
#include <QStatusBar>
#include <QMenuBar>
#include <QLineEdit>
#include <QInputDialog>
#include <QAction>
#include <QtSql>
#include <QtCore>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    createActions();
    createMenus();
    createWidgets();
    createToolBars();
    createStatusBar();

    setWindowTitle(qApp->applicationName());
    setWindowIcon(QIcon(":/resources/pixmaps/application.png"));

    setupUi();
    retranslateUi();
    connectSlots();

    readSettings();

    connected=false;

    cdlg = new ConnDialog(this);

    connect(cdlg,SIGNAL(databaseConnected()),this,SLOT(handleConnection()));
    connect(cdlg,SIGNAL(rejected()),this,SLOT(close()));

    QTimer::singleShot(0, cdlg, SLOT(show()));



    //qDebug() << unscrambleString(scrambleString(QByteArray("this is a test")));


}


void MainWindow::openSubWindow(QWidget *widget){

    QMdiSubWindow *nSubWin;
    nSubWin = new QMdiSubWindow(mdiArea);
    nSubWin->setAttribute(Qt::WA_DeleteOnClose);
    nSubWin->setWidget(widget);
    nSubWin->show();
    nSubWin->raise();

}

void MainWindow::closeEvent(QCloseEvent *event)
{

    if (!connected || confirmAction(QString::fromUtf8("¿Seguro que quiere salir?"))) {
        mdiArea->closeAllSubWindows();
        if(!mdiArea->subWindowList().isEmpty())
        {
            event->ignore();
        } else {
            writeSettings();
        }
    } else {
        event->ignore();
    }
}

void MainWindow::writeSettings(){
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("state",saveState());
    settings.setValue("geometry",saveGeometry());
    settings.setValue("tabbedMdiViewMode", actSetTabbedMdiViewMode->isChecked());
    settings.endGroup();
}

void MainWindow::readSettings(){
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    actSetTabbedMdiViewMode->setChecked(settings.value("tabbedMdiViewMode",false).toBool());
    settings.endGroup();
}

bool MainWindow::confirmAction(const QString &message)
{
    int ret = QMessageBox::question(this,
                                    QString::fromUtf8("Confirmación"),
                                    message,
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes  );
    return (ret == QMessageBox::Yes);
}

void MainWindow::grabWidget()
{
    QPixmap p;
    p = QPixmap::grabWidget(this);

    int id= savePixmap(p);
    if(-1 != id){
        QMessageBox::information(this,tr("Captura de pantalla"),tr("La pantalla fue capturada. El identificador de la captura es %1").arg(id));
    }else{
        QMessageBox::critical(this,tr("Error - Captura de pantalla"),tr("No se pudo almacenar la captura de pantalla."));
    }

    QPixmap r = retrievePixmap(id);

    if(!r.isNull()){
        QFile f("/home/dschulz/Desktop/screenshot.png");
        if(f.open(QIODevice::WriteOnly)){
            r.save(&f,"PNG");
            f.close();
        }
    }
}

void MainWindow::aboutThisApp()
{
    QMessageBox::about(this,tr("Acerca de %1").arg(qAppName()),
                       tr("<b>%1 version %2 usando Qt %3</b><br /><i>%4</i> <a href='http://%5'>%5</a><br />").arg(qAppName()).arg(qApp->applicationVersion()).arg(qVersion()).arg(qApp->organizationName()).arg(qApp->organizationDomain()));
}

void MainWindow::createWidgets()
{
    mdiArea = new QMdiArea(this);

    setCentralWidget(mdiArea);
}

void MainWindow::connectSlots()
{
    connect(mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateActions()));
    connect(actNextSubWindow,SIGNAL(triggered()),mdiArea,SLOT(activateNextSubWindow()));
    connect(actPrevSubWindow,SIGNAL(triggered()),mdiArea,SLOT(activatePreviousSubWindow()));
    connect(actCascadeSubWindows,SIGNAL(triggered()),mdiArea,SLOT(cascadeSubWindows()));
    connect(actTileSubWindows,SIGNAL(triggered()),mdiArea,SLOT(tileSubWindows()));
    connect(actPwChange,SIGNAL(triggered()),this,SLOT(changePasswordDialog()));
    connect(actCloseSubWindow,SIGNAL(triggered()),this,SLOT(closeSubWindow()));
    connect(actCloseAllSubWindows,SIGNAL(triggered()),this,SLOT(closeAllSubWindows()));
    connect(actSetTabbedMdiViewMode,SIGNAL(toggled(bool)),this,SLOT(setTabbedViewMode(bool)));
    connect(actExit,SIGNAL(triggered()),this, SLOT(close()));
    connect(actAboutThisApp,SIGNAL(triggered()),this,SLOT(aboutThisApp()));
    connect(actParamsSwnd,SIGNAL(triggered()),SLOT(showParamsDialog()));
    connect(actUserAdmin,SIGNAL(triggered()),this,SLOT(showUserAdminDialog()));
    connect(actConInfo, SIGNAL(triggered()),this,SLOT(connInfoDialog()));
    connect(actGrabScr,SIGNAL(triggered()),this,SLOT(grabWidget()));

    connect(actTestSW, SIGNAL(triggered()),this, SLOT(showTestSW()));

}

void MainWindow::createActions()
{

    /// MDI related
    actNextSubWindow = new QAction(this);
    actPrevSubWindow = new QAction(this);
    actCloseSubWindow = new QAction(this);
    actCloseAllSubWindows = new QAction(this);
    actCascadeSubWindows = new QAction(this);
    actTileSubWindows = new QAction(this);
    actSetTabbedMdiViewMode = new QAction (this);
    actSetTabbedMdiViewMode->setCheckable(true);

    /// Misc
    actConInfo = new QAction(this);
    actGrabScr = new QAction(this);
    actGrabScr->setShortcut(Qt::Key_F11);

    actAboutThisApp = new QAction(this);
    actAboutThisApp->setIconVisibleInMenu(true);
    actUserAdmin = new QAction(this);
    actUserAdmin->setIconVisibleInMenu(true);
    actPwChange = new QAction(this);
    actPwChange->setIconVisibleInMenu(true);
    actExit = new QAction(this);
    actExit->setIconVisibleInMenu(true);
    actExit->setShortcut(Qt::Key_Escape);

    /// Subwindows
    actParamsSwnd = new QAction(this);
    actParamsSwnd->setIconVisibleInMenu(true);

    actTestSW = new QAction(this);
    actTestSW->setIconVisibleInMenu(true);

    actPrevSubWindow->setIcon(QIcon(":/resources/pixmaps/previous.png"));
    actNextSubWindow->setIcon(QIcon(":/resources/pixmaps/next.png"));
    actCloseSubWindow->setIcon(QIcon(":/resources/pixmaps/close.png"));
    actCloseAllSubWindows->setIcon(QIcon(":/resources/pixmaps/shutdown.png"));
    actCascadeSubWindows->setIcon(QIcon(":/resources/pixmaps/cascade.png"));
    actTileSubWindows->setIcon(QIcon(":/resources/pixmaps/tile.png"));
    actSetTabbedMdiViewMode->setIcon(QIcon(":/resources/pixmaps/mdimode.png"));

    actPwChange->setIcon(QIcon(":/resources/pixmaps/password.png"));
    actConInfo->setIcon(QIcon(":/resources/pixmaps/info.png"));
    actExit->setIcon(QIcon(":/resources/pixmaps/shutdown.png"));

    actParamsSwnd->setIcon(QIcon(":/resources/pixmaps/params.png"));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Listo.");
}

void MainWindow::createMenus()
{
    mainMenuBar = new QMenuBar(this);
    menuApp = new QMenu(this);
    menuMvmt = new QMenu(this);
    menuReps = new QMenu(this);
    menuRegs = new QMenu(this);
    menuHelp = new QMenu(this);
}

void MainWindow::createToolBars()
{

    mdiToolBar = new QToolBar("Subventanas",this);
    mdiToolBar->setObjectName("mainToolBar");

    mdiToolBar->setIconSize(QSize(16,16));
    addToolBar(Qt::RightToolBarArea, mdiToolBar);
    mdiToolBar->setAllowedAreas(Qt::RightToolBarArea);
    mdiToolBar->setMovable(false);
}

void MainWindow::updateActions()
{
    //qDebug() << "updateActions()";
}

void MainWindow::setupUi()
{
    mdiToolBar->addAction(actCloseSubWindow);
    mdiToolBar->addAction(actCloseAllSubWindows);
    mdiToolBar->addSeparator();
    mdiToolBar->addAction(actPrevSubWindow);
    mdiToolBar->addAction(actNextSubWindow);
    mdiToolBar->addSeparator();
    mdiToolBar->addAction(actCascadeSubWindows);
    mdiToolBar->addAction(actTileSubWindows);
    mdiToolBar->addSeparator();
    mdiToolBar->addAction(actSetTabbedMdiViewMode);

    menuApp->addAction(actParamsSwnd);
    menuApp->addAction(actConInfo);
    menuApp->addAction(actUserAdmin);
    menuApp->addAction(actPwChange);
    menuApp->addSeparator();
    menuApp->addAction(actExit);

    menuRegs->addAction(actTestSW);

    menuHelp->addAction(actGrabScr);
    menuHelp->addAction(actAboutThisApp);


    mainMenuBar->addMenu(menuApp);
    mainMenuBar->addMenu(menuRegs);
    mainMenuBar->addMenu(menuMvmt);
    mainMenuBar->addMenu(menuReps);
    mainMenuBar->addMenu(menuHelp);

    setMenuBar(mainMenuBar);
}



void MainWindow::retranslateUi()
{

    actPrevSubWindow->setText(tr("Ventana anterior"));
    actNextSubWindow->setText(tr("Ventana siguiente"));
    actTileSubWindows->setText(tr("Ventanas en mosaico"));
    actCascadeSubWindows->setText(tr("Ventanas en cascada"));
    actSetTabbedMdiViewMode->setText(tr("Modo vista"));
    actCloseSubWindow->setText(tr("Cerrar subventana"));
    actCloseAllSubWindows->setText(tr("Cerrar todas las subventanas"));

    actPrevSubWindow->setStatusTip(tr("Ventana anterior"));
    actNextSubWindow->setStatusTip(tr("Ventana siguiente"));
    actTileSubWindows->setStatusTip(tr("Ventanas en mosaico"));
    actCascadeSubWindows->setStatusTip(tr("Ventanas en cascada"));
    actSetTabbedMdiViewMode->setStatusTip(tr("Modo vista"));
    actCloseSubWindow->setStatusTip(tr("Cerrar subventana"));
    actCloseAllSubWindows->setStatusTip(tr("Cerrar todas las subventanas"));

    actParamsSwnd->setText(tr("Parametros"));
    actParamsSwnd->setStatusTip(tr("Parametros"));

    actTestSW->setText(tr("Test subventana"));
    actTestSW->setStatusTip(tr("Test subventana"));

    actUserAdmin->setText(tr("Administrar usuarios y permisos"));
    actUserAdmin->setStatusTip(tr("Administrar usuarios, grupos y permisos"));
    actPwChange->setText(tr("Cambiar contrasena"));
    actPwChange->setStatusTip(tr("Cambiar contrasena de el usuario actual"));
    actConInfo->setText(tr("Informacion sobre la conexion"));
    actConInfo->setStatusTip(tr("Detalles sobre la conexion actual al servidor"));

    actExit->setText(tr("Salir"));
    actExit->setStatusTip(tr("Salir de la aplicacion"));

    actAboutThisApp->setText(tr("Acerca de %1").arg(qAppName()));
    actAboutThisApp->setStatusTip(tr("Acerca de esta aplicacion"));

    actGrabScr->setText(tr("Capturar pantalla"));
    actGrabScr->setStatusTip(tr("Capturar pantalla para solicitud de soporte"));

    menuApp->setTitle(tr("Aplicacion"));
    menuMvmt->setTitle(tr("&Movimientos"));
    menuReps->setTitle(tr("&Reportes"));
    menuRegs->setTitle(tr("R&egistro"));
    menuHelp->setTitle(tr("Ayuda"));

}

void MainWindow::changePasswordDialog()
{

    QPointer<PasswordChangeDialog> pwcdlg = new PasswordChangeDialog(this);
    int ret = pwcdlg->exec();

    if (ret == QDialog::Accepted){
        statusBar()->showMessage(tr("Contrasena cambiada"),5000);
    }else{
        statusBar()->showMessage(tr("Ningun cambio"),5000);
    }

    delete pwcdlg;

}

void MainWindow::connInfoDialog()
{
    QPointer<DBInfoDialog> dlg = new DBInfoDialog(this);

    dlg->exec();

    delete dlg;
}

void MainWindow::closeSubWindow()
{
    QMdiSubWindow * aw = mdiArea->activeSubWindow();

    if(aw){
        aw->close();
        qDebug() << "subwindow close() invoked";
    }

    qDebug() << "closeSubWindow() on active subwindow";
}

void MainWindow::closeAllSubWindows()
{
    QList<QMdiSubWindow *> ws = mdiArea->subWindowList();

    foreach(QMdiSubWindow *w, ws){
        if(w)
            w->close();
    }

    // Si ya no hay subwindows abiertos, ofrecer dialogo para salir
    if(mdiArea->subWindowList().isEmpty()){
        close();
    }

}

void MainWindow::setTabbedViewMode(bool enabled)
{    
    mdiArea->setViewMode( (enabled ? QMdiArea::TabbedView : QMdiArea::SubWindowView ));
    actTileSubWindows->setEnabled(!enabled);
    actCascadeSubWindows->setEnabled(!enabled);

}

void MainWindow::showParamsDialog()
{

    QString pw = QInputDialog::getText(this,tr("Verificacion"),tr("Su contrasena?"), QLineEdit::Password);

    if(!(pw.length()>0))
        return;

    if(!authenticateUser(pw)){
        QMessageBox::warning(this,tr("No autorizado"),tr("Contrasena incorrecta."));
        return;
    }

    if(!psqlSetRole(QString("admin"))){
        QMessageBox::warning(this,tr("Privilegios insuficientes"),tr("Su perfil no tiene privilegios administrativos."));
        return;
    }

    QPointer<ParamsDialog> dlg = new ParamsDialog(this);
    dlg->exec();
    delete dlg;

    psqlResetRole();

}

bool MainWindow::authenticateUser(const QString &pw)
{
    return psqlAuthenticate(psqlSessionRole(), pw);
}

void MainWindow::showUserAdminDialog()
{

    QString pw = QInputDialog::getText(this,tr("Verificacion"),tr("Su contrasena?"), QLineEdit::Password);

    if(!(pw.length()>0))
        return;

    if(!authenticateUser(pw)){
        QMessageBox::warning(this,tr("No autorizado"), tr("Contrasena incorrecta."));
        return;
    }

    if(!psqlSetRole(QString("admin"))){
        QMessageBox::warning(this,tr("Privilegios insuficientes"), tr("Su perfil no tiene privilegios administrativos."));
        return;
    }

    if(!psqlSessionIsAdmin()){
        QMessageBox::warning(this,tr("Privilegios insuficientes"), tr("Su perfil no tiene privilegios administrativos."));
        return;
    }

    QPointer<RoleAdminDialog> dlg = new RoleAdminDialog(this);
    dlg->exec();
    delete dlg;

    psqlResetRole();
}

void MainWindow::handleConnection()
{
    connected=true;
    statusBar()->showMessage(psqlSessionInfo());
}

void MainWindow::showTestSW()
{
    openSubWindow(new TestWidget(this));
}
