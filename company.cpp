#include "company.h"
#include "ui_company.h"

company::company(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::company)
{
    /*
    CREATE TABLE "CompanyInfo" (
        "name"	text,
        "address"	text,
        "city"	text,
        "state"	text,
        "country"	text,
        "pincode"	text,
        "pan"	text,
        "email"	text,
        "website"	text,
        "cin"	text,
        "gstin"	text,
        "taxationtype"	text,
        "contact1"	text,
        "contact2"	text
    )*/

    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    qDebug() << "Company: Company Constructor: " << "Company Window constructed";

    ui->taxationtypeComboBox->addItem("GST");
    ui->taxationtypeComboBox->addItem("Composition Scheme");

    QSqlQuery query;
    query.prepare("SELECT * FROM CompanyInfo");
    query.exec();
    if (!query.next())
        qDebug() << "Company: Company Constructor: " << query.lastError();

    ui->nameLineEdit->setText(query.value(0).toString());
    ui->addressLineEdit->setText(query.value(1).toString());
    ui->cityLineEdit->setText(query.value(2).toString());
    ui->stateLineEdit->setText(query.value(3).toString());
    ui->countryLineEdit->setText(query.value(4).toString());
    ui->pincodeLineEdit->setText(query.value(5).toString());
    ui->panLineEdit->setText(query.value(6).toString());
    ui->emailLineEdit->setText(query.value(7).toString());
    ui->websiteLineEdit->setText(query.value(8).toString());
    ui->cinLineEdit->setText(query.value(9).toString());
    ui->gstinLineEdit->setText(query.value(10).toString());
    ui->taxationtypeComboBox->setCurrentText(query.value(11).toString());
    ui->contact1LineEdit->setText(query.value(12).toString());
    ui->contact2LineEdit->setText(query.value(13).toString());

}

company::~company()
{
    delete ui;
    qDebug() << "Company: Company Constructor: " << "Company Window destroyed";
}

void company::on_updateCompanyInfoPushButton_clicked()
{
    QSqlQuery query;
    query.prepare("UPDATE CompanyInfo SET name = ?, address = ?, city = ?, state = ?, country = ?, pincode = ?, pan = ?, email = ?, website = ?, cin = ?, gstin = ?, taxationtype = ?, contact1 = ?, contact2 = ?");
    query.addBindValue(ui->nameLineEdit->text());
    qDebug() << ui->nameLineEdit->text();
    query.addBindValue(ui->addressLineEdit->text());
    query.addBindValue(ui->cityLineEdit->text());
    query.addBindValue(ui->stateLineEdit->text());
    query.addBindValue(ui->countryLineEdit->text());
    query.addBindValue(ui->pincodeLineEdit->text());
    query.addBindValue(ui->panLineEdit->text());
    query.addBindValue(ui->emailLineEdit->text());
    query.addBindValue(ui->websiteLineEdit->text());
    query.addBindValue(ui->cinLineEdit->text());
    query.addBindValue(ui->gstinLineEdit->text());
    query.addBindValue(ui->taxationtypeComboBox->currentText());
    query.addBindValue(ui->contact1LineEdit->text());
    query.addBindValue(ui->contact2LineEdit->text());
    query.exec();
    qDebug() << "Company: on_updateCompanyInfoPushButton_clicked(): " << query.lastError();
    this->close();
}
