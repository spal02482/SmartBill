#include "smartbill.h"
#include "ui_smartbill.h"

SmartBill::SmartBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmartBill)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    QSqlQuery query;
    query.exec("SELECT * FROM CompanyInfo");

    if (!query.next()) {
        qDebug() << "Error retrieving values, SmartBill Constructor:" << query.lastError();
        return;
    }

    ui->companyNameLabel->setText(query.value(0).toString());
    ui->companyEmailLabel->setText(query.value(7).toString());
    ui->companyCinLabel->setText("<b>CIN:</b>" + query.value(9).toString());
    ui->companyAddressLabel->setText(query.value(1).toString() + " " +  query.value(2).toString() + " " + query.value(5).toString() + \
                                     " " + query.value(3).toString() + " " + query.value(4).toString());
    ui->companyContact1Label->setText(query.value(12).toString());
    ui->companyContact2Label->setText(query.value(13).toString());

    selectQueryParam = "InvoiceID, ClientName, ClientAddress, IssueDate, DueDate, BillingAmount,"
                       "GstAmount, ShipAmount, (BillingAmount + GstAmount + ShipAmount),"
                        "(BillingAmount + GstAmount + ShipAmount) - paidAmount";

    model = std::make_unique<QSqlQueryModel>(this);
    model->setQuery("SELECT " + selectQueryParam +  " FROM InvoiceInfo WHERE isDeleted = 0");
    ui->invoicesReportTableView->setModel(model.get());

    QSqlQueryModel* clientNamesList = new QSqlQueryModel(this);
    clientNamesList->setQuery("SELECT ClientName FROM InvoiceInfo");
    completer = std::make_unique<QCompleter>(clientNamesList);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->clientNameLineEditSearch->setCompleter(completer.get());

    /*Set Header Data. */
    model->setHeaderData(0, Qt::Horizontal, tr("ID"), Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, tr("Name"), Qt::DisplayRole);
    model->setHeaderData(2, Qt::Horizontal, tr("Address"), Qt::DisplayRole);
    model->setHeaderData(3, Qt::Horizontal, tr("Date issued"), Qt::DisplayRole);
    model->setHeaderData(4, Qt::Horizontal, tr("Date due"), Qt::DisplayRole);
    model->setHeaderData(5, Qt::Horizontal, tr("Bill Amount"), Qt::DisplayRole);
    model->setHeaderData(6, Qt::Horizontal, tr("GST"), Qt::DisplayRole);
    model->setHeaderData(7, Qt::Horizontal, tr("Ship Cost"), Qt::DisplayRole);
    model->setHeaderData(8, Qt::Horizontal, tr("Total Amount"), Qt::DisplayRole);
    model->setHeaderData(9, Qt::Horizontal, tr("Due Amount"), Qt::DisplayRole);

    /* Set Column Widths. */
    ui->invoicesReportTableView->setColumnWidth(0, 65);
    ui->invoicesReportTableView->setColumnWidth(1, 180);
    ui->invoicesReportTableView->setColumnWidth(2, 311);
    ui->invoicesReportTableView->setColumnWidth(3, 140);
    ui->invoicesReportTableView->setColumnWidth(4, 140);
    ui->invoicesReportTableView->setColumnWidth(5, 100);
    ui->invoicesReportTableView->setColumnWidth(6, 100);
    ui->invoicesReportTableView->setColumnWidth(7, 100);
    ui->invoicesReportTableView->setColumnWidth(8, 100);
    ui->invoicesReportTableView->setColumnWidth(9, 100);

    ui->paymentStatusComboBox->addItem("All");
    ui->paymentStatusComboBox->addItem("Paid");
    ui->paymentStatusComboBox->addItem("Due");
    ui->paymentStatusComboBox->addItem("Overdue");

    ui->issueDateDateEditSearch->setDate(QDate::currentDate());
    ui->dueDateDateEditSearch->setDate(QDate::currentDate());
}

SmartBill::~SmartBill()
{
    delete ui;
    qDebug() << "Deleting SmartBill Window";
}


void SmartBill::on_addInvoicePushButton_clicked()
{
    invoice = new Invoice(this);
    invoice->open();
}

void SmartBill::on_addProductPushButton_clicked()
{
    product = new Product(this);
    product->open();
}

void SmartBill::on_viewProductPushButton_clicked()
{
    QDialog* ProductView = new QDialog(this);
    if (ProductView->objectName().isEmpty())
        ProductView->setObjectName(QStringLiteral("ProductView"));
    ProductView->setWindowModality(Qt::ApplicationModal);
    ProductView->resize(730, 500);
    QFont font;
    font.setFamily(QStringLiteral("Candara"));
    font.setPointSize(10);
    ProductView->setFont(font);
    QIcon icon;
    icon.addFile(QStringLiteral(":/Icons/smartbill.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
    ProductView->setWindowIcon(icon);
    productViewTableView = new QTableView(ProductView);
    productViewTableView->setObjectName(QStringLiteral("productViewTableView"));
    productViewTableView->setGeometry(QRect(0, 0, 730, 500));
    productViewTableView->setAutoScrollMargin(15);
    productViewTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    productViewTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    productViewTableView->setShowGrid(true);
    productViewTableView->verticalHeader()->setVisible(false);

    modelProductView = std::make_unique<QSqlQueryModel>(ProductView);
    QSqlQuery query;
    query.exec("SELECT ProductID, ProductName, Price, NumberInStock, SupplierName, Description FROM ProductInfo where isDeleted = 0");
    modelProductView->setQuery(query);
    productViewTableView->setModel(modelProductView.get());

    /* Set Header Names. */
    modelProductView->setHeaderData(0, Qt::Horizontal, tr("ID"), Qt::DisplayRole);
    modelProductView->setHeaderData(1, Qt::Horizontal, tr("Product Name"), Qt::DisplayRole);
    modelProductView->setHeaderData(2, Qt::Horizontal, tr("Price (INR)"), Qt::DisplayRole);
    modelProductView->setHeaderData(3, Qt::Horizontal, tr("Stock Qty."), Qt::DisplayRole);
    modelProductView->setHeaderData(4, Qt::Horizontal, tr("Suppliler Name"), Qt::DisplayRole);
    modelProductView->setHeaderData(5, Qt::Horizontal, tr("Description"), Qt::DisplayRole);

    /* Set Column Widths. */
    productViewTableView->setColumnWidth(0, 30);
    productViewTableView->setColumnWidth(1, 200);
    productViewTableView->setColumnWidth(2, 80);
    productViewTableView->setColumnWidth(3, 80);
    productViewTableView->setColumnWidth(4, 100);
    productViewTableView->setColumnWidth(5, 225);

    QObject::connect(productViewTableView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(updateProductInfo(const QModelIndex&)));
    ProductView->open();
}

void SmartBill::updateProductInfo(const QModelIndex& index)
{
    qDebug() << index;
    QModelIndex idx = index.siblingAtColumn(0);
    int ProductID = idx.data(Qt::DisplayRole).toInt();
    qDebug() << ProductID;

    QSqlQuery query;
    query.prepare("SELECT * FROM ProductInfo WHERE ProductID = ?");
    query.addBindValue(ProductID);
    if (query.exec()) {
        query.next();
    }
    else {
        qDebug() << "SmartBill: updateProductInfo() " << query.lastError();
    }
    Product* product = new Product(query.record(), this);
    product->open();
}

void SmartBill::on_invoiceIDLineEditSearch_editingFinished()
{
    int invoiceID = ui->invoiceIDLineEditSearch->text().toInt();
    if (invoiceID > 0) {
        QSqlQuery query;
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE InvoiceID = ? and isDeleted = 0");
        query.addBindValue(invoiceID);
        query.exec();
        model->setQuery(query);
        /* Don't do query.finish(). */
    }
}

void SmartBill::on_clientNameLineEditSearch_editingFinished()
{
    QString clientName = ui->clientNameLineEditSearch->text();
    qDebug() << clientName;
    if (clientName != "") {
       QSqlQuery query;
       query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE ClientName = ? and isDeleted = 0");
       query.addBindValue(clientName);
       query.exec();
       model->setQuery(query);
    }
}

void SmartBill::on_issueDateDateEditSearch_editingFinished()
{
    QDate issueDate = ui->issueDateDateEditSearch->date();
    qDebug() << issueDate.isNull() << issueDate.isValid();
    if (!issueDate.isNull()) {
        QSqlQuery query;
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE IssueDate = ? and isDeleted = 0");
        query.addBindValue(issueDate.toString("yyyy-MM-dd"));
        qDebug() << "issueDateSearch: " << issueDate.toString("yyyy-MM-dd");
        query.exec();
        qDebug() << query.lastError();
        model->setQuery(query);
        /* Don't do query.finish(). */
    }
}

void SmartBill::on_dueDateDateEditSearch_editingFinished()
{
    QDate dueDate = ui->dueDateDateEditSearch->date();
    if (!dueDate.isNull()) {
        QSqlQuery query;
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE DueDate = ? and isDeleted = 0");
        query.addBindValue(dueDate.toString("yyyy-MM-dd"));
        qDebug() << "dueDateSearch: " << dueDate.toString("yyyy-MM-dd");
        query.exec();
        model->setQuery(query);
        /* Don't do query.finish(). */
    }
}

void SmartBill::on_refreshPushButton_clicked()
{
    ui->invoiceIDLineEditSearch->clear();
    ui->clientNameLineEditSearch->clear();
    ui->issueDateDateEditSearch->setDate(QDate::currentDate());
    ui->dueDateDateEditSearch->setDate(QDate::currentDate());
    ui->paymentStatusComboBox->setCurrentText("All");
    model->setQuery("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE isDeleted = 0");
}

void SmartBill::on_invoicesReportTableView_doubleClicked(const QModelIndex &index)
{
   QModelIndex idx = index.siblingAtColumn(0);
   int InvoiceID = idx.data(Qt::DisplayRole).toInt();
   QSqlQuery query;
   query.prepare("SELECT * FROM InvoiceInfo WHERE InvoiceID = ?");
   query.addBindValue(InvoiceID);
   query.exec();

   QString clientName;
   QString clientAddress;
   QString productList;
   double billingAmount = 0.0;
   double gstAmount = 0.0;
   double shipAmount = 0.0;
   double paidAmount = 0.0;
   QDate issueDate;
   QDate dueDate;

   if (query.next()) {
       clientName = query.value(1).toString();
       clientAddress = query.value(2).toString();
       productList = query.value(3).toString();
       issueDate = QDate::fromString(query.value(4).toString(), "yyyy-MM-dd");
       dueDate = QDate::fromString(query.value(5).toString(), "yyyy-MM-dd");
       billingAmount = query.value(6).toDouble();
       gstAmount = query.value(7).toDouble();
       shipAmount = query.value(8).toDouble();
       paidAmount = query.value(9).toDouble();
   }

   Invoice* invoice = new Invoice(clientName, clientAddress, billingAmount, gstAmount, shipAmount, paidAmount, issueDate, \
                                  dueDate, productList, InvoiceID, this);
   invoice->open();
}

void SmartBill::on_openInvoiceReportPushButton_clicked()
{
    QModelIndex index = ui->invoicesReportTableView->currentIndex();
    QModelIndex idx = index.siblingAtColumn(0);
    int InvoiceID = idx.data(Qt::DisplayRole).toInt();
    QSqlQuery query;
    query.prepare("SELECT * FROM InvoiceInfo WHERE InvoiceID = ?");
    query.addBindValue(InvoiceID);
    query.exec();

    QString clientName;
    QString clientAddress;
    QString productList;
    double billingAmount = 0.0;
    double gstAmount = 0.0;
    double shipAmount = 0.0;
    QString issueDate;
    QString dueDate;

    if (query.next()) {
        clientName = query.value(1).toString();
        clientAddress = query.value(2).toString();
        productList = query.value(3).toString();
        issueDate = QDate::fromString(query.value(4).toString(), "yyyy-MM-dd").toString();
        dueDate = QDate::fromString(query.value(5).toString(), "yyyy-MM-dd").toString();
        billingAmount = query.value(6).toDouble();
        gstAmount = query.value(7).toDouble();
        shipAmount = query.value(8).toDouble();
    }

    invoiceReportDialog = new QDialog(this);
    invoiceReportDialog->setGeometry(0, 0, 700, 600);
    invoiceReportTextEdit = new QTextEdit(invoiceReportDialog);
    invoiceReportTextEdit->setGeometry(5, 5, 690, 545);
    QPushButton* btn = new QPushButton("Print", invoiceReportDialog);
    btn->setGeometry(5, 550, 100, 45);

    QString html =
            "<h1 align=\"center\">Invoice</h1><br>"
            "<h2 align=\"center\"><b>" + clientName + "</b></h2><br>"
            "<h3 align=\"center\"><i>" + clientAddress + "</i></h3><br>"
            "<table align=\"center\" border=\"1\">"
            "<tr>"
            "<td>ProductID</td>"
            "<td>ProductName</td>"
            "<td>Quantity</td>"
            "<td>Price/Item</td>"
            "<td>Price</td>"
            "</tr>";

    QJsonDocument doc = QJsonDocument::fromJson(productList.toUtf8());
    QJsonObject obj = doc.object();

    for (QJsonObject::Iterator it = obj.begin(); it != obj.end(); ++it) {
        qDebug() << it.key() << it.value() <<  it.value().toString();
        html += "<tr>";
        int ProductID = it.key().toInt();
        QSqlQuery query;
        query.prepare("SELECT ProductName, Price FROM ProductInfo WHERE ProductID = ?");
        query.addBindValue(ProductID);
        query.exec();

        if (query.next()) {
            html += ("<td>" + it.key() + "</td>");
            html += ("<td>" + query.value(0).toString() + "</td>");
            html += ("<td>" + QString::number(it.value().toInt()) + "</td>");
            html += ("<td>" + query.value(1).toString() + "</td>");
            html += ("<td>" + QString::number((query.value(1).toDouble() * it.value().toInt())) + "</td>");
        }
        html += "</tr>";
    }

    html += "</table>";

    html += "<h3 align=\"center\">GST Price: " + QString::number(gstAmount) + "</h3>";
    html += "<h3 align=\"center\">Shipping Price: " + QString::number(shipAmount) + "</h3>";
    html += "<h2 align=\"center\">Total Price: " + QString::number(billingAmount + gstAmount + shipAmount) + "</h2>";
    html += "<h3 align=\"center\">Date issued: " + issueDate + "</h3>";
    html += "<h3 align=\"center\">Payment due till date: " + dueDate + "</h3>";

    invoiceReportTextEdit->setHtml(html);
    invoiceReportDialog->open();
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(printInvoiceReport()));
}

void SmartBill::printInvoiceReport()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, invoiceReportDialog);
    if (printDialog.exec() == QDialog::Accepted) {
        invoiceReportTextEdit->print(&printer);
    }

}

void SmartBill::on_changeCompanyInfoPushButton_clicked()
{
    companyInfo = new company(this);
    companyInfo->open();
}

void SmartBill::submitCompanyInfo()
{
    QSqlQuery query;
    query.prepare("UPDATE CompanyInfo SET CompanyName = ?, CompanyEmail = ?, CompanyCIN = ?, CompanyAddress = ?,"
                  "CompanyContact1 = ?, CompanyContact2 = ?");
    query.addBindValue(companyName->text());
    query.addBindValue(companyEmail->text());
    query.addBindValue(companyCIN->text());
    query.addBindValue(companyAddress->text());
    query.addBindValue(companyContact1->text());
    query.addBindValue(companyContact2->text());
    query.exec();

    QMessageBox::information(this, tr("Success"), tr("Company Information successfully changed.\n"
                                                     "Restart Application for the change to Reflect."));

    companyInfo->close();
}

QValidator::State NameValidator::validate(QString& name, int& cursorPosition) const
{
    bool nameContainsSpecialChars = false;
    for (QChar c : name) {
        if (not c.isLetter()) {
            nameContainsSpecialChars = true;
        }
    }

    if (nameContainsSpecialChars == false and cursorPosition == name.length()) {
        return QValidator::Acceptable;
    }
    else {
        return QValidator::Invalid;
    }
}

void SmartBill::on_paymentStatusComboBox_activated(const QString &paymentStatus)
{
    qDebug() << "SmartBill: on_paymentStatusComboBox_activated(): " << paymentStatus << " selected";

    QSqlQuery query;
    QString sql;

    QString remainingAmount = "((BillingAmount + GstAmount + ShipAmount) - paidAmount)";
    if (paymentStatus == "All") {
        sql = "SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE isDeleted = 0";
    }
    else if (paymentStatus == "Paid") {
        sql = "SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE " + remainingAmount + " = 0 and isDeleted = 0";
    }
    else if (paymentStatus == "Due") {
        sql = "SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE " + remainingAmount + " != 0 and DueDate >= date('now') and isDeleted = 0";
    }
    else if (paymentStatus == "Overdue") {
        sql = "SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE " + remainingAmount + " != 0 and DueDate < date('now') and isDeleted = 0";
    }
    query.exec(sql);
    qDebug() << "SmartBill: on_paymentStatusComboBox_activated(): " << query.lastError();
    model->setQuery(query);
}
