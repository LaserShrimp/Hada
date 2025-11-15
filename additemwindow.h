#ifndef ADDITEMWINDOW_H
#define ADDITEMWINDOW_H

#include <QDialog>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>

namespace Ui {
class AddItemWindow;
}

class AddItemWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemWindow(QWidget *parent = nullptr);
    ~AddItemWindow();

    std::string getNewItem() const;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_addPropertyButton_released();

private:
    Ui::AddItemWindow *ui;
    std::vector<std::string> properties_;
};

#endif // ADDITEMWINDOW_H
