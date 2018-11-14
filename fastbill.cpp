#include "fastbill.h"
#include "ui_fastbill.h"

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
