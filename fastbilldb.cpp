#include "fastbilldb.h"

fastbilldb::fastbilldb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString databasePath = "C:/Users/Suraj/Projects/FastBill/FastBill/fastbill.db";
    db.setDatabaseName(databasePath);
}

fastbilldb::~fastbilldb()
{
    QSqlDatabase::removeDatabase("");
    qDebug() << "Removed Database Connection";
}

void fastbilldb::createConnection()
{
    if (db.open()) {
        qDebug() << "Connection Successfully created to Database";
    }
    else {
        qDebug() << "Connection to Database Failed\n" << db.lastError();
    }
}

QSqlDatabase fastbilldb::getConnection()
{
    return QSqlDatabase::database();
}

void fastbilldb::closeConnection()
{
    db.close();
    qDebug() << "Connection to Database closed";
}
