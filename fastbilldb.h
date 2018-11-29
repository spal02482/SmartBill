#ifndef FASTBILLDB_H
#define FASTBILLDB_H

#include <QtDebug>

#include <QSqlDatabase>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlQueryModel>

class fastbilldb
{
public:
    fastbilldb();
    ~fastbilldb();
    void createConnection();
    QSqlDatabase getConnection();
    void closeConnection();
private:
    QSqlDatabase db;
};

#endif // FASTBILLDB_H
