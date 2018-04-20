#ifndef DEPOSITEXCHANGECONTRACTDIALOG_H
#define DEPOSITEXCHANGECONTRACTDIALOG_H

#include <QDialog>

namespace Ui {
class DepositExchangeContractDialog;
}

class DepositExchangeContractDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositExchangeContractDialog(QWidget *parent = 0);
    ~DepositExchangeContractDialog();

    void  pop();

    void init();

    void setCurrentAsset(QString _assetSymbol);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::DepositExchangeContractDialog *ui;
};

#endif // DEPOSITEXCHANGECONTRACTDIALOG_H
