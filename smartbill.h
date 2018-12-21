#ifndef SMARTBILL_H
#define SMARTBILL_H

#include "invoice.h"
#include "product.h"

#include <QLayout>
#include <QTableView>

#include <QDebug>

#include <QValidator>
#include <QTextEdit>
#include <cctype>

#include <QPrinter>
#include <QPrintDialog>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>

namespace Ui {
class SmartBill;
}

class SmartBill : public QWidget
{
    Q_OBJECT

public:
    explicit SmartBill(QWidget *parent = nullptr);
    ~SmartBill();

    void reloadClientNames();

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

    void submitCompanyInfo();

    void on_changeCompanyInfoPushButton_clicked();

private:
    Ui::SmartBill *ui;
    std::unique_ptr<QSqlQueryModel> model;
    /* For adding new Invoices */
    Invoice* invoice;

    /* For adding new Products */
    Product* product;

    /* For viewing added Products */
    QTableView *productViewTableView;
    QDialog* ProductView;
    std::unique_ptr<QSqlQueryModel> modelProductView;

    std::unique_ptr<QCompleter> completer;

    QDialog* invoiceReportDialog;
    QTextEdit* invoiceReportTextEdit;

    /* Company Info */
    QDialog* companyInfo;
    QLineEdit* companyName;
    QLineEdit* companyEmail;
    QLineEdit* companyCIN;
    QLineEdit* companyAddress;
    QLineEdit* companyContact1;
    QLineEdit* companyContact2;
    QLabel* companyLogo;
    QPushButton* submitCompanyInfoPushButton;
};

class NameValidator : QValidator
{
    QValidator::State validate(QString&, int&) const;
};

#endif // SMARTBILL_H
