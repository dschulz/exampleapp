#ifndef PARAMSDIALOG_H
#define PARAMSDIALOG_H

#include <QDialog>

class QSqlTableModel;
class QTableView;
class QPushButton;
class QSqlRecord;

class ParamsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParamsDialog(QWidget* parent = 0);

    
signals:
    
public slots:
    void refreshModels();

    // borrar esto
    void beforeDelete(int);
    void beforeInsert(QSqlRecord &);
    void primeInsert(int, QSqlRecord&);
    void beforeUpdate(int,QSqlRecord&);

private slots:
    void on_tView_activated();
    void newRecord();
    void deleteRecord();
    void editRecord();

private:
    QSqlTableModel* tModel;
    QTableView* tView;

    QPushButton* btnNew;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnClose;

    void createWidgets();
    void layoutWidgets();
    void retranslateUi();
    void connectSlots();
    
};

#endif // PARAMSDIALOG_H
