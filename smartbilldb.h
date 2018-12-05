#ifndef SMARTBILLDB_H
#define SMARTBILLDB_H

#include <QtDebug>

#include <QSqlDatabase>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include <memory> // for using unique_ptr

class smartbilldb
{
public:
    smartbilldb();
    ~smartbilldb();
    void createConnection();
    QSqlDatabase getConnection();
    void closeConnection();
private:
    QSqlDatabase db;
};

#endif // SMARTBILLDB_H
