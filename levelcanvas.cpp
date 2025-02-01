#include "levelcanvas.h"

LevelCanvas::LevelCanvas() {}

void LevelCanvas::mouseReleaseEvent(QMouseEvent *event) {
    QPoint clickPos = event->pos();
    if(grid_) {
        clickPos.setX((clickPos.x() / gridInterval_) * gridInterval_);
        clickPos.setY((clickPos.y() / gridInterval_) * gridInterval_);
    }
    QString strType = QString::fromStdString(selectedItem_.name());
    placeItemOnMap(clickPos.x(), clickPos.y(), selectedItem_.width(), selectedItem_.height(), strType);
    qDebug() << "item added" << QString::fromStdString(selectedItem_.name()) << " " << selectedItem_.width() << " " << selectedItem_.height();
}

void LevelCanvas::paintEvent(QPaintEvent *event) {
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

/**
 * @brief LevelCanvas::parseToJson parses the map to a json string. It must have been cleaned beforehand with cleanItems()
 * @return std::string
 */
std::string LevelCanvas::parseToJson() const {
    nlohmann::json jsonDoc;
    jsonDoc["MapWidth"] = width();
    jsonDoc["MapHeight"] = height();

    nlohmann::json array = nlohmann::json::array();
    for(auto& it: itemsOnMap_) {
        nlohmann::json newElement = {{"Type", it.second->type()}, {"Width", it.second->width()}, {"Height", it.second->height()}, {"X", it.second->x()}, {"Y", it.second->y()}};
        array.push_back(newElement);
    }
    jsonDoc["Map"] = array;

    std::string parsed = jsonDoc.dump();

    return parsed;
}

using namespace nlohmann;

void LevelCanvas::loadLevel(std::string level) {
    qDebug() << "loading level from: " << projectPath_;
    nlohmann::json json;
    json = nlohmann::json::parse(level);
    setFixedWidth(json["MapWidth"]);
    setFixedHeight(json["MapHeight"]);
    for(auto& item: json["Map"]) {
        QString strType = QString::fromStdString(item["Type"].get<std::string>());
        placeItemOnMap(item["X"].get<int>(), item["Y"].get<int>(), item["Width"].get<int>(), item["Height"].get<int>(), strType);
    }
}

void LevelCanvas::placeItemOnMap(int const x, int const y, int const width, int const height, QString const strType) {
    Tile *button = loadTile(this, x, y, width, height, strType);
    button->show();
    itemsOnMap_.emplace(button->id(), button);
}

Tile* LevelCanvas::loadTile(QWidget *parent, int const x, int const y, int const width, int const height, QString const strType) {
    Tile *button = new Tile(parent);
    QPixmap image(projectPath_+ "/images/" + strType + ".png");
    QIcon icon(image);
    button->setIcon(icon);
    button->setType(strType.toStdString());
    QSize size(width, height);
    button->setIconSize(size);
    button->setFlat(true);
    button->move(x, y);
    button->setStyleSheet("QPushButton {background:transparent; border:none;}");
    button->setFixedSize(size);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return button;
}

void LevelCanvas::cleanItems() {
    for(auto& it: itemsOnMap_) {
        std::cout << "cleaning" << std::endl;
        if(it.second->id() == -1) {
            qDebug() << "clean it";
            itemsOnMap_.erase(std::find(itemsOnMap_.begin(), itemsOnMap_.end(), it));
        }
    }
}

long Tile::objectCounter_;
Tile::Tile(QWidget *parent): QPushButton(parent){
    id_ = Tile::objectCounter_;
    Tile::objectCounter_++;

    connect(this, &Tile::clicked, this, &Tile::onClicked);
}
