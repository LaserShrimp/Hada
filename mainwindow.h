#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QGridLayout>
#include <vector>
#include "canvaslayer.h"
#include "savewindow.h"
#include "additemwindow.h"
#include "item.h"

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

    void on_mapHeight_sliderMoved(int position);

    void on_saveButton_released();

    void on_loadButton_released();

    void on_checkBox_toggled(bool checked);

    void on_addItemButton_clicked();

    void on_addLayerButton_clicked();

    void on_deleteLayerButton_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<CanvasLayer*> levelCanvas_;
    std::vector<QPushButton*> layerButtons_;
    QString itemText_;
    std::map<std::string, Item> availableItems_;
    int nextPickItemX{0};
    QString projectPath_{""};
    int focusedLayer_{0};
    int mapWidth_{1000};
    int mapHeight_{1000};

    void loadTiles(std::string tilesFileName);
    void addAvailableItem(std::string const name, std::string const imagePath, int const width, int const height);
    void addItemToPickItemArea(QString const name, int const width, int const height);
    void addLayer();
    void addLayer(CanvasLayer* layerToAdd);
    void deleteLayer(int layerToDelete);
    void setFocusOnLayer(int layerToFocus);
    void setMapWidth(int newWidth);
    void setMapHeight(int newHeight);
    void updateCanvasSize();
    void resetProject();
    void resetMap();
};
#endif // MAINWINDOW_H
