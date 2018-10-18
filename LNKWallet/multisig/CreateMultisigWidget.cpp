#include "CreateMultisigWidget.h"
#include "ui_CreateMultisigWidget.h"

#include "wallet.h"
#include "AddPubKeyDialog.h"
#include "ConfirmCreateMultiSigDialog.h"
#include "commondialog.h"

CreateMultisigWidget::CreateMultisigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateMultisigWidget)
{
    ui->setupUi(this);

    connect( HXChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->pubKeyTableWidget->installEventFilter(this);
    ui->pubKeyTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->pubKeyTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pubKeyTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->pubKeyTableWidget->setFrameShape(QFrame::NoFrame);
    ui->pubKeyTableWidget->setMouseTracking(true);
    ui->pubKeyTableWidget->setShowGrid(false);//隐藏表格线

    ui->pubKeyTableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->pubKeyTableWidget->horizontalHeader()->setFixedHeight(40);
    ui->pubKeyTableWidget->horizontalHeader()->setVisible(true);
    ui->pubKeyTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->pubKeyTableWidget->setColumnWidth(0,530);
    ui->pubKeyTableWidget->setColumnWidth(1,100);
    ui->pubKeyTableWidget->horizontalHeader()->setStretchLastSection(true);

    QRegExp regx("[0-9]{2}");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->requiredLineEdit->setValidator( validator );

    showPubKeys();
}

CreateMultisigWidget::~CreateMultisigWidget()
{
    delete ui;
}

void CreateMultisigWidget::showPubKeys()
{
    int size = accountPubKeyVector.size();
    ui->pubKeyTableWidget->setRowCount(0);
    ui->pubKeyTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        ui->pubKeyTableWidget->setItem( i, 0, new QTableWidgetItem( accountPubKeyVector.at(i).pubKey));
        ui->pubKeyTableWidget->setItem( i, 1, new QTableWidgetItem( tr("delete")));
    }

    ui->totalNumLabel->setText(QString("/ %1").arg(size));
    on_requiredLineEdit_textEdited(ui->requiredLineEdit->text());
}

void CreateMultisigWidget::jsonDataUpdated(QString id)
{
    if( id == "CreateMultisigWidget-create_multisignature_address" )
    {
        QString result = HXChain::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        result.prepend("{");
        result.append("}");
        QJsonObject object = QJsonDocument::fromJson(result.toUtf8()).object();
        QString pubKey = object.value("result").toString();
        HXChain::getInstance()->configFile->setValue("/multisigAddresses/" + pubKey, 1);
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Multi-sig address created: %1").arg(pubKey));
        commonDialog.pop();

        return;
    }
}

void CreateMultisigWidget::on_addPubKeyBtn_clicked()
{
    AddPubKeyDialog addPubKeyDialog;
    addPubKeyDialog.pop();
    if(!addPubKeyDialog.pubKey.isEmpty())
    {
        AccountPubKey apk;
        apk.pubKey = addPubKeyDialog.pubKey;
        if(!accountPubKeyVector.contains(apk))
        {
            accountPubKeyVector.append(apk);
            showPubKeys();
        }
    }
}

void CreateMultisigWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(229,226,240),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(229,226,240),Qt::SolidPattern));

    painter.drawRect(rect());
}

void CreateMultisigWidget::on_requiredLineEdit_textEdited(const QString &arg1)
{
    if(ui->requiredLineEdit->text().toInt() < 1)
    {
        ui->requiredLineEdit->setText("1");
        return;
    }

    if( accountPubKeyVector.size() > 0 && ui->requiredLineEdit->text().toInt() > accountPubKeyVector.size())
    {
        ui->requiredLineEdit->setText(QString::number(accountPubKeyVector.size()));
    }
}

void CreateMultisigWidget::on_createBtn_clicked()
{
    ConfirmCreateMultiSigDialog confirmCreateMultiSigDialog;
    if(confirmCreateMultiSigDialog.pop())
    {
        QJsonArray pubKeysArray;
        foreach (const AccountPubKey& pk, accountPubKeyVector)
        {
            pubKeysArray << pk.pubKey;
        }

        HXChain::getInstance()->postRPC( "CreateMultisigWidget-create_multisignature_address",
                                         toJsonFormat( "create_multisignature_address",
                                                       QJsonArray() << confirmCreateMultiSigDialog.account << pubKeysArray
                                                       << ui->requiredLineEdit->text().toInt() << true ));

    }
}

void CreateMultisigWidget::on_pubKeyTableWidget_cellClicked(int row, int column)
{
    if(column == 1)
    {
        QTableWidgetItem* item = ui->pubKeyTableWidget->item(row,0);
        if(item)
        {
            accountPubKeyVector.remove(row);
            showPubKeys();
        }
        return;
    }
}
