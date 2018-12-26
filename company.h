#ifndef COMPANY_H
#define COMPANY_H

#include <QtCore/QDebug>
#include <QtWidgets/QDialog>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace Ui {
class company;
}

class company : public QDialog
{
    Q_OBJECT

public:
    explicit company(QWidget *parent = nullptr);
    ~company();

private slots:
    void on_updateCompanyInfoPushButton_clicked();

private:
    Ui::company *ui;
};

#endif // COMPANY_H
