#include "fastbill.h"
#include "ui_fastbill.h"

FastBill::FastBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FastBill)
{
    ui->setupUi(this);
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery* query = new QSqlQuery();
    query->prepare("select * from Report");
    query->exec();
    model->setQuery(*query);
    ui->invoicesReportTableView->setModel(model);
}

FastBill::~FastBill()
{
    delete ui;
}

void FastBill::on_addInvoicePushButton_clicked()
{
    Invoice* invoice = new Invoice();
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
    Product* product = new Product();
    product->setWindowModality(Qt::ApplicationModal);
    product->setWindowTitle("Product");
    product->setFixedSize(QSize(630, 375));
    product->open();
}
