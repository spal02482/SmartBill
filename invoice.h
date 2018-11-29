#ifndef INVOICE_H
#define INVOICE_H

#include "fastbilldb.h"

#include <QDialog>

#include <QMessageBox>
#include <QCompleter>

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

    void on_addProductPushButton_clicked();

    void on_submitPushButton_clicked();

    void on_doneInvoicePushButton_clicked();

private:
    Ui::Invoice *ui;
    QCompleter* completer;
    QSqlQuery query;
    int numberOfProductsAdded = 0;
    bool doneInvoice = false;
};

#endif // INVOICE_H
