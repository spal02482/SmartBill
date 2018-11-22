#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->UserName->setPlaceholderText(QString("Username Here"));
    ui->PassWord->setPlaceholderText((QString("Password Here")));
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/surajpal/projects/FastBill/FastBill/fastbill.db");
    if (db.open()) {
        qDebug() << "Connected\n";
    }
    else {
        qDebug() << "Connection Failed\n";
    }
}

Login::~Login()
{
    db.close();
    delete ui;
}

void Login::on_LoginBtn_clicked()
{
    QString username = ui->UserName->text();
    QString password = ui->PassWord->text();
    qDebug() << username << " " << password;

    QSqlQuery query;
    query.prepare("select PassWord from UserInfo where UserName = ?");
    query.addBindValue(username);
    bool querySuccess = query.exec();

    if (querySuccess == true) {
        query.next();
        if (query.isValid()) {
            if (query.value(0).toString() == password) {
                fastbill = new FastBill();
                fastbill->setWindowModality(Qt::ApplicationModal);
                fastbill->show();
                this->close();
            }
            else {
                QMessageBox::critical(nullptr, QString("Login"), QString("Wrong Password"));
            }
        }
        else {
            QMessageBox::critical(nullptr, QString("Login"), QString("Login failed"));
        }
    }
}
