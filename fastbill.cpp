#include "fastbill.h"
#include "ui_fastbill.h"
#include "invoice.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

FastBill::FastBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FastBill)
{
    ui->setupUi(this);
}

FastBill::~FastBill()
{
    delete ui;
}

void FastBill::on_addInvoice_clicked()
{
    Invoice* invoice = new Invoice();
    invoice->setWindowModality(Qt::ApplicationModal);
    invoice->open();
}

void FastBill::on_closeButton_clicked()
{
    this->close();
}

void FastBill::on_invoicesReport_clicked()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery* query = new QSqlQuery();
    query->prepare("select * from ProductInfo");
    query->exec();
    model->setQuery(*query);
    ui->invoicesTable->setModel(model);
}
