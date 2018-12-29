#include "login.h"
#include "invoice.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login login;
    login.open();
    return a.exec();
}
