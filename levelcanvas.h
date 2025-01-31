#ifndef LEVELCANVAS_H
#define LEVELCANVAS_H

#include <QScrollArea>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QCoreApplication>
#include <iostream>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "item.h"

class Tile : public QPushButton {
    Q_OBJECT
private:
    static long objectCounter_;
    long id_{0};
    std::string type_{""};
public:
    Tile(QWidget *parent = nullptr);
    std::string type()const {return type_;}
    void setType(std::string type) {type_ = type;}
    long id() {return id_;}
    long objectCounter() {return objectCounter_;}

private slots:
    void onClicked() {id_ = -1; hide();}

};

class LevelCanvas : public QScrollArea
{
    Q_OBJECT
private:
    std::map<long, Tile*> itemsOnMap_;
    Item selectedItem_{};
    int gridInterval_{50};
    bool grid_{true};
    QString projectPath_{""};

    void placeItemOnMap(int const x, int const y, int const width, int const height, QString const strType);
public:
    LevelCanvas();
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void loadLevel(std::string level);
    void cleanItems();
    void setProjectPath(QString path) {projectPath_ = path;}

    std::string parseToJson() const;

public slots:
    void setSelectedItem(QString name, int width, int height) {
        selectedItem_.setName(name.toStdString());
        selectedItem_.setWidth(width);
        selectedItem_.setHeight(height);
    }
    void setSelectedItem(Item& item) {
        setSelectedItem(QString::fromStdString(item.name()), item.width(), item.height());
    }

    void setGrid(bool enable) {grid_ = enable;}
    void setGridInterval(int newInterval){gridInterval_ = newInterval;}
};

#endif // LEVELCANVAS_H
