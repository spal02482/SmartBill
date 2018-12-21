#ifndef PRODUCT_H
#define PRODUCT_H

#include <QDialog>

#include <QDebug>

#include <QMessageBox>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

#include <memory>

namespace Ui {
class Product;
}

class Product : public QDialog
{
    Q_OBJECT

public:
    explicit Product(QWidget *parent = nullptr);
    ~Product();

    bool validateProduct(QString, double, int);

private slots:

    void on_cancelAddProductPushButton_clicked();

    void on_addProductInPushButton_clicked();

private:
    Ui::Product *ui;
};

#endif // PRODUCT_H
