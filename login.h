#ifndef LOGIN_H
#define LOGIN_H

#include "fastbill.h"
#include "fastbilldb.h"

#include <QApplication>
#include <QMessageBox>
#include <QtDebug>

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
    fastbilldb fbdb;
    FastBill* fastbill;
};

#endif // LOGIN_H
