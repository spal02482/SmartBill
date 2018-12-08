#ifndef SMARTBILL_H
#define SMARTBILL_H

#include "invoice.h"
#include "product.h"
#include "smartbilldb.h"

#include <QLayout>
#include <QTableView>

#include <QValidator>
#include <QTextEdit>
#include <cctype>

#include <QPrinter>
#include <QPrintDialog>

namespace Ui {
class SmartBill;
}

class SmartBill : public QWidget
{
    Q_OBJECT

public:
    explicit SmartBill(QWidget *parent = nullptr);
    ~SmartBill();

     QString selectQueryParam;

private slots:
    void on_addInvoicePushButton_clicked();

    void on_addProductPushButton_clicked();

    void on_viewProductPushButton_clicked();

    void on_clientNameLineEditSearch_editingFinished();

    void on_invoiceIDLineEditSearch_editingFinished();

    void on_issueDateDateEditSearch_editingFinished();

    void on_dueDateDateEditSearch_editingFinished();

    void on_clearSearchPushButton_clicked();

    void on_invoicesReportTableView_doubleClicked(const QModelIndex &index);

    void on_openInvoiceReportPushButton_clicked();

    void printInvoiceReport();

private:
    Ui::SmartBill *ui;
    smartbilldb fbdb;
    std::unique_ptr<QSqlQueryModel> model;
    /* For adding new Invoices */
    Invoice* invoice;

    /* For adding new Products */
    Product* product;

    /* For viewing added Products */
    //ProductView* productView;
    QTableView *productViewTableView;
    QDialog* ProductView;
    std::unique_ptr<QSqlQueryModel> modelProductView;

    std::unique_ptr<QCompleter> completer;

    QDialog* invoiceReportDialog;
    QTextEdit* invoiceReportTextEdit;
};

class NameValidator : QValidator
{
    QValidator::State validate(QString&, int&) const;
};

#endif // SMARTBILL_H
