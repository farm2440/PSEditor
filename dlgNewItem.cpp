#include "dlgNewItem.h"
#include "ui_dlgNewItem.h"

#include <QRegExp>
#include <QValidator>
#include <QRegExpValidator>

DlgNewItem::DlgNewItem(QWidget *parent) :    QDialog(parent),    ui(new Ui::DlgNewItem)
{
    ui->setupUi(this);

    QRegExp regExp("^([0-9,a-z,A-Z,а-я,А-Я,-,_])+$");
    QValidator *vali = new QRegExpValidator(regExp);
    ui->lineEdit->setValidator(vali);
}

DlgNewItem::~DlgNewItem()
{
    delete ui;
}

QString DlgNewItem::getItemName()
{
    _itemName = ui->lineEdit->text().trimmed();

    return _itemName;
}

void DlgNewItem::on_pushButton_clicked()
{
    this->accept();
}

void DlgNewItem::on_pushButton_2_clicked()
{
    this->reject();
}
