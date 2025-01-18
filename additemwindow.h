#ifndef ADDITEMWINDOW_H
#define ADDITEMWINDOW_H

#include <QDialog>
#include <nlohmann/json.hpp>
#include <iostream>

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

private:
    Ui::AddItemWindow *ui;
};

#endif // ADDITEMWINDOW_H
