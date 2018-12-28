#ifndef SMARTBILL_H
#define SMARTBILL_H

#include <QtCore/QDebug>
#include <QtGui/QValidator>
#include <QtGui/QTextTableCell>
#include <QtWidgets/QLayout>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlRecord>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>

#include <cctype>
#include <memory>

#include "invoice.h"
#include "product.h"
#include "company.h"

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
    void setCellContent(QTextTable*, int, int, QString, QTextCursor*);

private slots:
    void on_addInvoicePushButton_clicked();
    void on_addProductPushButton_clicked();
    void on_viewProductPushButton_clicked();
    void updateProductInfo(const QModelIndex&);
    void on_invoiceIDLineEditSearch_editingFinished();
    void on_clientNameLineEditSearch_editingFinished();
    void on_issueDateDateEditSearch_editingFinished();
    void on_dueDateDateEditSearch_editingFinished();
    void on_refreshPushButton_clicked();
    void on_invoicesReportTableView_doubleClicked(const QModelIndex&);
    void on_openInvoiceReportPushButton_clicked();
    void printInvoiceReport();
    void on_changeCompanyInfoPushButton_clicked();
    void submitCompanyInfo();
    void on_paymentStatusComboBox_activated(const QString&);

private:
    Ui::SmartBill *ui;
    std::unique_ptr<QSqlQueryModel> model;
    /* For adding new Invoices */
    Invoice* invoice;

    /* For adding new Products */
    Product* product;

    /* For changing Company Info */
    company* companyInfo;

    /* For viewing added Products */
    QTableView *productViewTableView;
    QDialog* ProductView;
    std::unique_ptr<QSqlQueryModel> modelProductView;

    std::unique_ptr<QCompleter> completer;

    QDialog* invoiceReportDialog;
    QTextEdit* invoiceReportTextEdit;

    /* Company Info */
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
