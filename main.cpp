#include "fastbill.h"
#include "login.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login* login = new Login();
    login->show();
    return a.exec();
}
