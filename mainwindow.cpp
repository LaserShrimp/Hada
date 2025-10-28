#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup the canvas
    // addLayer();

    QGridLayout *gridLayout = new QGridLayout();
    ui->pickItemArea->setLayout(gridLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_gridSpin_valueChanged(int arg1)
{
    for(auto& it: levelCanvas_) {
        it->setGridInterval(arg1);
    }
}


void MainWindow::on_mapHeight_sliderReleased()
{
    setMapHeight(ui->mapHeight->value());
}


void MainWindow::on_mapWidth_sliderMoved(int position)
{
    ui->mapWidthValue->setText(QString::number(position));
}


void MainWindow::on_mapWidth_sliderReleased()
{
    setMapWidth(ui->mapWidth->value());
}



void MainWindow::on_mapHeight_sliderMoved(int position)
{
    ui->mapHeightValue->setText(QString::number(position));
}

void MainWindow::on_saveButton_released()
{
    levelCanvas_[0]->cleanItems();
    levelCanvas_[0]->parseToJson();
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save as", ".lvl", "Level Files (*.lvl)");
    if(!filename.isEmpty()) {
        std::ofstream saveFile;
        QString extension = ".lvl";
        //Avoid ".lvl" redundance
        if(filename.contains(".lvl")) {
            extension = "";
        }
        saveFile.open(filename.toStdString() + extension.toStdString());
        saveFile << levelCanvas_[0]->parseToJson();
        saveFile.close();
    }
}

void MainWindow::on_loadButton_released()
{
    std::ifstream levelFile;
    QFileInfo fileInfos(QFileDialog::getOpenFileName(nullptr, "Open document", "", "Level Files (*.lvl)"));
    QString fileDir{fileInfos.absoluteDir().absolutePath()};
    QString filename{fileInfos.absoluteFilePath()};
    if(!filename.isEmpty()) {
        projectPath_ = fileDir;
        levelCanvas_[0]->setProjectPath(projectPath_);

        //erase the current tiles on canvas
        levelCanvas_[0]->resetMap();

        // loading the level
        levelFile.open(filename.toStdString());
        std::string jsonString;
        levelFile >> jsonString;
        levelCanvas_[0]->loadLevel(jsonString);
        ui->mapHeight->setValue(levelCanvas_[0]->height());
        ui->mapWidth->setValue(levelCanvas_[0]->width());
        levelFile.close();

        // loading the corresponding tiles in the comboBox
        std::ifstream tilesFile(fileDir.toStdString() + "/tiles.tiles");
        std::string strJson;
        tilesFile >> strJson;
        qDebug() << QString::fromStdString(strJson);
        nlohmann::json tilesJson = nlohmann::json::parse(strJson);
        for(auto& it: tilesJson) {
            addAvailableItem(it["Name"].get<std::string>(), "", it["Width"].get<int>(), it["Height"].get<int>());
            std::string itemName = it["Name"].get<std::string>();
            int w = it["Width"].get<int>();
            int h = it["Height"].get<int>();
            addItemToPickItemArea(QString::fromStdString(itemName), w, h);
        }
    }
}

void MainWindow::addAvailableItem(std::string const name, std::string const imagePath, int const width, int const height) {
    Item newItem{name, imagePath, width, height};
    availableItems_.emplace(newItem.name(), newItem);
}

void MainWindow::addItemToPickItemArea(QString const name, int const width, int const height) {
    QPushButton *newButton = new QPushButton();
    QPixmap image(projectPath_+ "/images/" + name + ".png");
    QIcon icon(image);
    newButton->setIcon(icon);
    QSize size(width, height);
    newButton->setIconSize(size);
    newButton->setStyleSheet("QPushButton:hover{background-color:blue;}");
    newButton->show();
    std::string itemName = name.toStdString();
    connect(newButton, &QPushButton::clicked, this, [&, itemName]{
        qDebug() << "selected " + QString::fromStdString(itemName);
        for(auto& it: levelCanvas_) {
            it->setSelectedItem(availableItems_[itemName]);
        }
    });
    ui->horizontalLayout_6->addWidget(newButton);
}


void MainWindow::on_checkBox_toggled(bool checked)
{
    for(auto& it: levelCanvas_) {
        it->setGrid(checked);
    }
}

void MainWindow::loadTiles(std::string tilesFileName) {
    std::ifstream file;
    file.open(tilesFileName);
    qDebug() << "Loading Tiles File: " + QString::fromStdString(tilesFileName);
    std::string fileContent;
    file >> fileContent;
    nlohmann::json json;
    json = nlohmann::json::parse(fileContent);
    for(auto& it: json) {
        addAvailableItem(it["Name"].get<std::string>(),
                projectPath_.toStdString(),
                it["Width"].get<int>(),
                it["Height"].get<int>());
        std::string newItemName = it["Name"].get<std::string>();
        addItemToPickItemArea(QString::fromStdString(newItemName), it["Width"].get<int>(), it["Height"].get<int>());
    }
}


void MainWindow::on_addItemButton_clicked()
{
    AddItemWindow addItemWindow(this);
    if(addItemWindow.exec() == QDialog::Accepted) {
        QString result(QString::fromStdString(addItemWindow.getNewItem()));
        qDebug() << "new item specs: " + result;

        // Add the new item to comboBox and availableItems_
        nlohmann::json parsedResult = nlohmann::json::parse(result.toStdString());
        std::string itemName = parsedResult["Name"].get<std::string>();
        int w = parsedResult["Width"].get<int>();
        int h = parsedResult["Height"].get<int>();
        addAvailableItem(itemName, "", w, h);
        addItemToPickItemArea(QString::fromStdString(itemName), w, h);

        // save the new item into items file
        if(projectPath_.isEmpty()) {
            projectPath_ = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly);
        }
        std::ifstream tilesFileRead(projectPath_.toStdString() + "/tiles.tiles");
        std::string fileContent;
        tilesFileRead >> fileContent;
        nlohmann::json tilesJson = nlohmann::json::array();
        if(fileContent != "") {
            for(auto& it: nlohmann::json::parse(fileContent)) {
                tilesJson.push_back(it);
            }
        }
        tilesJson.push_back(parsedResult);
        tilesFileRead.close();
        std::ofstream tilesFileWrite(projectPath_.toStdString() + "/tiles.tiles");
        tilesFileWrite << tilesJson.dump();
        tilesFileWrite.close();
    }
}

#include <string>

// TODO: currently working on this feature
void MainWindow::addLayer(){
    int nbLayers = levelCanvas_.size();
    CanvasLayer* newLayer = new CanvasLayer(ui->scrollArea);
    newLayer->setProjectPath(projectPath_);
    newLayer->setMinimumSize(mapWidth_, mapHeight_);
    newLayer->setFixedSize(mapWidth_, mapHeight_);
    newLayer->move(0, 0);
    newLayer->show();

    // add the layer in layer manager panel
    std::string newLayerName = "layer_" + std::to_string(nbLayers);
    newLayer->setName(newLayerName);
    QPushButton* newLayerButton = new QPushButton(QString::fromStdString(newLayerName));
    connect(newLayerButton, &QPushButton::clicked, [&, nbLayers]() {
                setFocusOnLayer(nbLayers);
                qDebug() << "focus set on layer: " + QString::number(nbLayers);
            });
    levelCanvas_.push_back(newLayer);
    layerButtons_.push_back(newLayerButton);
    ui->layers_area->addWidget(newLayerButton);
    setFocusOnLayer(nbLayers);
}

void MainWindow::deleteLayer(int layerToDelete) {
    ui->layers_area->removeWidget(layerButtons_[layerToDelete]);
    layerButtons_.erase(layerButtons_.begin()+layerToDelete);
    setFocusOnLayer(0);
}

void MainWindow::setFocusOnLayer(int layerToFocus) {
    focusedLayer_ = layerToFocus;
    for(int i = 0; i < levelCanvas_.size(); i++) {
        if (i == layerToFocus) {
            levelCanvas_[i]->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        } else {
            levelCanvas_[i]->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        }
    }
}

void MainWindow::on_addLayerButton_clicked()
{
    addLayer();
}


void MainWindow::on_deleteLayerButton_clicked()
{
    deleteLayer(focusedLayer_);
    setFocusOnLayer(0);
}

void MainWindow::setMapWidth(int newWidth) {
    mapWidth_ = newWidth;
    for(auto& it: levelCanvas_) {
        it->setFixedWidth(mapWidth_);
    }
}

void MainWindow::setMapHeight(int mapHeight) {
    mapHeight_ = mapHeight;
    for(auto& it: levelCanvas_) {
        it->setFixedHeight(mapHeight_);
    }
}
