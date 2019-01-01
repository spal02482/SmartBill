#include "company.h"
#include "ui_company.h"

company::company(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::company)
{
    ui->setupUi(this);
    qDebug() << "Company: Company Constructor: " << "Company Window constructed";

    this->setAttribute(Qt::WA_DeleteOnClose);

    QStringList states({"Andra Pradesh", "Arunachal Pradesh", "Assam", "Bihar", "Chhattisgarh", "Goa", "Gujarat", "Haryana",
                       "Himachal Pradesh", "Jammu & Kashmir", "Jharkhand", "Karnataka", "Kerala", "Madya Pradesh", "Maharashtra",
                       "Manipur", "Meghalaya", "Mizoram", "Nagaland", "Orissa", "Punjab", "Rajasthan", "Sikkim", "Tamil Nadu",
                       "Telagana", "Tripura", "Uttaranchal", "Uttar Pradesh", "West Bengal", "Andaman & Nicobar", "Chandigarh",
                       "Dadar & Nagar Haveli", "Daman & Diu", "Delhi", "Lakshadeep", "Pondicherry"});

    ui->taxationtypeComboBox->addItem("GST");
    ui->taxationtypeComboBox->addItem("Composition Scheme");

    for (auto i = 0; i < states.size(); ++i) {
        ui->stateComboBox->addItem(states.at(i));
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM CompanyInfo");
    query.exec();
    if (!query.next()) {
        qDebug() << "Company: Company Constructor: " << query.lastError();
        return;
    }

    ui->nameLineEdit->setText(query.value(0).toString());
    ui->addressLineEdit->setText(query.value(1).toString());
    ui->districtComboBox->setCurrentText(query.value(2).toString());
    ui->stateComboBox->setCurrentText(query.value(3).toString());
    ui->countryLineEdit->setText(query.value(4).toString());
    ui->cityLineEdit->setText(query.value(5).toString());
    ui->pincodeLineEdit->setText(query.value(6).toString());
    ui->panLineEdit->setText(query.value(7).toString());
    ui->emailLineEdit->setText(query.value(8).toString());
    ui->websiteLineEdit->setText(query.value(9).toString());
    ui->cinLineEdit->setText(query.value(10).toString());
    ui->gstinLineEdit->setText(query.value(11).toString());
    ui->taxationtypeComboBox->setCurrentText(query.value(12).toString());
    ui->contact1LineEdit->setText(query.value(13).toString());
    ui->contact2LineEdit->setText(query.value(14).toString());

}

company::~company()
{
    delete ui;
    qDebug() << "Company: Company Constructor: " << "Company Window destroyed";
}

void company::on_updateCompanyInfoPushButton_clicked()
{
    QSqlQuery query;
    query.prepare("UPDATE CompanyInfo SET name = ?, address = ?, district = ?, state = ?, country = ?, city = ?, pincode = ?, pan = ?, email = ?, website = ?, cin = ?, gstin = ?, taxationtype = ?, contact1 = ?, contact2 = ?");
    query.addBindValue(ui->nameLineEdit->text());
    query.addBindValue(ui->addressLineEdit->text());
    query.addBindValue(ui->districtComboBox->currentText());
    query.addBindValue(ui->stateComboBox->currentText());
    query.addBindValue(ui->countryLineEdit->text());
    query.addBindValue(ui->cityLineEdit->text());
    query.addBindValue(ui->pincodeLineEdit->text());
    query.addBindValue(ui->panLineEdit->text());
    query.addBindValue(ui->emailLineEdit->text());
    query.addBindValue(ui->websiteLineEdit->text());
    query.addBindValue(ui->cinLineEdit->text());
    query.addBindValue(ui->gstinLineEdit->text());
    query.addBindValue(ui->taxationtypeComboBox->currentText());
    query.addBindValue(ui->contact1LineEdit->text());
    query.addBindValue(ui->contact2LineEdit->text());
    if (!query.exec()) {
        qDebug() << "Company: on_updateCompanyInfoPushButton_clicked(): " << query.lastError();
    }
    this->close();
}
