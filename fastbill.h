#ifndef FASTBILL_H
#define FASTBILL_H

#include "invoice.h"
#include "product.h"

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>


namespace Ui {
class FastBill;
}

class FastBill : public QWidget
{
    Q_OBJECT

public:
    explicit FastBill(QWidget *parent = nullptr);
    ~FastBill();

private slots:
    void on_addInvoicePushButton_clicked();

    void on_closeFastBillPushButton_clicked();

    void on_addProductPushButton_clicked();

private:
    Ui::FastBill *ui;
    fastbilldb fbdb;
};

#endif // FASTBILL_H
