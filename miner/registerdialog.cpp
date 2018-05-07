#include "registerdialog.h"
#include "ui_registerdialog.h"

#include "wallet.h"
#include "commondialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    setParent(UBChain::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(251, 251, 254,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}");

    ui->closeBtn->setIconSize(QSize(12,12));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->closeBtn->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                                "QToolButton:hover{background-color:rgb(208,228,255);}");


    setStyleSheet("QToolButton#okBtn,QToolButton#okBtn2{background-color:#5474EB; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                  "QToolButton#okBtn:hover,QToolButton#okBtn2:hover{background-color:#00D2FF;}"
                  "QToolButton#cancelBtn,QToolButton#cancelBtn2{background-color:#00D2FF; border:none;border-radius:10px;color: rgb(255, 255, 255);}"
                  "QToolButton#cancelBtn:hover,QToolButton#cancelBtn2:hover{background-color:#5474EB;}"
                  "QComboBox{    \
                  border: none;\
                  background:transparent;\
                  font-size: 10pt;\
                  font-family: MicrosoftYaHei;\
                  background-position: center left;\
                  color: black;\
                  selection-background-color: darkgray;}"
                  "QLineEdit{border:none;background:transparent;color:#5474EB;font-size:12pt;margin-left:2px;}"
                  "QLineEdit:focus{border:none;}"
                  );
    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    init();

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::pop()
{
    move(0,0);
    exec();
}

void RegisterDialog::init()
{
    ui->okBtn->setEnabled(false);

    QStringList keys = UBChain::getInstance()->getUnregisteredAccounts();
    ui->accountComboBox->addItems(keys);

    ui->stackedWidget->setCurrentIndex(0);
}

void RegisterDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-RegisterDialog")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result == "\"result\":null")
        {
            if(ui->accountComboBox->currentText() != ui->registerNameLineEdit->text())
            {
                // 如果注册的名字和当前名字不一样 先改为注册的名字
                UBChain::getInstance()->postRPC(  "id-change_account_name", toJsonFormat("change_account_name",
                                               QJsonArray() << ui->accountComboBox->currentText() << ui->registerNameLineEdit->text() ));
            }
            else
            {
                UBChain::getInstance()->postRPC(  "id-register_account", toJsonFormat("register_account",
                                                                                      QJsonArray() << ui->accountComboBox->currentText() << true ));
            }

        }
        else if(result.startsWith("\"error\":"))
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr("Wrong password!") + "</font></body>" );
        }

        return;
    }

    if( id == "id-change_account_name")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        if(result.startsWith("\"result\":{\"id\":\"0.0.0\""))
        {
            UBChain::getInstance()->postRPC(  "id-register_account", toJsonFormat("register_account",
                                                                                  QJsonArray() << ui->registerNameLineEdit->text() << true ));
        }

        return;
    }

    if( id == "id-register_account")
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if(result.startsWith("\"result\":{"))
        {
            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Register transaction has been sent out!"));
            commonDialog.pop();
        }

        return;
    }

    if( id.startsWith("id-get_account-") )
    {
        QString result = UBChain::getInstance()->jsonDataValue(id);

        // 如果跟当前输入框中的内容不一样，则是过时的rpc返回，不用处理
        if( id.mid(QString("id-get_account-").size()) != ui->registerNameLineEdit->text())  return;
;

        if( result.startsWith("\"result\":{\"id\":\"0.0.0\""))
        {
            ui->okBtn->setEnabled(true);
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#2be683>" + tr( "The name is available") + "</font></body>" );
        }
        else
        {
            ui->okBtn->setEnabled(false);
            ui->tipLabel->setText("<body><font style=\"font-size:12px\" color=#ff224c>" + tr( "This name has been used") + "</font></body>" );
        }

        return;
    }
}

void RegisterDialog::on_closeBtn_clicked()
{
    close();
}

void RegisterDialog::on_okBtn_clicked()
{
    if(ui->accountComboBox->currentText().isEmpty())    return;

    if(ui->accountComboBox->currentText() != ui->registerNameLineEdit->text())
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText(tr("The name that will be registered is not the same as current name. Sure to change the name of this account?"));
        if(commonDialog.pop())
        {
            ui->stackedWidget->setCurrentIndex(1);
        }
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
    }

}

void RegisterDialog::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    ui->addressLabel->setText(UBChain::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText()).address);
    ui->registerNameLineEdit->setText(ui->accountComboBox->currentText());
}

void RegisterDialog::on_okBtn2_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())  return;

    UBChain::getInstance()->postRPC( "id-unlock-RegisterDialog", toJsonFormat( "unlock", QJsonArray() << ui->pwdLineEdit->text()
                                               ));
}

void RegisterDialog::on_cancelBtn_clicked()
{
    close();
}

void RegisterDialog::on_registerNameLineEdit_textChanged(const QString &arg1)
{
    UBChain::getInstance()->postRPC( "id-get_account-" + ui->registerNameLineEdit->text(),
                                     toJsonFormat( "get_account", QJsonArray() << ui->registerNameLineEdit->text() ));
}

void RegisterDialog::on_cancelBtn2_clicked()
{
    close();
}