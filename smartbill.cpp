#include "smartbill.h"
#include "ui_smartbill.h"

SmartBill::SmartBill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmartBill)
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

void SmartBill::on_viewProductPushButton_clicked()
{
    productView = new ProductView(fbdb, this);
    productView->setFixedSize(QSize(605, 500));
    productView->open();
}
