#include <QtGui>
#include <QtSql>
#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "strutil.h"


int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QApplication::setApplicationName("ExampleApp");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("My Business");
    QApplication::setOrganizationDomain("www.example.com.py");


    QTranslator translator;
    translator.load(app.applicationName() + "_" + QLocale::system().name(), ":/ts");
    app.installTranslator(&translator);

    app.setStyleSheet(QString(
                          " QLineEdit,QSpinBox"
                          " { background-color: #fff; color: #808080; }"
                          " QLineEdit:focus,QSpinBox:focus"
                          " { background-color: #FFFACD; color: #222222; }"
                          ));

    QStringList lista = QSqlDatabase::drivers();

    if(!lista.contains("QPSQL"))
    {
        QMessageBox::critical(0,
                              QString("%1 %2").arg(app.applicationName()).arg(app.applicationVersion()),
                              QObject::tr("El controlador PostgreSQL no esta disponible.\nNo es posible continuar."));
        exit(1);
    }


    MainWindow *w = new MainWindow;
    w->show();

    int ret =  app.exec();

    delete w;
    return ret;

}
