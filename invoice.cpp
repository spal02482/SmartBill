#include "invoice.h"
#include "ui_invoice.h"


void Invoice::initializeInvoiceWindow(Ui::Invoice* ui, smartbilldb& fbdb)
{
    /* Invoice window should be deleted upon closing it. */
    QDialog::setAttribute(Qt::WA_DeleteOnClose);

    /* Assign the Issue and Due date current Date. */
    ui->issueDateDateEdit->setDate(QDate::currentDate());
    ui->dueDateDateEdit->setDate(QDate::currentDate());

    /* Initialize query with the database. */
    query = std::make_unique<QSqlQuery>(fbdb.getConnection());

    /* Set the Headers in the Add Product Table Widget. */
    QList<QString> productLabelsList ({"ID", "Name", "Qty", "Price"});
    QStringList productLabels(productLabelsList);
    ui->productTableWidget->setHorizontalHeaderLabels(productLabels);

    /* Get the list of Products available. */
    QStringList productNames;
    query->exec("SELECT ProductName FROM ProductInfo WHERE isDeleted = 0");
    while (query->next()) {
        productNames << query->value(0).toString();
    }
    query->finish();

    //qDebug() << productNames;

    /* Populate the completer with the products available and set it for autocompletion of Product Names. */
    completer.reset(new QCompleter(productNames));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->productNameLineEdit->setCompleter(completer.get());
}


Invoice::Invoice(smartbilldb& fbdb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    ui->setupUi(this);
    qDebug() << "Created Invoice Window.";
    initializeInvoiceWindow(ui, fbdb);
}

Invoice::Invoice(smartbilldb& fbdb, QString clientName, QString clientAddress, \
                 double billingAmount, double gstAmount, double shipAmount, \
                 QDate issueDate, QDate dueDate, QString productList, int InvoiceID, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    toBeUpdatedinvoiceID = InvoiceID;
    ui->setupUi(this);
    qDebug() << "Created Invoice Window, with Invoice Parameter values";
    initializeInvoiceWindow(ui, fbdb);

    updateInvoice = true;
    qDebug() << billingAmount << gstAmount << shipAmount << issueDate.toString() << dueDate.toString();

    this->billingAmount = billingAmount;
    ui->clientNameLineEdit->setText(clientName);
    ui->clientAddressLineEdit->setText(clientAddress);
    ui->billingAmountDoubleSpinBox->setValue(billingAmount);
    ui->gstAmountDoubleSpinBox->setValue(gstAmount);
    ui->shipAmountDoubleSpinBox->setValue(shipAmount);
    ui->issueDateDateEdit->setDate(issueDate);
    ui->dueDateDateEdit->setDate(dueDate);

    QJsonDocument doc = QJsonDocument::fromJson(productList.toUtf8());
    productListJSONobj = doc.object();

    for (QJsonObject::iterator it = productListJSONobj.begin(); it != productListJSONobj.end(); ++it) {
        int productID = it.key().toInt();
        int quantity = it.value().toInt();
        QSqlQuery query(fbdb.getConnection());
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
    }
}

Invoice::~Invoice()
{
    delete ui;
    qDebug() << "Deleted Invoice Window";
}

bool Invoice::validateInvoice() const
{
    bool invoiceValid = false;
    if (clientName != "" and numberOfProductsAdded > 0 and billingAmount >= 0.0 and gstAmount >= 0.0 \
            and shipAmount >= 0.0 and issueDate >= QDate::currentDate() and dueDate >=  QDate::currentDate()) {
        invoiceValid = true;
    }
    return invoiceValid;
}

bool Invoice::initializeInvoiceData()
{
    bool initializedInvoiceData = false;

    clientName = ui->clientNameLineEdit->text();
    clientAddress = ui->clientAddressLineEdit->text();
    billingAmount = ui->billingAmountDoubleSpinBox->value();
    gstAmount = ui->gstAmountDoubleSpinBox->value();
    shipAmount = ui->shipAmountDoubleSpinBox->value();
    issueDate = ui->issueDateDateEdit->date();
    dueDate = ui->dueDateDateEdit->date();

    if (validateInvoice()) {

        QTableWidget& productTableWidget = *(ui->productTableWidget);

        /* A JSON Object which holds the key-value pair of product IDs and and their Quantity bought. */
        for (int i = 0; i < productTableWidget.rowCount(); ++i) {
            productListJSONobj.insert(productTableWidget.item(i, 0)->text(), productTableWidget.item(i, 2)->text().toInt());
        }
        productListJSONdoc = std::make_unique<QJsonDocument>(productListJSONobj);
        productList = productListJSONdoc->toJson(QJsonDocument::Compact);

        initializedInvoiceData = (productList != "");
    }

    return initializedInvoiceData;
}

void Invoice::on_addProductPushButton_clicked()
{
    QString productName = ui->productNameLineEdit->text();
    int quantity = ui->quantitySpinBox->value();

    int productID = -1;
    double price = -1.0;
    int quantityDatabase = -1;

    bool querySuccess = true;

    query->prepare("SELECT ProductID, NumberInStock, Price FROM ProductInfo WHERE ProductName = ?");
    query->addBindValue(productName);
    query->exec();
    if (query->next()) {
        productID        = query->value(0).toInt();
        quantityDatabase = query->value(1).toInt();
        price            = query->value(2).toDouble();
        qDebug() << productID << quantityDatabase << price;
    }
    else {
       querySuccess = false;
    }

    if (quantity > quantityDatabase) {
        querySuccess = false;
    }

    query->finish();

    if (querySuccess == false) {
        QMessageBox::warning(this, tr("Failed to Add Product"), tr("1. Check whether the Product exists or not.\n"
                             "2. Quantity of Product you are trying to add is more than present in the stock.\n"));
        return;
    }

    QTableWidget& productTableWidget = *(ui->productTableWidget);

    int row = productTableWidget.rowCount();
    productTableWidget.insertRow(row);

    productTableWidget.setItem(row, 0, new QTableWidgetItem(QString::number(productID)));
    productTableWidget.setItem(row, 1, new QTableWidgetItem(productName));
    productTableWidget.setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    productTableWidget.setItem(row, 3, new QTableWidgetItem(QString::number(price * quantity)));

    billingAmount += (price * quantity);
    ui->billingAmountDoubleSpinBox->setValue(billingAmount);
    numberOfProductsAdded += 1;
}

void Invoice::on_submitPushButton_clicked()
{
    if (initializeInvoiceData()) {

        if (updateInvoice) {
            query->prepare("UPDATE InvoiceInfo SET ClientName = ?, ClientAddress = ?, ProductList = ?, IssueDate = ?,"
                           "DueDate = ?, BillingAmount = ?, GstAmount = ?, ShipAmount = ? WHERE InvoiceID = ?");
        }
        else {
            query->prepare("INSERT INTO InvoiceInfo (ClientName, ClientAddress, ProductList, IssueDate, DueDate, "
                           "BillingAmount, GstAmount, ShipAmount) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        }
        query->addBindValue(clientName);
        query->addBindValue(clientAddress);
        query->addBindValue(productList);
        query->addBindValue(issueDate.toString());
        query->addBindValue(dueDate.toString());
        query->addBindValue(billingAmount);
        query->addBindValue(gstAmount);
        query->addBindValue(shipAmount);
        if (updateInvoice) {
            query->addBindValue(toBeUpdatedinvoiceID);
        }
        query->exec();
        qDebug() << query->lastError();
        close();
    }
    else {
        QMessageBox::warning(this, tr("Invoice Submission Failed"), tr("Check Whether all fields are filled with valid values"));
    }
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

