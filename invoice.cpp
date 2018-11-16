#include "invoice.h"
#include "ui_invoice.h"

Invoice::Invoice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    ui->setupUi(this);
    QDialog::setAttribute(Qt::WA_DeleteOnClose);
}

Invoice::~Invoice()
{
    delete ui;
}

void Invoice::on_closeInvoicePushButton_clicked()
{
    this->close();
}
