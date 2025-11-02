#include "canvaslayer.h"
#include "ui_canvaslayer.h"

CanvasLayer::CanvasLayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CanvasLayer)
{
    ui->setupUi(this);
}

CanvasLayer::~CanvasLayer()
{
    delete ui;
}

void CanvasLayer::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    if (grid_) {
        clickPos.setX((clickPos.x() / gridInterval_) * gridInterval_);
        clickPos.setY((clickPos.y() / gridInterval_) * gridInterval_);
    }
    QString strType = QString::fromStdString(selectedItem_.name());
    if (!strType.isEmpty()) {
        placeItemOnMap(clickPos.x(),
                       clickPos.y(),
                       selectedItem_.width(),
                       selectedItem_.height(),
                       strType);
        qDebug() << "item added: " << QString::fromStdString(selectedItem_.name()) << " "
                 << selectedItem_.width() << " " << selectedItem_.height() << " to layer: " << QString::fromStdString(name_);
    } else {
        qDebug() << "No item selected";
    }
}

void CanvasLayer::paintEvent(QPaintEvent *event)
{
    if (!grid_) {
        return;
    }
    QWidget::paintEvent(event);
    QPainter painter(this);

    QPen pen(Qt::lightGray);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    int width = this->width();
    int height = this->height();

    for (int i = 0; i < width; i += gridInterval_) {
        painter.drawLine(i, 0, i, height);
    }

    for (int i = 0; i < height; i += gridInterval_) {
        painter.drawLine(0, i, width, i);
    }
}

/**
 * @brief LevelCanvas::parseToJson parses the map to a json string. It must have been cleaned beforehand with cleanItems()
 * @return std::string
 */
std::string CanvasLayer::parseToJsonString() const
{
    return buildJson().dump();
}

nlohmann::json CanvasLayer::buildJson() const {
    nlohmann::json result;
    result["MapWidth"] = width();
    result["MapHeight"] = height();

    nlohmann::json array = nlohmann::json::array();
    for (auto &it : itemsOnMap_) {
        nlohmann::json newElement = {{"Type", it.second->type()},
                                     {"Width", it.second->width()},
                                     {"Height", it.second->height()},
                                     {"X", it.second->x()},
                                     {"Y", it.second->y()}};
        array.push_back(newElement);
    }
    result["Map"] = array;
    return result;
}

using namespace nlohmann;

void CanvasLayer::loadLayer(std::string layer)
{
    qDebug() << "loading layer from: " << projectPath_;
    nlohmann::json json;
    json = nlohmann::json::parse(layer);
    setFixedSize(json["MapWidth"], json["MapHeight"]);
    for (auto &item : json["Map"]) {
        QString strType = QString::fromStdString(item["Type"].get<std::string>());
        placeItemOnMap(item["X"].get<int>(),
                       item["Y"].get<int>(),
                       item["Width"].get<int>(),
                       item["Height"].get<int>(),
                       strType);
    }
}

void CanvasLayer::placeItemOnMap(
    int const x, int const y, int const width, int const height, QString const strType)
{
    Tile *button = loadTile(this, x, y, width, height, strType);
    button->show();
    itemsOnMap_.emplace(button->id(), button);
}

Tile *CanvasLayer::loadTile(QWidget *parent,
                            int const x,
                            int const y,
                            int const width,
                            int const height,
                            QString const strType)
{
    Tile *button = new Tile(parent);
    QPixmap image(projectPath_ + "/images/" + strType + ".png");
    qDebug() << "project path : " << projectPath_ + "/images/" + strType + ".png";
    QIcon icon(image);
    button->setIcon(icon);
    button->setType(strType.toStdString());
    QSize size(width, height);
    button->setIconSize(size);
    button->setFlat(true);
    button->move(x, y);
    button->setStyleSheet("QPushButton {background:transparent; border:none;opacity:0.5;}");
    button->setFixedSize(size.width(), size.height());
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return button;
}

void CanvasLayer::cleanItems()
{
    for (auto &it : itemsOnMap_) {
        std::cout << "cleaning" << std::endl;
        if (it.second->id() == -1) {
            qDebug() << "clean it";
            itemsOnMap_.erase(std::find(itemsOnMap_.begin(), itemsOnMap_.end(), it));
        }
    }
}

void CanvasLayer::resetMap()
{
    for (auto &it : itemsOnMap_) {
        delete it.second;
    }
    itemsOnMap_.clear();
    qDebug() << "items cleared";
}

long Tile::objectCounter_;
Tile::Tile(QWidget *parent)
    : QPushButton(parent)
{
    foreground_ = new QWidget(this);
    foreground_->move(0, 0);
    foreground_->setFixedSize(width(), height());
    foreground_->setStyleSheet("QWidget:hover{background-color:rgba(155, 0, 155, 0.3);}");
    id_ = Tile::objectCounter_;
    Tile::objectCounter_++;

    connect(this, &Tile::clicked, this, &Tile::onClicked);
}

void Tile::setFixedSize(int w, int h) {
    this->QPushButton::setFixedSize(w, h);
    foreground_->setFixedSize(w, h);
}
