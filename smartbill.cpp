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

    qDebug() << "Smartbill: SmartBill Constructor: " << "SmartBill Window constructed";
}

SmartBill::~SmartBill()
{
    delete ui;
    qDebug() << "Smartbill: SmartBill Desstructor: " << "SmartBill Window destroyed";
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
       /* Don't do query.finish(). */
    }
}

void SmartBill::on_issueDateDateEditSearch_editingFinished()
{
    QDate issueDate = ui->issueDateDateEditSearch->date();
    if (!issueDate.isNull()) {
        QSqlQuery query;
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE IssueDate = ? and isDeleted = 0");
        query.addBindValue(issueDate.toString());
        qDebug() << "issueDateSearch: " << issueDate.toString();
        query.exec();
        qDebug() << query.lastError();
        model->setQuery(query);
    }
}

void SmartBill::on_dueDateDateEditSearch_editingFinished()
{
    QDate dueDate = ui->dueDateDateEditSearch->date();
    if (!dueDate.isNull()) {
        QSqlQuery query;
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE DueDate = ? and isDeleted = 0");
        query.addBindValue(dueDate.toString());
        qDebug() << "dueDateSearch: " << dueDate.toString();
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
       issueDate = QDate::fromString(query.value(4).toString());
       dueDate = QDate::fromString(query.value(5).toString());
       billingAmount = query.value(6).toDouble();
       gstAmount = query.value(7).toDouble();
       shipAmount = query.value(8).toDouble();
       paidAmount = query.value(9).toDouble();
   }

   Invoice* invoice = new Invoice(clientName, clientAddress, billingAmount, gstAmount, shipAmount, paidAmount, issueDate, \
                                  dueDate, productList, InvoiceID, this);
   invoice->open();
}

void SmartBill::setCellContent(QTextTable* table, int row, int col, QString string, QTextCursor* cursor)
{
    QTextTableCell cell = table->cellAt(row, col);
    QTextCursor cellCursor = cell.firstCursorPosition();

    QTextBlockFormat cellBlockFormat(cellCursor.blockFormat());
    cellBlockFormat.setAlignment(Qt::AlignHCenter);
    cellCursor.setBlockFormat(cellBlockFormat);

    cellCursor.insertText(string);
    cursor->movePosition(QTextCursor::NextBlock);
}

void SmartBill::on_openInvoiceReportPushButton_clicked()
{
    QModelIndex index = ui->invoicesReportTableView->currentIndex();
    QModelIndex idx = index.siblingAtColumn(0);
    int InvoiceID = idx.data(Qt::DisplayRole).toInt();

    invoiceReportDialog = new QDialog(this);
    invoiceReportDialog->setGeometry(0, 0, 600, 700);
    invoiceReportTextEdit = new QTextEdit(invoiceReportDialog);
    invoiceReportTextEdit->setGeometry(0, 0, 600, 700);
    QPushButton* btn = new QPushButton("Print", invoiceReportDialog);
    btn->setGeometry(5, 550, 100, 45);

    QSqlQuery query;

    query.exec("SELECT name, address, city, pincode, state, country, email, website, contact1, contact2 FROM CompanyInfo");
    if (!query.next()) {
        qDebug() << "SmartBill: on_openInvoiceReportPushButton_clicked(): " << query.lastError();
    }

    QTextCursor cursor(invoiceReportTextEdit->textCursor());

    QTextBlockFormat blockFormat = cursor.blockFormat();
    QTextCharFormat charFormat = cursor.charFormat();

    blockFormat.setAlignment(Qt::AlignCenter);
    charFormat.setFontFamily("Cambria");
    charFormat.setFontPointSize(20);
    charFormat.setFontWeight(10);
    cursor.mergeBlockFormat(blockFormat);
    cursor.mergeCharFormat(charFormat);
    cursor.insertText(query.value(0).toString() + "\n");

    charFormat.setFontPointSize(10);
    cursor.mergeCharFormat(charFormat);
    cursor.insertText(query.value(1).toString() + "\n");
    cursor.insertText(query.value(2).toString() + ",  " + query.value(3).toString() + ", " + query.value(4).toString() + ", " + query.value(5).toString() + "\n");
    cursor.insertText(tr("E-Mail: ") + query.value(6).toString() + ",  " + tr("Website: ") + query.value(7).toString() + "\n");
    cursor.insertText(tr("Contact: ") + query.value(8).toString() + " " + query.value(9).toString() + "\n");
    cursor.insertText(tr("\n\n"));
    query.finish();

    query.prepare("SELECT * FROM InvoiceInfo WHERE InvoiceID = ?");
    query.addBindValue(InvoiceID);
    query.exec();

    if (!query.next()) {
        qDebug() << "SmartBill: on_openInvoiceReportPushButton_clicked(): " << "Invoice Data: " << query.lastError();
    }

    blockFormat.setAlignment(Qt::AlignLeft);
    charFormat.setFontPointSize(10);
    cursor.mergeBlockFormat(blockFormat);
    cursor.mergeCharFormat(charFormat);
    cursor.insertText(tr("Invoice ID: ") + QString::number(InvoiceID) + "\n");
    cursor.insertText(tr("Name: ") + query.value(1).toString() + "\n");
    cursor.insertText(tr("Address: ") + query.value(2).toString() + "\n");
    cursor.insertText(tr("\n\n"));

    QJsonDocument doc = QJsonDocument::fromJson(query.value(3).toString().toUtf8());
    QJsonObject obj = doc.object();

    QTextTableFormat tableFormat;
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 52);
    constraints << QTextLength(QTextLength::PercentageLength, 16);
    constraints << QTextLength(QTextLength::PercentageLength, 16);
    constraints << QTextLength(QTextLength::PercentageLength, 16);
    tableFormat.setColumnWidthConstraints(constraints);
    QTextTable* table  = cursor.insertTable(obj.size() + 1, 4, tableFormat);

    setCellContent(table, 0, 0, "Product Name", &cursor);
    setCellContent(table, 0, 1, "Quantity", &cursor);
    setCellContent(table, 0, 2, "Unit Price", &cursor);
    setCellContent(table, 0, 3, "Price", &cursor);

    int row = 1;
    for (QJsonObject::Iterator it = obj.begin(); it != obj.end(); ++it, ++row) {

        int ProductID = it.key().toInt();
        int quantity = it.value().toInt();

        QSqlQuery query;
        query.prepare("SELECT ProductName, Price FROM ProductInfo WHERE ProductID = ?");
        query.addBindValue(ProductID);
        query.exec();

        if (!query.next()) {
            qDebug() << "SmartBill: on_openInvoiceReportPushButton_clicked(): " << "String to JSON: " << query.lastError();
            break;
        }

        setCellContent(table, row, 0, query.value(0).toString(), &cursor);
        setCellContent(table, row, 1, QString::number(quantity), &cursor);
        setCellContent(table, row, 2, query.value(1).toString(), &cursor);
        setCellContent(table, row, 3, QString::number(query.value(1).toDouble() * quantity), &cursor);

        query.finish();
    }

    cursor.movePosition(QTextCursor::NextBlock);
    cursor.insertText(tr("\n\n"));
    charFormat.setFontFamily("Cambria");
    charFormat.setFontPointSize(10);
    charFormat.setFontWeight(10);
    cursor.mergeCharFormat(charFormat);
    cursor.insertText(tr("Bill Amount: ") + query.value(7).toString() + "\n");
    cursor.insertText(tr("GST: ") + query.value(8).toString() + "\n");
    cursor.insertText(tr("Ship Amount: ") + query.value(9).toString() + "\n");
    cursor.insertText(tr("Total Amount: ") + QString::number(query.value(7).toDouble() + query.value(8).toDouble() + query.value(9).toDouble()) + "\n");
    cursor.insertText(tr("Paid Amount: ") + query.value(10).toString() + "\n");

    blockFormat.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(blockFormat);
    cursor.insertText("Thank You. :)");

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
        sql = "SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE " + remainingAmount + " != 0 and DueDate_sqlite >= date('now') and isDeleted = 0";
    }
    else if (paymentStatus == "Overdue") {
        sql = "SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE " + remainingAmount + " != 0 and DueDate_sqlite < date('now') and isDeleted = 0";
    }
    query.exec(sql);
    if (!query.next())
        qDebug() << "SmartBill: on_paymentStatusComboBox_activated(): " << query.lastError();
    model->setQuery(query);
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
