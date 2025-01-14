#include "levelcanvas.h"

LevelCanvas::LevelCanvas() {}

void LevelCanvas::mousePressEvent(QMouseEvent *event) {
    QPoint clickPos = event->pos();
    clickPos.setX((clickPos.x() / gridInterval_) * gridInterval_);
    clickPos.setY((clickPos.y() / gridInterval_) * gridInterval_);
    Tile *button = new Tile(this);
    QPixmap image("./images/"+selectedItem_+".png");
    QIcon icon(image);
    button->setIcon(icon);
    button->setType(selectedItem_.toStdString());
    button->setStyleSheet("QPushButton {background:transparent; border:none;}");
    std::cout << image.size().width() << std::endl;
    button->setIconSize(image.size());
    button->setFixedSize(image.size().width(), image.size().height());
    button->move(clickPos);
    button->show();
    std::cout << button->size().width() << std::endl;
    itemsOnMap_.push_back(button);
}

void LevelCanvas::paintEvent(QPaintEvent *event) {
    qDebug() << "repainting !";
    if(!grid_) { return; }
    QScrollArea::paintEvent(event);
    QPainter painter(viewport());

    QPen pen(Qt::lightGray);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    int width = this->width();
    int height = this->height();

    for(int i = 0; i < width; i+= gridInterval_) {
        painter.drawLine(i, 0, i, height);
    }

    for(int i = 0; i < height; i+= gridInterval_) {
        painter.drawLine(0, i, width, i);
    }
}

std::string LevelCanvas::parseToJson() const {
    nlohmann::json jsonDoc;
    jsonDoc["MapWidth"] = width();
    jsonDoc["MapHeight"] = height();

    nlohmann::json array = nlohmann::json::array();
    for(auto& it: itemsOnMap_) {
        nlohmann::json newElement = {{"Type", it->type()}, {"Width", it->width()}, {"Height", it->height()}, {"X", it->x()}, {"Y", it->y()}};
        array.push_back(newElement);
    }
    jsonDoc["Map"] = array;

    std::string parsed = jsonDoc.dump();

    return parsed;
}

using namespace nlohmann;

void LevelCanvas::loadLevel(std::string level) {
    nlohmann::json json;
    json = nlohmann::json::parse(level);
    setFixedWidth(json["MapWidth"]);
    setFixedHeight(json["MapHeight"]);
    for(auto& item: json["Map"]) {
        Tile *button = new Tile(this);
        QString strType = QString::fromStdString(item["Type"].get<std::string>());
        QPixmap image("./images/"+strType+".png");
        QIcon icon(image);
        button->setIcon(icon);
        button->setType(item["Type"].get<std::string>());
        QSize size(item["Width"].get<int>(), item["Height"].get<int>());
        button->setIconSize(size);
        button->setFlat(true);
        button->move(item["X"].get<int>(), item["Y"].get<int>());
        button->setStyleSheet("QPushButton {background:transparent; border:none;}");
        button->setFixedSize(image.size().width(), image.size().height());
        button->show();
        itemsOnMap_.push_back(button);
    }
}

Tile::Tile(QWidget *parent): QPushButton(parent){}