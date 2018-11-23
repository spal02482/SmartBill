#include "fastbill.h"
#include "ui_fastbill.h"

#include "fastbilldb.h"

FastBill::FastBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FastBill)
{
    ui->setupUi(this);

    fbdb.createConnection();

    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query(fbdb.getConnection());
    query.prepare("SELECT * FROM Report");
    query.exec();
    model->setQuery(query);
    ui->invoicesReportTableView->setModel(model);
}

FastBill::~FastBill()
{
    fbdb.closeConnection();
    qDebug() << "Closed Database Connection and Deleting FastBill Window";
    delete ui;
}

void FastBill::on_addInvoicePushButton_clicked()
{
    Invoice* invoice = new Invoice(fbdb, this);
    invoice->setWindowModality(Qt::ApplicationModal);
    invoice->setWindowTitle(QString("Invoice"));
    invoice->setFixedSize(QSize(630, 363));
    invoice->open();
}

void FastBill::on_closeFastBillPushButton_clicked()
{
    this->close();
}


void FastBill::on_addProductPushButton_clicked()
{
    Product* product = new Product(fbdb, this);
    product->setWindowModality(Qt::ApplicationModal);
    product->setWindowTitle("Product");
    product->setFixedSize(QSize(455, 275));
    product->open();
}
