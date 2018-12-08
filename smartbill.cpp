#include "smartbill.h"
#include "ui_smartbill.h"


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

SmartBill::SmartBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmartBill)
{
    selectQueryParam = "InvoiceID, ClientName, ClientAddress, ProductList, IssueDate, DueDate, BillingAmount,"
            "GstAmount, ShipAmount, (BillingAmount + GstAmount + ShipAmount)";
    ui->setupUi(this);
    fbdb.createConnection();
    model = std::make_unique<QSqlQueryModel>(this);
    model->setQuery("SELECT " + selectQueryParam +  " FROM InvoiceInfo WHERE isDeleted = 0", fbdb.getConnection());
    ui->invoicesReportTableView->setModel(model.get());

    QSqlQuery query;
    query.exec("SELECT ClientName FROM InvoiceInfo");
    QStringList clientNamesList;
    while (query.next()) {
        clientNamesList << query.value(0).toString();
    }
    completer = std::make_unique<QCompleter>(clientNamesList);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->clientNameLineEditSearch->setCompleter(completer.get());
    query.finish();

    ui->issueDateDateEditSearch->setDate(QDate::currentDate());
    ui->dueDateDateEditSearch->setDate(QDate::currentDate());
}

SmartBill::~SmartBill()
{
    fbdb.closeConnection();
    delete ui;
    qDebug() << "Closed Database Connection\n" << "Deleting SmartBill Window";
}


void SmartBill::on_addInvoicePushButton_clicked()
{
    invoice = new Invoice(fbdb, this);
    invoice->setFixedSize(QSize(630, 363));
    invoice->open();
}

void SmartBill::on_addProductPushButton_clicked()
{
    product = new Product(fbdb, this);
    product->setFixedSize(QSize(455, 275));
    product->open();
}

void SmartBill::on_clientNameLineEditSearch_editingFinished()
{
    QString clientName = ui->clientNameLineEditSearch->text();
    qDebug() << clientName;
    if (clientName != "") {
       QSqlQuery query(fbdb.getConnection());
       query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE ClientName = ? and isDeleted = 0");
       query.addBindValue(clientName);
       query.exec();
       model->setQuery(query);
    }
}

void SmartBill::on_invoiceIDLineEditSearch_editingFinished()
{
    int invoiceID = ui->invoiceIDLineEditSearch->text().toInt();
    if (invoiceID > 0) {
        QSqlQuery query(fbdb.getConnection());
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE InvoiceID = ? and isDeleted = 0");
        query.addBindValue(invoiceID);
        query.exec();
        model->setQuery(query);
        /* Don't do query.finish(). */
    }
}

void SmartBill::on_issueDateDateEditSearch_editingFinished()
{
    QDate issueDate = ui->issueDateDateEditSearch->date();
    if (!issueDate.isNull()) {
        QSqlQuery query(fbdb.getConnection());
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE IssueDate = ? and isDeleted = 0");
        query.addBindValue(issueDate.toString());
        qDebug() << "issueDateSearch: " << issueDate.toString();
        query.exec();
        model->setQuery(query);
        /* Don't do query.finish(). */
    }
}

void SmartBill::on_dueDateDateEditSearch_editingFinished()
{
    QDate dueDate = ui->dueDateDateEditSearch->date();
    if (!dueDate.isNull()) {
        QSqlQuery query(fbdb.getConnection());
        query.prepare("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE DueDate = ? and isDeleted = 0");
        query.addBindValue(dueDate.toString());
        qDebug() << "dueDateSearch: " << dueDate.toString();
        query.exec();
        model->setQuery(query);
        /* Don't do query.finish(). */
    }
}

void SmartBill::on_clearSearchPushButton_clicked()
{
    ui->invoiceIDLineEditSearch->clear();
    ui->clientNameLineEditSearch->clear();
    ui->issueDateDateEditSearch->setDate(QDate::currentDate());
    ui->dueDateDateEditSearch->setDate(QDate::currentDate());
    model->setQuery("SELECT " + selectQueryParam + " FROM InvoiceInfo WHERE isDeleted = 0", fbdb.getConnection());
}

void SmartBill::on_invoicesReportTableView_doubleClicked(const QModelIndex &index)
{
   QModelIndex idx = index.siblingAtColumn(0);
   int InvoiceID = idx.data(Qt::DisplayRole).toInt();
   QSqlQuery query(fbdb.getConnection());
   query.prepare("SELECT * FROM InvoiceInfo WHERE InvoiceID = ?");
   query.addBindValue(InvoiceID);
   query.exec();

   QString clientName;
   QString clientAddress;
   QString productList;
   double billingAmount = 0.0;
   double gstAmount = 0.0;
   double shipAmount = 0.0;
   QDate issueDate;
   QDate dueDate;

   if (query.next()) {
       clientName = query.value(1).toString();
       clientAddress = query.value(2).toString();
       productList = query.value(3).toString();
       issueDate = QDate::fromString(query.value(4).toString(), "ddd MMM dd yyyy");
       dueDate = QDate::fromString(query.value(5).toString(), "ddd MMM dd yyyy");
       billingAmount = query.value(6).toDouble();
       gstAmount = query.value(7).toDouble();
       shipAmount = query.value(8).toDouble();
   }

   Invoice* invoice = new Invoice(fbdb, clientName, clientAddress, billingAmount, gstAmount, shipAmount, issueDate, \
                                  dueDate, productList, InvoiceID, this);
   invoice->open();
}

void SmartBill::on_openInvoiceReportPushButton_clicked()
{
    QModelIndex index = ui->invoicesReportTableView->currentIndex();
    QModelIndex idx = index.siblingAtColumn(0);
    int InvoiceID = idx.data(Qt::DisplayRole).toInt();
    QSqlQuery query(fbdb.getConnection());
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
        issueDate = query.value(4).toString();
        dueDate = query.value(5).toString();
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
        QSqlQuery query(fbdb.getConnection());
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

void SmartBill::on_viewProductPushButton_clicked()
{
    qDebug() << "view product push button clicked";
    QDialog* ProductView = new QDialog(this);
    if (ProductView->objectName().isEmpty())
        ProductView->setObjectName(QStringLiteral("ProductView"));
    ProductView->setWindowModality(Qt::ApplicationModal);
    ProductView->resize(710, 500);
    QFont font;
    font.setFamily(QStringLiteral("Candara"));
    font.setPointSize(10);
    ProductView->setFont(font);
    QIcon icon;
    icon.addFile(QStringLiteral(":/Icons/smartbill.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
    ProductView->setWindowIcon(icon);
    productViewTableView = new QTableView(ProductView);
    productViewTableView->setObjectName(QStringLiteral("productViewTableView"));
    productViewTableView->setGeometry(QRect(0, 0, 700, 500));
    productViewTableView->setAutoScrollMargin(15);
    productViewTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    productViewTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    productViewTableView->setShowGrid(true);
    productViewTableView->verticalHeader()->setVisible(false);

    modelProductView = std::make_unique<QSqlQueryModel>(ProductView);
    QSqlQuery query(fbdb.getConnection());
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
    productViewTableView->setColumnWidth(5, 200);

    ProductView->open();
}


