#ifndef PRODUCTVIEW_H
#define PRODUCTVIEW_H

#include "fastbilldb.h"

#include <QDialog>

namespace Ui {
class ProductView;
}

class ProductView : public QDialog
{
    Q_OBJECT

public:
    explicit ProductView(fastbilldb &fbdb, QWidget *parent = nullptr);
    ~ProductView();

private:
    Ui::ProductView *ui;
    QSqlQueryModel *model;
};

#endif // PRODUCTVIEW_H
