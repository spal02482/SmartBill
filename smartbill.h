#ifndef SMARTBILL_H
#define SMARTBILL_H

#include "invoice.h"
#include "product.h"
#include "productview.h"
#include "smartbilldb.h"

#include <QLayout>

namespace Ui {
class SmartBill;
}

class SmartBill : public QWidget
{
    Q_OBJECT

public:
    explicit SmartBill(QWidget *parent = nullptr);
    ~SmartBill();

private slots:
    void on_addInvoicePushButton_clicked();

    void on_addProductPushButton_clicked();

    void on_viewProductPushButton_clicked();

private:
    Ui::SmartBill *ui;
    smartbilldb fbdb;
    Invoice* invoice;
    Product* product;
    ProductView* productView;
};

#endif // SMARTBILL_H
