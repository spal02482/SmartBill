#include "login.h"
#include "invoice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login login;
    login.open();
    qDebug() << a.topLevelWidgets();
    return a.exec();
}
