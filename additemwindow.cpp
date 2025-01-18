#include "additemwindow.h"
#include "ui_additemwindow.h"

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

