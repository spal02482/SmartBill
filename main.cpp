#include "login.h"
#include "invoice.h"

#include <QPushButton>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login login;
    login.open();
    qDebug() << a.topLevelWidgets();
    return a.exec();
}

/*
 * QJsonObject
 *      |
 *      | Pass to QJsonDocument() Constructor.
 *      |
 * QJsonDocument
 *      |
 *      | toJson() QJsonDocment Non-static Public function.
 *      |
 * QByteArray(i.e. utf-8 text, can be assigned to QString)
 *      |
 *      | fromJson() QJsonDocument static public function.
 *      |
 * QJsonDocument
 */

