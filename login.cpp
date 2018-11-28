#include "login.h"
#include "ui_login.h"

#include "fastbilldb.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    QDialog::setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->UserName->setPlaceholderText(QString("Username Here"));
    ui->PassWord->setPlaceholderText((QString("Password Here")));
    fbdb.createConnection();
}

Login::~Login()
{
    fbdb.closeConnection();
    qDebug() << "Deleting Login Window and Closing Database Connection";
    delete ui;
}

void Login::on_LoginBtn_clicked()
{
    QString username = ui->UserName->text();
    QString password = ui->PassWord->text();
    qDebug() << username << " " << password;

    QSqlQuery query(fbdb.getConnection());
    query.prepare("SELECT PassWord FROM UserInfo WHERE UserName = ?");
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
                QMessageBox::critical(this, QString("Login"), QString("Wrong Password"));
            }
        }
        else {
            QMessageBox::critical(this, QString("Login"), QString("Login failed"));
        }
    }
    else {
        qDebug() << query.lastError();
    }

    query.finish();
}
