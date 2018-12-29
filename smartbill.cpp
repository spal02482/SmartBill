#include "smartbill.h"
#include "ui_smartbill.h"

SmartBill::SmartBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmartBill)
{
    ui->setupUi(this);
    qDebug() << "Smartbill: SmartBill Constructor: " << "SmartBill Window constructed";

    this->setAttribute(Qt::WA_DeleteOnClose);

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

    QStringList smartBillLabels({"ID", "Name", "Address", "Date issued", "Date due", "Bill Amount", "GST", "Ship Cost", "Total Amount", "Due Amount"});
    QVector<int> smartBillColumnWidths({65, 180, 311, 140, 140, 100, 100, 100, 100, 100});
    for (int i = 0; i < 10; ++i) {
        model->setHeaderData(i, Qt::Horizontal, smartBillLabels.at(i), Qt::DisplayRole);
        ui->invoicesReportTableView->setColumnWidth(i, smartBillColumnWidths.at(i));
    }

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

    /* Set Header Names and Column Widths */
    QStringList productInfoLabels({"ID", "Product Name", "Price (INR)", "Stock Qty.", "Supplier Name", "Description"});
    QVector<int> productColumnWidths({30, 200, 80, 80, 100, 225});
    for (int i = 0; i < 6; ++i) {
        modelProductView->setHeaderData(i, Qt::Horizontal, productInfoLabels.at(i), Qt::DisplayRole);
        productViewTableView->setColumnWidth(i, productColumnWidths.at(i));
    }

    QObject::connect(productViewTableView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(updateProductInfo(const QModelIndex&)));
    ProductView->open();
}

void SmartBill::updateProductInfo(const QModelIndex& index)
{
    QModelIndex idx = index.siblingAtColumn(0);
    int ProductID = idx.data(Qt::DisplayRole).toInt();

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
   if (!query.next()) {
       qDebug() << "SmartBill: on_invoicesReportTableView_doubleClicked(): " << query.lastError();
   }

   Invoice* invoice = new Invoice(query.record());
   invoice->open();
}

void SmartBill::setCellContent(QTextTable* table, int row, int col, QString string, QTextCursor* cursor)
{
    QTextTableCell cell = table->cellAt(row, col);
    QTextCursor cellCursor = cell.firstCursorPosition();

    QTextBlockFormat cellBlockFormat(cellCursor.blockFormat());
    QTextCharFormat cellCharFormat (cellCursor.charFormat());
    cellCharFormat.setFontFamily("Cambria");
    cellCharFormat.setFontPointSize(10);
    cellCharFormat.setFontWeight(10);
    if (row == 0) cellBlockFormat.setBackground(QBrush(Qt::gray));
    cellBlockFormat.setAlignment(Qt::AlignHCenter);
    cellCursor.setCharFormat(cellCharFormat);
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
    invoiceReportDialog->setFixedWidth(600);

    invoiceReportTextEdit = new QTextEdit(invoiceReportDialog);
    invoiceReportTextEdit->setGeometry(0, 0,600, 660);
    invoiceReportTextEdit->setFixedWidth(600);

    QPushButton* btn = new QPushButton("Print", invoiceReportDialog);
    btn->setGeometry(5, 665, 70, 30);

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
    charFormat.setFontPointSize(9);
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
    tableFormat.setCellSpacing(0);
    tableFormat.setBorderBrush(QBrush(Qt::SolidLine));
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

    cursor.insertText(tr("\n\n\n"));

    cursor.movePosition(QTextCursor::NextBlock);
    charFormat.setFontFamily("Cambria");
    charFormat.setFontPointSize(9);
    charFormat.setFontWeight(9);
    cursor.mergeCharFormat(charFormat);

    cursor.insertText(tr("Bill Amount: ") + query.value(7).toString() + "\n");
    cursor.insertText(tr("GST: ") + query.value(8).toString() + "\n");
    cursor.insertText(tr("Ship Amount: ") + query.value(9).toString() + "\n");
    cursor.insertText(tr("Total Amount: ") + QString::number(query.value(7).toDouble() + query.value(8).toDouble() + query.value(9).toDouble()) + "\n");
    cursor.insertText(tr("Paid Amount: ") + query.value(10).toString() + "\n");
    cursor.insertText(tr("\n"));
    cursor.insertText(tr("Date issued: ") + query.value(4).toString() + "\n");
    cursor.insertText(tr("Date due: ") + query.value(5).toString() + "\n");
    cursor.insertText(tr("Bill issued: ") + QDateTime::currentDateTime().toString() + "\n");
    cursor.insertText(tr("\n\n"));

    blockFormat.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(blockFormat);

    cursor.insertText("Thank You. :)");

    query.finish();

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

    QString remainingAmount = "((BillingAmount + GstAmount + ShipAmount) - paidAmount)";
    QString sql;

    QSqlQuery query;
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
    else {}
    if (!query.exec(sql)) {
        qDebug() << "SmartBill: on_paymentStatusComboBox_activated(): " << query.lastError();
    }

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
