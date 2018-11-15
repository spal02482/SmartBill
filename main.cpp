#include "fastbill.h"
#include "login.h"

#include <iostream>
#include <QMessageBox>
#include <QApplication>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login* login = new Login();
    int r = login->exec();
    qDebug() << r << "\n";
    if (!r) {
        FastBill* fastbill = new FastBill();
        fastbill->setWindowModality(Qt::ApplicationModal);
        fastbill->show();
    }
    else {
        QMessageBox::critical(nullptr, QString("Login"), QString("Login Failed"));
    }
    return a.exec();
}
