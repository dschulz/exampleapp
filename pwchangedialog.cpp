#include <QtWidgets>
#include <QtSql>
#include <QtDebug>
#include "pwchangedialog.h"


PasswordChangeDialog::PasswordChangeDialog(QWidget *parent, const QString &role) :
    QDialog(parent), m_role(role)
{

    setWindowIcon(QIcon(":/resources/pixmaps/password.png"));

    createWidgets();
    doLayout();
    retranslateUi();
    connectSlots();

    bool isAdmin = psqlSessionIsAdmin();

    if(isAdmin){
        lblCurPas->setEnabled(!isAdmin);
        lineCurPas->setEnabled(!isAdmin);
        lineCurPas->setText(tr("*************"));
    }

}

void PasswordChangeDialog::createWidgets()
{
    lblCurPas = new QLabel(this);
    lblPass1 = new QLabel(this);
    lblPass2 = new QLabel(this);
    lineCurPas = new QLineEdit(this);
    linePass1 = new QLineEdit(this);
    linePass2 = new QLineEdit(this);
    lineCurPas->setEchoMode(QLineEdit::Password);
    linePass1->setEchoMode(QLineEdit::Password);
    linePass2->setEchoMode(QLineEdit::Password);
    // Workaround
    lineCurPas->setAttribute(Qt::WA_InputMethodEnabled,true);
    lineCurPas->setInputMethodHints(Qt::ImhNoPredictiveText);
    linePass1->setAttribute(Qt::WA_InputMethodEnabled,true);
    linePass1->setInputMethodHints(Qt::ImhNoPredictiveText);
    linePass2->setAttribute(Qt::WA_InputMethodEnabled,true);
    linePass2->setInputMethodHints(Qt::ImhNoPredictiveText);

    lineCurPas->installEventFilter(this);
    linePass1->installEventFilter(this);
    linePass2->installEventFilter(this);

    lblCurPas->setBuddy(lineCurPas);
    lblPass1->setBuddy(linePass1);
    lblPass2->setBuddy(linePass2);

    btnConfirm = new QPushButton(this);
    btnConfirm->setEnabled(false);
    btnConfirm->setDefault(true);
    btnCancel = new QPushButton(this);
}

void PasswordChangeDialog::retranslateUi()
{
    setWindowTitle(tr("Cambiar contrasena"));
    btnConfirm->setText(tr("Cambiar"));
    btnCancel->setText(tr("Cancelar"));
    lblCurPas->setText(tr("Contrasena actual de <i>%1</i>:").arg(m_role));
    lblPass1->setText(tr("Nueva contrasena para <i>%1</i>:").arg(m_role));
    lblPass2->setText(tr("Confirmar nueva contrasena:"));

    btnCancel->setShortcut(Qt::Key_Escape);
}

void PasswordChangeDialog::doLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;

    layout->addWidget(lblCurPas);
    layout->addWidget(lineCurPas);
    layout->addWidget(lblPass1);
    layout->addWidget(linePass1);
    layout->addWidget(lblPass2);
    layout->addWidget(linePass2);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnConfirm);

    layout->addSpacing(lblPass1->height()/2);
    layout->addLayout(btnLayout);

    layout->setSizeConstraint( QLayout::SetFixedSize );


    setLayout(layout);
}

void PasswordChangeDialog::checkPasswords()
{
    QString p0 = lineCurPas->text().trimmed();
    QString p1 = linePass1->text().trimmed();
    QString p2 = linePass2->text().trimmed();

    int minPasswordLength=4;

    if((p2.length()<minPasswordLength)||(p1.length()<minPasswordLength)){
        btnConfirm->setEnabled(false);
        return;
    }

    if((QString::compare(p1,p2)==0)&&(QString::compare(p0,p1)!=0)){
        btnConfirm->setEnabled(true);
        return;
    }

    btnConfirm->setEnabled(false);
}

void PasswordChangeDialog::connectSlots()
{
    connect(linePass1,SIGNAL(textChanged(QString)),this,SLOT(checkPasswords()));
    connect(linePass2,SIGNAL(textChanged(QString)),this,SLOT(checkPasswords()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(close()));
    connect(btnConfirm,SIGNAL(clicked()),this,SLOT(changePassword()));
}

void PasswordChangeDialog::changePassword()
{
    QString opw = lineCurPas->text().trimmed();
    QString npw = linePass1->text().trimmed();

    if(!psqlSessionIsAdmin()){
        if(!psqlAuthenticate(m_role,opw)){
            QMessageBox::critical(this,tr("Error"),tr("La contrasena que introdujo es incorrecta.\nNingun cambio."));
            linePass1->clear();
            linePass2->clear();
            lineCurPas->selectAll();
            lineCurPas->setFocus();
            checkPasswords();
            return;
        }
    }

    setControlsEnabled(false);
    bool changed = psqlSetPassword(m_role,npw);
    setControlsEnabled(true);

    if(changed){
        QTimer::singleShot(0,this,SLOT(accept()));
    }else{
        QMessageBox::warning(this,tr("Error"),tr("Ocurrio un error intentando cambiar la contrasena.\nPor favor, intenta otra vez."));
    }
}

void PasswordChangeDialog::setControlsEnabled(bool enabled)
{
    btnConfirm->setEnabled(enabled);
    btnCancel->setEnabled(enabled);
    linePass1->setEnabled(enabled);
    linePass2->setEnabled(enabled);
}


bool PasswordChangeDialog::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type()==QEvent::KeyPress){

        if(obj==lineCurPas){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key();
            if((key==Qt::Key_Enter) || (key==Qt::Key_Return)){
                focusNextChild();
            }
        }

        if(obj==linePass1){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key();
            if((key==Qt::Key_Enter) || (key==Qt::Key_Return)){
                focusNextChild();
            }
        }

        if(obj==linePass2){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key();
            if((key==Qt::Key_Enter) || (key==Qt::Key_Return)){
                if(btnConfirm->isEnabled()){
                    btnConfirm->animateClick(200);
                }
            }
        }
    }


    return QWidget::eventFilter(obj,event);
}

