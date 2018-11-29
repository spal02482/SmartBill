#include "product.h"
#include "ui_product.h"

Product::Product(fastbilldb& fbdb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Product)
{
    ui->setupUi(this);

    /* Delete the Dialog upon closing */
    QDialog::setAttribute(Qt::WA_DeleteOnClose);

    /* Create a query object, prepare the Insert query and bind values to it and Execute it */
    QSqlQuery query(fbdb.getConnection());
}

Product::~Product()
{
    delete ui;
    qDebug() << "Deleting Product Window";
}

bool Product::validateProduct(QString ProductName, double Price, int NumberInStock)
{
    query.prepare("SELECT * FROM ProductInfo WHERE ProductName = ?");
    query.addBindValue(ProductName);
    query.exec();
    bool productValid = false;
    if (ProductName != "" and Price >= 0.0 and NumberInStock > 0 and !query.next()) {
        productValid = true;
    }
    query.finish();
    return productValid;
}

/* Cancel Adding Product in ProductInfo Table */
void Product::on_cancelAddProductPushButton_clicked()
{
    this->close();

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


        QString sql = "INSERT INTO ProductInfo (ProductName, Price, NumberInStock, SupplierName, Description)"
                "VALUES (?, ?, ?, ?, ?)";
        query.prepare(sql);
        query.addBindValue(ProductName);
        query.addBindValue(Price);
        query.addBindValue(NumberInStock);
        query.addBindValue(SupplierName);
        query.addBindValue(Description);
        bool querySuccess = query.exec();

        if (querySuccess) {
            qDebug() << "Query Successfull";
        }
        else {
            qDebug() << "Query Unsuccessfull " << query.lastError();
        }

        query.finish();
    }
    else {
        QMessageBox::critical(this, tr("Failed to Add Product"), tr("Make sure that\n 1. All asterisks marked"
                              "fields are filled\n2. Product already doesn't exist"));
    }

    this->close();
}
