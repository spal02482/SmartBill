#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->UserName->setPlaceholderText(QString("Username Here"));
    ui->PassWord->setPlaceholderText((QString("Password Here")));
    fbdb.createConnection();
}

Login::~Login()
{
    fbdb.closeConnection();
    delete ui;
    qDebug() << "Deleting Login Window" << "\nClosing Database Connection";
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
                smartbill = new SmartBill();
                smartbill->setWindowModality(Qt::ApplicationModal);
                smartbill->showMaximized();
                qDebug() << smartbill->size();
                smartbill->show();
                this->close();
            }
            else {
                QMessageBox::critical(this, tr("Login"), tr("Wrong Password"));
            }
        }
        else {
            QMessageBox::critical(this, tr("Login"), tr("Login failed"));
        }
    }
    else {
        qDebug() << query.lastError();
    }

    query.finish();
}
