#include "additemwindow.h"
#include "ui_additemwindow.h"
#include "propertyrow.h"
#include <QDebug>

AddItemWindow::AddItemWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddItemWindow)
{
    ui->setupUi(this);
}

std::string AddItemWindow::getNewItem() const{
    nlohmann::json newItem = {
        {"Name", ui->nameEdit->text().toStdString()},
        {"Width", ui->widthSpinBox->value()},
        {"Height", ui->heightSpinBox->value()}};
    return newItem.dump();
}

AddItemWindow::~AddItemWindow()
{
    delete ui;
}

void AddItemWindow::on_buttonBox_accepted()
{
    accept();
}


void AddItemWindow::on_buttonBox_rejected()
{
    reject();
}


void AddItemWindow::on_addPropertyButton_released()
{
    QString propertyName = ui->newProperty->text();
    for(auto& it: properties_) {
        if(it == propertyName.toStdString()) {
            ui->newProperty->setText("");
            qDebug() << "This property already exists";
            return;
        }
    }
    PropertyRow* propertyRow = new PropertyRow(nullptr, propertyName, "");
    properties_.push_back(propertyName.toStdString());
    ui->gridLayout->addWidget(propertyRow, ui->gridLayout->rowCount(), 0, 1, 2);
    ui->newProperty->setText("");
}

