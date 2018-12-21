#ifndef LOGIN_H
#define LOGIN_H

#include "smartbill.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_LoginBtn_clicked();

private:
    Ui::Login *ui;
    std::unique_ptr<QSqlDatabase> db;
    SmartBill* smartbill;
};

#endif // LOGIN_H
