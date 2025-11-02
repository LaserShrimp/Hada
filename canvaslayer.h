#ifndef CANVASLAYER_H
#define CANVASLAYER_H

#include <QCoreApplication>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include "item.h"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace Ui {
class CanvasLayer;
}

class Tile : public QPushButton
{
    Q_OBJECT
private:
    static long objectCounter_;
    long id_{0};
    std::string type_{""};
    QWidget* foreground_;

public:
    Tile(QWidget *parent = nullptr);
    std::string type() const { return type_; }
    void setType(std::string type) { type_ = type; }
    long id() { return id_; }
    long objectCounter() { return objectCounter_; }
    void setFixedSize(int w, int h);

private slots:
    void onClicked()
    {
        id_ = -1;
        hide();
    }
};

class CanvasLayer : public QWidget
{
    Q_OBJECT
private:
    std::map<long, Tile *> itemsOnMap_;
    Item selectedItem_{};
    int gridInterval_{50};
    bool grid_{true};
    QString projectPath_{""};
    std::string name_{""};
    Ui::CanvasLayer *ui;

    void placeItemOnMap(
        int const x, int const y, int const width, int const height, QString const strType);

public:
    CanvasLayer(QWidget *parent = nullptr);
    ~CanvasLayer();

    void setName(std::string name) {name_ = name;}
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void loadLayer(std::string level);
    void cleanItems();
    void setProjectPath(QString path) { projectPath_ = path; }
    Tile *loadTile(QWidget *parent,
                   int const x,
                   int const y,
                   int const width,
                   int const height,
                   QString const strType);
    void resetMap();

    std::string parseToJsonString() const;
    nlohmann::json buildJson() const;

public slots:
    void setSelectedItem(QString name, int width, int height)
    {
        selectedItem_.setName(name.toStdString());
        selectedItem_.setWidth(width);
        selectedItem_.setHeight(height);
    }
    void setSelectedItem(Item &item)
    {
        setSelectedItem(QString::fromStdString(item.name()), item.width(), item.height());
    }

    void setGrid(bool enable) { grid_ = enable; update();} //update works but provokes a weird behaviour when loading a map
    void setGridInterval(int newInterval) { gridInterval_ = newInterval; }
};

#endif // CANVASLAYER_H
