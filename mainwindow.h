#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QFileDialog>
#include "levelcanvas.h"
#include "savewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_gridSpin_valueChanged(int arg1);

    void on_mapHeight_sliderReleased();

    void on_mapWidth_sliderMoved(int position);

    void on_mapWidth_sliderReleased();

    void on_mapWidth_valueChanged(int value);

    void on_mapHeight_sliderMoved(int position);

    void on_mapHeight_valueChanged(int value);

    void on_saveButton_released();

    void on_loadButton_released();

    void on_checkBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    LevelCanvas *levelCanvas_;
    QString itemText_;

    void setSelectedItem();
};
#endif // MAINWINDOW_H
