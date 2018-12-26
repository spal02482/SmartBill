#include "product.h"
#include "ui_product.h"

Product::Product(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Product)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

Product::Product(QSqlRecord rec, QWidget *parent):
    QDialog(parent),
    ui(new Ui::Product)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    updateProduct = true;
    toBeUpdatedProductID = rec.value(0).toInt();

    ui->productNameLineEdit->setText(rec.value(1).toString());
    ui->priceDoubleSpinBox->setValue(rec.value(2).toDouble());
    ui->quanititySpinBox->setValue(rec.value(3).toInt());
    ui->supplierNameLineEdit->setText(rec.value(4).toString());
    ui->productDescpnLineEdit->setText((rec.value(5).toString()));
}

Product::~Product()
{
    delete ui;
    qDebug() << "Deleting Product Window";
}

bool Product::validateProduct(QString ProductName, double Price, int NumberInStock)
{
    /* Create a query object, prepare the Insert query and bind values to it and Execute it */
    bool productValid = false;
    if (ProductName != "" and Price >= 0.0 and NumberInStock > 0) {
        productValid = true;
    }
    return productValid;
}

/* Add Product in ProductInfo Table */
void Product::on_addProductInPushButton_clicked()
{
    /* Extract All the Values from the Product Form*/
    QString ProductName = ui->productNameLineEdit->text();
    double Price = ui->priceDoubleSpinBox->value();
    int  NumberInStock= ui->quanititySpinBox->value();
    QString SupplierName = ui->supplierNameLineEdit->text();
    QString Description = ui->productDescpnLineEdit->text();


    /* If Product Information is valid, then add it to ProductInfo Table */
    if (validateProduct(ProductName, Price, NumberInStock)) {
        QString sql;
        if (updateProduct) {
            sql = "UPDATE ProductInfo SET ProductName = ?, Price = ?, NumberInStock = ?, SupplierName = ?, Description = ? WHERE "
                  "ProductID = ?";
        }
        else {
            sql = "INSERT INTO ProductInfo (ProductName, Price, NumberInStock, SupplierName, Description)"
                    "VALUES (?, ?, ?, ?, ?)";
        }
        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(ProductName);
        query.addBindValue(Price);
        query.addBindValue(NumberInStock);
        query.addBindValue(SupplierName);
        query.addBindValue(Description);

        if (updateProduct) {
            query.addBindValue(toBeUpdatedProductID);
        }


        if (query.exec()) {
            qDebug() << "Query Successfull";
        }
        else {
            qDebug() << "Query Unsuccessfull " << query.lastError();
        }

        query.finish();
    }
    else {
        QMessageBox::critical(this, tr("Failed to Add Product"), tr("Make sure that all asterisks marked fields are filled."));
    }

    this->close();
}
