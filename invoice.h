#ifndef INVOICE_H
#define INVOICE_H

#include "fastbilldb.h"

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Invoice;
}

class Invoice : public QDialog
{
    Q_OBJECT

public:
    explicit Invoice(fastbilldb& fbdb, QWidget *parent = nullptr);
    ~Invoice();

    bool validateInvoice(QString, int, double, double, double, QDate, QDate) const;

private slots:

    void on_closeInvoicePushButton_clicked();

    void on_addProductPushButton_clicked();

private:
    Ui::Invoice *ui;
    QCompleter* completer;
    QSqlQuery query;
};

#endif // INVOICE_H
