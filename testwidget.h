#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>

class QPushButton;
class QTableView;
class QSqlTableModel;

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = 0);
    ~TestWidget();
    
signals:
    
public slots:

private slots:

    void retranslateUi();

private:
    QPushButton *btnOk;
    QPushButton *btnNew;
    QTableView *tbl1;
    QSqlTableModel *model;

    void createWidgets();
    void layoutWidgets();
    void connectSlots();
    
};

#endif // TESTWIDGET_H
