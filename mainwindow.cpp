#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    updateCanvasSize();
}


void MainWindow::on_mapWidth_sliderMoved(int position)
{
    ui->mapWidthValue->setText(QString::number(position));
}


void MainWindow::on_mapWidth_sliderReleased()
{
    updateCanvasSize();
}



void MainWindow::on_mapHeight_sliderMoved(int position)
{
    ui->mapHeightValue->setText(QString::number(position));
}

void MainWindow::on_saveButton_released()
{
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save as", ".lvl", "Level Files (*.lvl)");
    if(filename.isEmpty()) {
        qDebug("[on_saveButton_released()]Filename is empty...");
        return;
    }
    std::ofstream saveFile;
    QString extension = ".lvl";
    //Avoid ".lvl" redundance
    if(filename.contains(".lvl")) {
        extension = "";
    }
    saveFile.open(filename.toStdString() + extension.toStdString());
    nlohmann::json builtJson;
    nlohmann::json layersArray = nlohmann::json::array();
    for(auto& it: levelCanvas_) {
        it->cleanItems();
        layersArray.push_back(it->buildJson());
    }
    builtJson.emplace("Layers", layersArray);
    saveFile << builtJson.dump();
}

void MainWindow::on_loadButton_released()
{
    QFileInfo fileInfos(QFileDialog::getOpenFileName(nullptr, "Open document", "", "Level Files (*.lvl)"));
    QString fileDir{fileInfos.absoluteDir().absolutePath()};
    QString filename{fileInfos.absoluteFilePath()};
    if(filename.isEmpty()) {
        qDebug() << "level file path is empty";
        return;
    }

    std::ifstream levelFile;
    levelFile.open(filename.toStdString());
    std::string jsonString;
    levelFile >> jsonString;

    nlohmann::json jsonLevel = nlohmann::json::parse(jsonString);

    projectPath_ = fileDir;
    if(jsonLevel.find("Layers") == jsonLevel.end()) {
        return;
    }

    resetProject();
    for(auto& it: jsonLevel["Layers"]) {
        CanvasLayer* currentLayer = new CanvasLayer(ui->canvas);
        currentLayer->setProjectPath(projectPath_); // If we don't set the layer's project path here, the tiles' images won't load
        currentLayer->loadLayer(it.dump());
        ui->mapHeight->setValue(currentLayer->height());
        ui->mapWidth->setValue(currentLayer->width());
        addLayer(currentLayer);
    }
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
    ui->pickItemAreaLayout->addWidget(newButton);
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

void MainWindow::resetProject() {
    while(ui->pickItemAreaLayout->count() > 0) {
        auto item = ui->pickItemAreaLayout->takeAt(0);
        delete item;
    }
    resetMap();
}

void MainWindow::resetMap() {
    for(auto& it: levelCanvas_) {
        delete it;
    }
    levelCanvas_.clear();
    for(auto& it: layerButtons_) {
        ui->layers_area->removeWidget(it);
        delete it;
    }
    layerButtons_.clear();

    // for(auto& it: availableItems_) {
    //     delete it.second;
    // }
    availableItems_.clear();
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

void MainWindow::addLayer(){
    int nbLayers = levelCanvas_.size();
    CanvasLayer* newLayer = new CanvasLayer(ui->canvas);
    newLayer->setProjectPath(projectPath_);
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
    updateCanvasSize();
}

void MainWindow::addLayer(CanvasLayer* layerToAdd) {
    int nbLayers = levelCanvas_.size();
    layerToAdd->setProjectPath(projectPath_);
    layerToAdd->move(0,0);
    layerToAdd->show();

    std::string newLayerName = "layer_" + std::to_string(nbLayers);
    layerToAdd->setName(newLayerName);
    QPushButton* newLayerButton = new QPushButton(QString::fromStdString(newLayerName));
    connect(newLayerButton, &QPushButton::clicked, [&, nbLayers]() {
        setFocusOnLayer(nbLayers);
        qDebug() << "focus set on layer: " + QString::number(nbLayers);
    });
    levelCanvas_.push_back(layerToAdd);
    layerButtons_.push_back(newLayerButton);
    ui->layers_area->addWidget(newLayerButton);
    setFocusOnLayer(nbLayers);
    updateCanvasSize();
}

void MainWindow::deleteLayer(int layerToDelete) {
    ui->layers_area->removeWidget(layerButtons_[layerToDelete]);
    delete layerButtons_[layerToDelete];
    layerButtons_.erase(layerButtons_.begin()+layerToDelete);
    delete levelCanvas_[layerToDelete];
    levelCanvas_.erase(levelCanvas_.begin()+layerToDelete);
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

void MainWindow::updateCanvasSize() {
    int currentW = ui->mapWidth->value();
    int currentH = ui->mapHeight->value();
    setMapWidth(currentW);
    setMapHeight(currentH);
    ui->canvas->setFixedSize(currentW, currentH);
}
