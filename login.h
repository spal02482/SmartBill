#ifndef LOGIN_H
#define LOGIN_H

#include "fastbill.h"

#include <QDialog>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
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
    FastBill* fastbill;
    QSqlDatabase db;
};

#endif // LOGIN_H
