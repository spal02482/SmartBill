#ifndef FASTBILL_H
#define FASTBILL_H

#include <QDialog>

namespace Ui {
class FastBill;
}

class FastBill : public QWidget
{
    Q_OBJECT

public:
    explicit FastBill(QWidget *parent = nullptr);
    ~FastBill();

private slots:
    void on_addInvoicePushButton_clicked();

    void on_closePushButton_clicked();

    void on_invoicesReportPushButton_clicked();

private:
    Ui::FastBill *ui;
};

#endif // FASTBILL_H
