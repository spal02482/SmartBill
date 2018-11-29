#include "fastbill.h"
#include "ui_fastbill.h"

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
    delete ui;
    qDebug() << "Closed Database Connection\n" << "Deleting FastBill Window";
}


void FastBill::on_addInvoicePushButton_clicked()
{
    invoice = new Invoice(fbdb, this);
    invoice->setFixedSize(QSize(630, 363));
    invoice->open();
}

void FastBill::on_addProductPushButton_clicked()
{
    product = new Product(fbdb, this);
    product->setFixedSize(QSize(455, 275));
    product->open();
}

void FastBill::on_viewProductPushButton_clicked()
{
    productView = new ProductView(fbdb, this);
    productView->setFixedSize(QSize(605, 500));
    productView->open();
}
