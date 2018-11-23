#ifndef FASTBILLDB_H
#define FASTBILLDB_H

#include <QtSql>
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QVariant>

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
