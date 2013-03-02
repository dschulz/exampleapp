#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMdiArea>
#include <QToolBar>
#include <QMainWindow>
#include <QMessageBox>

class ConnDialog;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);

public slots:
    void retranslateUi();

protected:
    void closeEvent(QCloseEvent*);

private slots:

    void grabWidget();
    void aboutThisApp();
    void updateActions();
    void changePasswordDialog();
    void connInfoDialog();
    void closeSubWindow();
    void closeAllSubWindows();
    void setTabbedViewMode(bool);
    void showParamsDialog();
    void showUserAdminDialog();
    void handleConnection();

    void showTestSW();

private:
    void openSubWindow(QWidget *);
    bool confirmAction(const QString &);
    bool authenticateUser(const QString &);
    void readSettings();
    void writeSettings();

    void createToolBars();
    void createMenus();
    void createStatusBar();
    void createActions();
    void createWidgets();
    void setupUi();

    void connectSlots();

    bool connected;
    QAction * actNextSubWindow;
    QAction * actPrevSubWindow;
    QAction * actCloseSubWindow;
    QAction * actCloseAllSubWindows;
    QAction * actCascadeSubWindows;
    QAction * actTileSubWindows;
    QAction * actSetTabbedMdiViewMode;

    QAction * actPwChange;
    QAction * actConInfo;

    QAction * actExit;

    QAction * actAboutThisApp;

    QAction * actParamsSwnd;
    QAction * actUserAdmin;
    QAction * actGrabScr;

    QAction * actTestSW;

    QMdiArea * mdiArea;
    QToolBar * mdiToolBar;

    QMenuBar * mainMenuBar;
    QMenu * menuApp;
    QMenu * menuRegs;
    QMenu * menuMvmt;
    QMenu * menuReps;
    QMenu * menuHelp;

    ConnDialog *cdlg;

};

#endif // MAINWINDOW_H
