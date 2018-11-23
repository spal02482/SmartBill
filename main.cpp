#include "login.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login* login = new Login();
    login->open();
    qDebug() << a.topLevelWidgets();
    return a.exec();
}
