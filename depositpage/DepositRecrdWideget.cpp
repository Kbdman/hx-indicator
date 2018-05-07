#include "DepositRecrdWideget.h"
#include "ui_DepositRecrdWideget.h"

#include "wallet.h"

DepositRecrdWideget::DepositRecrdWideget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepositRecrdWideget)
{
    ui->setupUi(this);

    ui->depositRecordTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->depositRecordTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->depositRecordTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->depositRecordTableWidget->setFrameShape(QFrame::NoFrame);
    ui->depositRecordTableWidget->setMouseTracking(true);
    ui->depositRecordTableWidget->setShowGrid(false);//隐藏表格线

    ui->depositRecordTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->depositRecordTableWidget->horizontalHeader()->setFixedHeight(30);
    ui->depositRecordTableWidget->horizontalHeader()->setVisible(true);
    ui->depositRecordTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


//    ui->depositRecordTableWidget->setColumnWidth(0,90);
//    ui->depositRecordTableWidget->setColumnWidth(1,70);
//    ui->depositRecordTableWidget->setColumnWidth(2,100);
//    ui->depositRecordTableWidget->setColumnWidth(3,100);
//    ui->depositRecordTableWidget->setColumnWidth(4,110);
//    ui->depositRecordTableWidget->setColumnWidth(5,70);

    init();
}

DepositRecrdWideget::~DepositRecrdWideget()
{
    delete ui;
}

void DepositRecrdWideget::init()
{

}

void DepositRecrdWideget::showDepositRecord(QString _tunnelAddress)
{
    tunnelAddress = _tunnelAddress;
    TransactionTypeIds typeIds = UBChain::getInstance()->transactionDB.getAccountTransactionTypeIdsByType(_tunnelAddress,TRANSACTION_TYPE_DEPOSIT);

    int size = typeIds.size();
    ui->depositRecordTableWidget->setRowCount(0);
    ui->depositRecordTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        QString transactionId = typeIds.at(size - i - 1).transactionId;
        TransactionStruct ts = UBChain::getInstance()->transactionDB.getTransactionStruct(transactionId);
        if(ts.type == -1)
        {
            qDebug() << "can not find transaction in transactionstruct db: " << transactionId;
            continue;
        }

        QJsonObject object = QJsonDocument::fromJson(ts.operationStr.toLatin1()).object();
        QJsonObject crossChainTxObject  = object.take("cross_chain_trx").toObject();
        QString     crossChainTxId      = crossChainTxObject.take("trx_id").toString();
        QString     amount              = crossChainTxObject.take("amount").toString();
        QString     assetSymbol         = crossChainTxObject.take("asset_symbol").toString();


        ui->depositRecordTableWidget->setItem(i,0, new QTableWidgetItem(QString::number(ts.blockNum)));

        QTableWidgetItem* item = new QTableWidgetItem(amount + " " + assetSymbol);
        ui->depositRecordTableWidget->setItem(i,1, item);
        item->setTextColor(QColor(0,255,0));

        ui->depositRecordTableWidget->setItem(i,2, new QTableWidgetItem(transactionId));

        ui->depositRecordTableWidget->setItem(i,3, new QTableWidgetItem(crossChainTxId));

        ui->depositRecordTableWidget->setItem(i,4, new QTableWidgetItem(tr("confirmed")));

    }

}

void DepositRecrdWideget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));
    painter.drawRect(0,0,770,530);
}
