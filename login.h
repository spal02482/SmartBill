#ifndef LOGIN_H
#define LOGIN_H

#include "smartbill.h"
#include "smartbilldb.h"


#include <QMessageBox>

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
    smartbilldb fbdb;
    SmartBill* smartbill;
};

#endif // LOGIN_H
