#include "login.h"
#include "ui_login.h"
#include "fastbill.h"

#include <iostream>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->UserName->setPlaceholderText(QString("Username Here"));
    ui->PassWord->setPlaceholderText((QString("Password Here")));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
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
    delete ui;
}

void Login::on_LoginBtn_clicked()
{
    QString username = ui->UserName->text();
    QString password = ui->PassWord->text();
    std::cout << "print: " << username.toStdString() << " " << password.toStdString() << "\n";
    QSqlQuery query;
    query.prepare("select password from userinfo where username = ?");
    query.addBindValue(username);
    if (query.exec()) {
        qDebug() << "Query Successfull\n";
        while (query.next()) {
            QString databasePassword = query.value(0).toString();
            qDebug() << databasePassword << endl;
            if (databasePassword.toStdString() == password.toStdString()) {
                this->hide();
                FastBill* fastbill = new FastBill();
                fastbill->show();
            }
            else {
                QMessageBox::critical(nullptr, QString("Login"), QString("Login Failed"));
            }
        }
    }
    else {
        qDebug() << query.lastError();
    }
}
