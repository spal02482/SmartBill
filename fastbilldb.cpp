#include "fastbilldb.h"

fastbilldb::fastbilldb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString databasePath = "C:/Users/Suraj/Projects/FastBill/FastBill/fastbill.db";
    db.setDatabaseName(databasePath);
}

fastbilldb::~fastbilldb()
{
    qDebug() << "Removed Database Connection";
    QSqlDatabase::removeDatabase("");
}

void fastbilldb::createConnection()
{
    if (db.open()) {
        qDebug() << "Connection Successfully Created";
    }
    else {
        qDebug() << "Connection to Database Failed";
        qDebug() << db.lastError();
    }
}

QSqlDatabase fastbilldb::getConnection()
{
    return QSqlDatabase::database();
}

void fastbilldb::closeConnection()
{
    db.close();
}
