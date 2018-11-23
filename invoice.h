#ifndef INVOICE_H
#define INVOICE_H

#include "fastbilldb.h"

#include <QDialog>

namespace Ui {
class Invoice;
}

class Invoice : public QDialog
{
    Q_OBJECT

public:
    explicit Invoice(fastbilldb& fbdb, QWidget *parent = nullptr);
    ~Invoice();

private slots:

    void on_closeInvoicePushButton_clicked();

private:
    Ui::Invoice *ui;
};

#endif // INVOICE_H
