#ifndef SAVEWINDOW_H
#define SAVEWINDOW_H

#include <QWidget>
#include <QFileDialog>
#include <fstream>
#include <iostream>

namespace Ui {
class SaveWindow;
}

class SaveWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SaveWindow(QWidget *parent = nullptr);
    void setJson(std::string json);
    ~SaveWindow();

private slots:
    void on_ConfirmBox_rejected();

    void on_ConfirmBox_accepted();

private:
    Ui::SaveWindow *ui;
    std::string jsonData_;
};

#endif // SAVEWINDOW_H
