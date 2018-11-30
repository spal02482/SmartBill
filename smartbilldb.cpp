#include "smartbilldb.h"

smartbilldb::smartbilldb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString databasePath = "C:/Users/Suraj/Projects/SmartBill/SmartBill/smartbill.db";
    db.setDatabaseName(databasePath);
}

smartbilldb::~smartbilldb()
{
    QSqlDatabase::removeDatabase("");
    qDebug() << "Removed Database Connection";
}

void smartbilldb::createConnection()
{
    if (db.open()) {
        qDebug() << "Connection Successfully created to Database";
    }
    else {
        qDebug() << "Connection to Database Failed\n" << db.lastError();
    }
}

QSqlDatabase smartbilldb::getConnection()
{
    return QSqlDatabase::database();
}

void smartbilldb::closeConnection()
{
    db.close();
    qDebug() << "Connection to Database closed";
}
