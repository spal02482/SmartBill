#include "invoice.h"
#include "ui_invoice.h"


Invoice::Invoice(fastbilldb& fbdb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    ui->setupUi(this);
    QDialog::setAttribute(Qt::WA_DeleteOnClose);

    ui->issueDateDateEdit->setDate(QDate::currentDate());
    ui->dueDateDateEdit->setDate(QDate::currentDate());

    QSqlQuery query(fbdb.getConnection());

    QList<QString> productLabelsList ({"ID", "Name", "Qty", "Price"});
    QStringList productLabels(productLabelsList);
    ui->productTableWidget->setHorizontalHeaderLabels(productLabels);

    QStringList productNames;
    query.exec("SELECT ProductName FROM ProductInfo WHERE isDeleted = 0");
    while (query.next()) {
        productNames << query.value(0).toString();
    }
    query.finish();
    qDebug() << productNames;

    completer = new QCompleter(productNames);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->productNameLineEdit->setCompleter(completer);
}

Invoice::~Invoice()
{
    delete completer;
    qDebug() << "Deleting Invoice Window";
    delete ui;
}

bool Invoice::validateInvoice(QString clientName, int numberOfProductsAdded, double billingAmount, \
                              double gstAmount, double shipAmount, QDate issueDate, QDate dueDate) const
{
    if (clientName != "" and numberOfProductsAdded > 0 and billingAmount > 0.0 and gstAmount >= 0.0 and shipAmount >= 0.0 \
            and QDate::currentDate() >= issueDate and QDate::currentDate() >= dueDate) {
        return true;
    }
    else {
        return false;
    }
}

void Invoice::on_closeInvoicePushButton_clicked()
{
    this->close();
}

void Invoice::on_addProductPushButton_clicked()
{
    QString productName = ui->productNameLineEdit->text();
    int quantity = ui->quantitySpinBox->value();

    int productID = -1;
    double price = -1.0;
    int quantityDatabase = -1;

    bool querySuccess = true;

    query.prepare("SELECT ProductID, NumberInStock, Price FROM ProductInfo WHERE ProductName = ?");
    query.addBindValue(productName);
    query.exec();
    if (query.next()) {
        productID = query.value(0).toInt();
        quantityDatabase = query.value(1).toInt();
        price = query.value(2).toDouble();
        qDebug() << productID << quantityDatabase << price;
    }
    else {
       querySuccess = false;
    }

    if (quantity > quantityDatabase) {
        querySuccess = false;
    }

    query.finish();

    if (querySuccess == false) {
        QMessageBox::warning(this, "Failed to Add Product", "1. Check whether the Product exists or not.\n"
                             "2. Quantity of Product you are trying to add is more than present in the stock.\n");
        return;
    }

    QTableWidget& productTableWidget = *ui->productTableWidget;

    int row = productTableWidget.rowCount();
    productTableWidget.insertRow(row);

    productTableWidget.setItem(row, 0, new QTableWidgetItem(QString::number(productID)));
    productTableWidget.setItem(row, 1, new QTableWidgetItem(productName));
    productTableWidget.setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    productTableWidget.setItem(row, 3, new QTableWidgetItem(QString::number(price * quantity)));
}
