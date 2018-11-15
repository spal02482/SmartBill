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
    void on_addInvoice_clicked();

    void on_closeButton_clicked();

    void on_invoicesReport_clicked();

private:
    Ui::FastBill *ui;
};

#endif // FASTBILL_H
