#ifndef PRODUCT_H
#define PRODUCT_H

#include "smartbilldb.h"

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Product;
}

class Product : public QDialog
{
    Q_OBJECT

public:
    explicit Product(smartbilldb& fbdb, QWidget *parent = nullptr);
    ~Product();

    bool validateProduct(QString, double, int);

    QSqlQuery query;

private slots:

    void on_cancelAddProductPushButton_clicked();

    void on_addProductInPushButton_clicked();

private:
    Ui::Product *ui;
};

#endif // PRODUCT_H
