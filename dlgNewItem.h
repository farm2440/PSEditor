#ifndef DLGNEWITEM_H
#define DLGNEWITEM_H

#include <QDialog>

namespace Ui {
class DlgNewItem;
}

class DlgNewItem : public QDialog
{
    Q_OBJECT

public:
    explicit DlgNewItem(QWidget *parent = 0);
    ~DlgNewItem();

    QString getItemName();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DlgNewItem *ui;
    QString _itemName;
};

#endif // DLGNEWITEM_H
