#include "productview.h"
#include "ui_productview.h"

ProductView::ProductView(smartbilldb& fbdb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProductView)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(this);
    QSqlQuery query(fbdb.getConnection());
    query.exec("SELECT ProductID, ProductName, Price, NumberInStock, SupplierName, Description FROM ProductInfo where isDeleted = 0");
    model->setQuery(query);
    ui->productViewTableView->setModel(model);
}

ProductView::~ProductView()
{
    delete model;
    delete ui;
}
