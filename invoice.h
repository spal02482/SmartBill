#ifndef INVOICE_H
#define INVOICE_H

#include <QtCore/QDebug>
#include <QtCore/QDate>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtWidgets/QDialog>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlDatabase>

#include <memory>

namespace Ui {
class Invoice;
}

class Invoice : public QDialog
{
    Q_OBJECT

public:
    explicit Invoice(QWidget *parent = nullptr);
    explicit Invoice(QSqlRecord, QWidget *parent = nullptr);
    ~Invoice();

    void initializeInvoiceWindow(Ui::Invoice*);
    bool validateInvoice() const;
    bool collectInvoiceData();

private slots:
    void on_addProductPushButton_clicked();
    void on_submitInvoicePushButton_clicked();
    void on_billingAmountDoubleSpinBox_valueChanged(double);
    void on_gstAmountDoubleSpinBox_valueChanged(double);
    void on_shipAmountDoubleSpinBox_valueChanged(double);
    void on_totalAmountDoubleSpinBox_valueChanged(double);

private:
    Ui::Invoice *ui;
    std::unique_ptr<QCompleter> completer;
    int numberOfProductsAdded = 0;  // This variable is used in validateInvoice() to prevent to store empty product list in db.

    /* Variable to hold Invoice field Data */
    QString clientName;
    QString clientAddress;
    double billingAmount;
    double gstAmount;
    double shipAmount;
    double paidAmount;
    QDate issueDate;
    QDate dueDate;
    QDate dueDate_sqlite;
    QString productList;

    /* Previous Value holders for Amounts */
    double prevBillingAmount = 0;
    double prevGstAmount = 0;
    double prevShipAmount = 0;

    bool updateInvoice = false;     // This is set to 'true' when parameterized Invoice constructor is invoked.
    int toBeUpdatedinvoiceID = 0;   // This gets the Invoice ID of the Invoice which is to be edited and updated in Invoice's ctor.

    /* These variables are used to update the product quantity in stock after making the purchase */
    QVariantList productIdList;
    QVariantList quantityList;

};

#endif // INVOICE_H;
