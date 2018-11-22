#ifndef LOGIN_H
#define LOGIN_H

#include <iostream>

#include <QDialog>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>


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
};

#endif // LOGIN_H
