#include "invoice.h"
#include "ui_invoice.h"

/*
 * Invoice default constructor: Invokes when creating a new Invoice.
 * Makes use of initializeInvoiceWindow to populate the dafault data.
 */
Invoice::Invoice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    ui->setupUi(this);
    qDebug() << "Invoice: Default Constructor: " << "Invoice Window constructed";

    initializeInvoiceWindow(ui);
}

/*
 * Invoice parametrized constructor: Invokes when editing an existing Invoice.
 * Makes use of initializeInvoiceWindow to populate the dafault data.
 * 1. Fills the Invoice fields.
 * 2. Converts Product List string to JSON Object and accordingly fills the Product List Widget.
 */
Invoice::Invoice(QSqlRecord rec,  QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    ui->setupUi(this);
    qDebug() << "Invoice: Paramterized Constructor: " << "Invoice Window constructed";

    initializeInvoiceWindow(ui);

    updateInvoice = true;
    toBeUpdatedinvoiceID = rec.value(0).toInt();

    ui->clientNameLineEdit->setText(rec.value(1).toString());
    ui->clientAddressLineEdit->setText(rec.value(2).toString());
    qDebug() << QDate::fromString(rec.value(4).toString()) << QDate::fromString(rec.value(5).toString());
    ui->issueDateDateEdit->setDate(QDate::fromString(rec.value(4).toString()));
    ui->dueDateDateEdit->setDate(QDate::fromString(rec.value(5).toString()));
    ui->billingAmountDoubleSpinBox->setValue(rec.value(7).toDouble());
    ui->gstAmountDoubleSpinBox->setValue(rec.value(8).toDouble());
    ui->shipAmountDoubleSpinBox->setValue(rec.value(9).toDouble());
    ui->paidAmountDoubleSpinBox->setValue(rec.value(10).toDouble());

    QJsonDocument doc = QJsonDocument::fromJson(rec.value(3).toString().toUtf8());
    QJsonObject productListJSONobj = doc.object();

    for (QJsonObject::iterator it = productListJSONobj.begin(); it != productListJSONobj.end(); ++it) {
        int productID = it.key().toInt();
        int quantity = it.value().toInt();
        QSqlQuery query;
        query.prepare("SELECT ProductName, Price FROM ProductInfo WHERE ProductID = ?");
        query.addBindValue(productID);
        query.exec();

        QTableWidget& productTableWidget = *(ui->productTableWidget);

        if (query.next()) {
            QString productName = query.value(0).toString();
            double price = query.value(1).toDouble();

            int row = productTableWidget.rowCount();
            productTableWidget.insertRow(row);

            productTableWidget.setItem(row, 0, new QTableWidgetItem(QString::number(productID)));
            productTableWidget.setItem(row, 1, new QTableWidgetItem(productName));
            productTableWidget.setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
            productTableWidget.setItem(row, 3, new QTableWidgetItem(QString::number(price * quantity)));
        }

        numberOfProductsAdded += 1;
    }
}

Invoice::~Invoice()
{
    delete ui;
    qDebug() << "Invoice: Destructor: " << "Invoice Window destroyed";
}

/*
 * Used by: Default Invoice Constructor, Parameterized Invoice Constructor.
 * 1. It sets the attribute on object to be deleted upon closing.
 * 2. Sets the default date.
 * 3. Puts the lables in Product List table widget.
 * 4. Fills the completer with the Product Names.
 */
void Invoice::initializeInvoiceWindow(Ui::Invoice* ui)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->issueDateDateEdit->setDate(QDate::currentDate());
    ui->dueDateDateEdit->setDate(QDate::currentDate());

    QList<QString> productLabelsList ({"ID", "Name", "Qty", "Price"});
    QStringList productLabels(productLabelsList);
    ui->productTableWidget->setHorizontalHeaderLabels(productLabels);

    QSqlQuery query;
    QStringList productNames;
    query.exec("SELECT ProductName FROM ProductInfo WHERE isDeleted = 0");
    while (query.next()) {
        productNames << query.value(0).toString();
    }
    query.finish();
    completer.reset(new QCompleter(productNames));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->productNameLineEdit->setCompleter(completer.get());

    ui->productTableWidget->setColumnWidth(0, 50);
    ui->productTableWidget->setColumnWidth(1, 120);
    ui->productTableWidget->setColumnWidth(2, 55);
    ui->productTableWidget->setColumnWidth(3, 60);

    qDebug() << "Invoice: initializeInvoiceWindow(): " << "Invoice Window initialized";
}


/*
 * Used by: collectInvoiceData
 * Verifies the Invoice data.
 */
bool Invoice::validateInvoice() const
{
    bool invoiceValid = false;
    if (clientName != "" and ((numberOfProductsAdded > 0 or updateInvoice == true))  and billingAmount >= 0.0 and gstAmount >= 0.0 \
            and shipAmount >= 0.0 and issueDate >= QDate::currentDate() and dueDate >=  QDate::currentDate()) {
        invoiceValid = true;
    }
    qDebug() << "Invoice: validateInvoice(): " << "Invoice validation: " << invoiceValid;
    return invoiceValid;
}

/*
 * Used by: on_submitInvoicePushButton_clicked()
 * 1. Retrieves the values from all the Invoice fields.
 * 2. It also collects data in Product List table widget and stores them into stringified form of JSON object.
 */
bool Invoice::collectInvoiceData()
{
    bool collectedInvoiceData = false;

    clientName = ui->clientNameLineEdit->text();
    clientAddress = ui->clientAddressLineEdit->text();
    billingAmount = ui->billingAmountDoubleSpinBox->value();
    gstAmount = ui->gstAmountDoubleSpinBox->value();
    shipAmount = ui->shipAmountDoubleSpinBox->value();
    paidAmount = ui->paidAmountDoubleSpinBox->value();
    issueDate = ui->issueDateDateEdit->date();
    dueDate = ui->dueDateDateEdit->date();
    dueDate_sqlite = ui->dueDateDateEdit->date();

    if (validateInvoice()) {
        QTableWidget& productTableWidget = *(ui->productTableWidget);

        QJsonObject productListJSONobj;
        for (int i = 0; i < productTableWidget.rowCount(); ++i) {
            QString ProductID = productTableWidget.item(i, 0)->text();
            QString quantity = productTableWidget.item(i, 2)->text();

            QJsonValue val = productListJSONobj.value(ProductID);
            if (val.isUndefined()) {
                productListJSONobj.insert(ProductID, quantity.toInt());
            }
            else {
                productListJSONobj.insert(ProductID, val.toInt() + quantity.toInt());
            }
        }

        qDebug() << productListJSONobj;

        QJsonDocument productListJSONdoc(productListJSONobj);
        productList = productListJSONdoc.toJson(QJsonDocument::Compact);

        collectedInvoiceData = (productList != "");
    }

    qDebug() << "Invoice: collectInvoiceData(): " << "Invoice data collection: " << collectedInvoiceData;
    return collectedInvoiceData;
}

/*
 * This method displays added product to Product List widget.
 * 1. It fetches the information associated with the product and checks whether the quantity added is not
 *    more than the quantity present in stock.
 * 2. It changes the billingAmount field as the products are added.
 */
void Invoice::on_addProductPushButton_clicked()
{
    QString productName = ui->productNameLineEdit->text();
    int quantity = ui->quantitySpinBox->value();

    int productID = -1;
    double price = -1.0;
    int NumberInStock = -1;

    bool querySuccess = true;
    QSqlQuery query;
    query.prepare("SELECT ProductID, NumberInStock, Price FROM ProductInfo WHERE ProductName = ?");
    query.addBindValue(productName);
    if (query.exec() and query.next()) {
        productID        = query.value(0).toInt();
        NumberInStock = query.value(1).toInt();
        price            = query.value(2).toDouble();
    }
    else {
       querySuccess = false;
       qDebug() << "Invoice: on_addProductPushButton_clicked(): " << query.lastError();
    }

    if (quantity > NumberInStock) {
        querySuccess = false;
    }
    query.finish();
    if (!querySuccess) {
        QMessageBox::warning(this, tr("Failed to Add Product"), tr("1. Check whether the Product exists or not.\n"
                             "2. Quantity of Product you are trying to add is more than present in the stock."));
        return;
    }

    QTableWidget& productTableWidget = *(ui->productTableWidget);
    int row = productTableWidget.rowCount();
    productTableWidget.insertRow(row);
    productTableWidget.setItem(row, 0, new QTableWidgetItem(QString::number(productID)));
    productTableWidget.setItem(row, 1, new QTableWidgetItem(productName));
    productTableWidget.setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    productTableWidget.setItem(row, 3, new QTableWidgetItem(QString::number(price * quantity)));

    productIdList << productID;
    quantityList <<  quantity;

    ui->billingAmountDoubleSpinBox->setValue(ui->billingAmountDoubleSpinBox->value() + (price * quantity));
    numberOfProductsAdded += 1;

    qDebug() << "Invoice: on_addProductPushButton_clicked(): " << productName <<  " added";
}

/*
 * 1.This method stores the Invoice to database and closes the Invoice window.
 *   On the basis of, whether A new Invoice is created or Existing invoice is being edited,
 *   it executees update or insert query. The deciding variable is 'updateInvoice'.
 *
 * 2.It updates the product quantity in database i.e. it subtracts the quantities which are bought
 *   in the Invoice from the database quantity values.
 */
void Invoice::on_submitInvoicePushButton_clicked()
{
    if (collectInvoiceData()) {

        QSqlQuery query;
        if (updateInvoice) {
            query.prepare("UPDATE InvoiceInfo SET ClientName = ?, ClientAddress = ?, ProductList = ?, IssueDate = ?,"
                          "DueDate = ?, DueDate_sqlite = ?, BillingAmount = ?, GstAmount = ?, ShipAmount = ?, PaidAmount = ? "
                          "WHERE InvoiceID = ? and isDeleted = 0");
        }
        else {
            query.prepare("INSERT INTO InvoiceInfo (ClientName, ClientAddress, ProductList, IssueDate, DueDate, DueDate_sqlite, "
                           "BillingAmount, GstAmount, ShipAmount, PaidAmount) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        }
        query.addBindValue(clientName);
        query.addBindValue(clientAddress);
        query.addBindValue(productList);
        query.addBindValue(issueDate.toString());
        query.addBindValue(dueDate.toString());
        query.addBindValue(dueDate_sqlite.toString("yyyy-MM-dd"));
        query.addBindValue(billingAmount);
        query.addBindValue(gstAmount);
        query.addBindValue(shipAmount);
        query.addBindValue(paidAmount);
        if (updateInvoice) {
            query.addBindValue(toBeUpdatedinvoiceID);
        }
        if (!query.exec()) {
            qDebug() << "Invoice: on_submitInvoicePushButton_clicked(): " << query.lastError();
        }
        query.finish();

        for (int i = 0; i < productIdList.size(); ++i) {

            QSqlQuery queryl;
            queryl.prepare("SELECT NumberInStock FROM ProductInfo WHERE ProductID = ?");
            queryl.addBindValue(productIdList.at(i));
            queryl.exec();
            if (!queryl.next()) {
                qDebug() << "Invoice: on_submitInvoicePushButton_clicked(): " << query.lastError();
                return;
            }

            QSqlQuery queryr;
            queryr.prepare("UPDATE ProductInfo SET NumberInStock = ? WHERE ProductID = ?");
            queryr.addBindValue(QString::number(queryl.value(0).toInt() - quantityList.at(i).toInt()));
            queryr.addBindValue(productIdList.at(i).toInt());
            if (!queryr.exec()) {
                qDebug() << "Invoice: on_submitInvoicePushButton_clicked(): " << query.lastError();
                return;
            }
        }

        close();
    }
    else {
        QMessageBox::warning(this, tr("Invoice Submission Failed"), tr("Check whether all fields are filled with valid values."));
    }

    qDebug() << "Invoice: on_submitInvoicePushButton_clicked(): " << "Invoice submitted";
}

void Invoice::on_billingAmountDoubleSpinBox_valueChanged(double value)
{
    ui->totalAmountDoubleSpinBox->setValue(ui->totalAmountDoubleSpinBox->value() + value - prevBillingAmount);
    prevBillingAmount = value;
}

void Invoice::on_gstAmountDoubleSpinBox_valueChanged(double value)
{
     ui->totalAmountDoubleSpinBox->setValue(ui->totalAmountDoubleSpinBox->value() + value - prevGstAmount);
     prevGstAmount = value;
}

void Invoice::on_shipAmountDoubleSpinBox_valueChanged(double value)
{
     ui->totalAmountDoubleSpinBox->setValue(ui->totalAmountDoubleSpinBox->value() + value - prevShipAmount);
     prevShipAmount = value;
}

void Invoice::on_totalAmountDoubleSpinBox_valueChanged(double value)
{
    ui->paidAmountDoubleSpinBox->setValue(value);
}
