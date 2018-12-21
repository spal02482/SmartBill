#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"));
    db->setDatabaseName(QCoreApplication::applicationDirPath() + "/smartbill.db");
    qDebug() << "Database Connection Added";

    if (db->open()) {
        qDebug() << "Connection opened to Database";
    }
    else {
        qDebug() << "Couldn't open database connection";
    }

    ui->UserName->setPlaceholderText(tr("Username Here"));
    ui->PassWord->setPlaceholderText(tr("Password Here"));
}

Login::~Login()
{
    db->close();
    qDebug() << "Connection to Database closed";

    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    qDebug() << "Database Connection Removed";

    delete ui;
    qDebug() << "Deleting Login Window";
}

void Login::on_LoginBtn_clicked()
{
    QString username = ui->UserName->text();
    QString password = ui->PassWord->text();
    qDebug() << username << " " << password;

    QSqlQuery query;
    query.prepare("SELECT PassWord FROM UserInfo WHERE UserName = ?");
    query.addBindValue(username);
    bool querySuccess = query.exec();

    if (querySuccess == true) {
        query.next();
        if (query.isValid()) {
            if (query.value(0).toString() == password) {
                query.finish();
                smartbill = new SmartBill();
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
        qDebug() << "Login:" << query.lastError();
    }
}
