#ifndef INVOICE_H
#define INVOICE_H

#include "smartbilldb.h"

#include <QDialog>

#include <QMessageBox>
#include <QCompleter>

#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class Invoice;
}

class Invoice : public QDialog
{
    Q_OBJECT

public:
    explicit Invoice(smartbilldb& fbdb, QWidget *parent = nullptr);
    explicit Invoice(smartbilldb& fbdb, QString, QString, double, double, double, QDate, QDate, QString, int, QWidget *parent = nullptr);
    ~Invoice();

    void initializeInvoiceWindow(Ui::Invoice*, smartbilldb&);
    bool validateInvoice() const;
    bool initializeInvoiceData();

private slots:

    void on_addProductPushButton_clicked();

    void on_submitPushButton_clicked();

    void on_billingAmountDoubleSpinBox_valueChanged(double);

    void on_gstAmountDoubleSpinBox_valueChanged(double);

    void on_shipAmountDoubleSpinBox_valueChanged(double);

private:
    Ui::Invoice *ui;
    std::unique_ptr<QCompleter> completer;
    std::unique_ptr<QSqlQuery> query;
    int numberOfProductsAdded = 0;
    bool doneInvoice = false;

    /* Invoice Data */
    QString clientName;
    QString clientAddress;
    double billingAmount;
    double gstAmount;
    double shipAmount;
    QDate issueDate;
    QDate dueDate;
    QString productList;

    /* Json Helpers */
    QJsonObject productListJSONobj;
    std::unique_ptr<QJsonDocument> productListJSONdoc;

    /* Previous Value holders for Amounts */
    double prevBillingAmount = 0;
    double prevGstAmount = 0;
    double prevShipAmount = 0;

     bool updateInvoice = false;
     int toBeUpdatedinvoiceID = 0;

};

#endif // INVOICE_H;
