#include "invoice.h"
#include "ui_invoice.h"

Invoice::Invoice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Invoice)
{
    ui->setupUi(this);
    QDialog::setAttribute(Qt::WA_DeleteOnClose);
    ui->productNameComboBox->addItem("Suraj");
    ui->productNameComboBox->addItem("Suraj");
    ui->productNameComboBox->addItem("Suraj");
    ui->productNameComboBox->addItem("Suraj");
}

Invoice::~Invoice()
{
    delete ui;
}

void Invoice::on_closeInvoicePushButton_clicked()
{
    this->close();
}
