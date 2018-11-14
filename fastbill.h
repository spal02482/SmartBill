#ifndef FASTBILL_H
#define FASTBILL_H

#include <QWidget>

namespace Ui {
class FastBill;
}

class FastBill : public QWidget
{
    Q_OBJECT

public:
    explicit FastBill(QWidget *parent = nullptr);
    ~FastBill();

private:
    Ui::FastBill *ui;
};

#endif // FASTBILL_H
