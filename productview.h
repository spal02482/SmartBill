#ifndef PRODUCTVIEW_H
#define PRODUCTVIEW_H

#include "smartbilldb.h"

#include <QDialog>

namespace Ui {
class ProductView;
}

class ProductView : public QDialog
{
    Q_OBJECT

public:
    explicit ProductView(smartbilldb &fbdb, QWidget *parent = nullptr);
    ~ProductView();

private:
    Ui::ProductView *ui;
    std::unique_ptr<QSqlQueryModel> model;
};

#endif // PRODUCTVIEW_H
