#include "fastbilldb.h"

fastbilldb::fastbilldb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/surajpal/projects/FastBill/FastBill/fastbill.db");
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
