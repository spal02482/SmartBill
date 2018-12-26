#ifndef PRODUCT_H
#define PRODUCT_H

#include <QtCore/QDebug>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>

#include <memory>

namespace Ui {
class Product;
}

class Product : public QDialog
{
    Q_OBJECT

public:
    explicit Product(QWidget *parent = nullptr);
    explicit Product(QSqlRecord , QWidget *parent = nullptr);
    ~Product();

    bool validateProduct(QString, double, int);

private slots:
    void on_addProductInPushButton_clicked();

private:
    Ui::Product *ui;

    bool updateProduct = false;
    int toBeUpdatedProductID = 0;
};

#endif // PRODUCT_H
