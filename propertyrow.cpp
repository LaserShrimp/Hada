#include "propertyrow.h"
#include "ui_propertyrow.h"

PropertyRow::PropertyRow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PropertyRow)
{
    ui->setupUi(this);
}

PropertyRow::PropertyRow(QWidget *parent, const QString& propertyName, const QString& propertyValue)
    : QWidget(parent)
    , ui(new Ui::PropertyRow)
{
    ui->setupUi(this);
    ui->label->setText(propertyName);
    ui->lineEdit->setText(propertyValue);
}

PropertyRow::~PropertyRow()
{
    delete ui;
}
