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

};

class LevelCanvas : public QScrollArea
{
    Q_OBJECT
private:
    std::map<long, Tile*> itemsOnMap_;
    QString selectedItem_{"Current Item"};
    int gridInterval_{50};
    bool grid_{true};
public:
    LevelCanvas();
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void loadLevel(std::string level);

    std::string parseToJson() const;

public slots:
    void setSelectedItem(QString item) {
        selectedItem_ = item;
    }

    void setGrid(bool enable) {grid_ = enable;}
    void setGridInterval(int newInterval){gridInterval_ = newInterval;}
};

#endif // LEVELCANVAS_H
