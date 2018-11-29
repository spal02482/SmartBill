#ifndef FASTBILL_H
#define FASTBILL_H

#include "invoice.h"
#include "product.h"
#include "productview.h"
#include "fastbilldb.h"

#include <QLayout>

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

    void on_addProductPushButton_clicked();

    void on_viewProductPushButton_clicked();

private:
    Ui::FastBill *ui;
    fastbilldb fbdb;
    Invoice* invoice;
    Product* product;
    ProductView* productView;
};

#endif // FASTBILL_H
