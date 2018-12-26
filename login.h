#ifndef LOGIN_H
#define LOGIN_H

#include <QtCore/QDebug>
#include <QtGui/QScreen>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <memory>

#include "smartbill.h"

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
    void on_changeCredPushButton_clicked();
    void submitLoginInfo();

private:
    Ui::Login *ui;
    std::unique_ptr<QSqlDatabase> db;
    SmartBill* smartbill;

    /* Old and New username and password changing */
    QDialog* changeCredentials;
    QLineEdit *oldusername, *oldpassword, *newusername, *newpassword;
    QPushButton *submitInfo;

};

#endif // LOGIN_H
