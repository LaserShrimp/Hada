#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup the canvas
    levelCanvas_ = new LevelCanvas();
    levelCanvas_->setMinimumSize(1000, 1000);
    ui->scrollArea->setWidget(levelCanvas_);
    ui->scrollArea->setWidgetResizable(true);

    QGridLayout *gridLayout = new QGridLayout();
    ui->pickItemArea->setLayout(gridLayout);

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSelectedItem);
    setSelectedItem();
}

void MainWindow::setSelectedItem() {
    levelCanvas_->setSelectedItem(availableItems_[ui->comboBox->currentText().toStdString()]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_gridSpin_valueChanged(int arg1)
{
    levelCanvas_->setGridInterval(arg1);
}


void MainWindow::on_mapHeight_sliderReleased()
{
    levelCanvas_->setFixedHeight(ui->mapHeight->value());
}


void MainWindow::on_mapWidth_sliderMoved(int position)
{
    ui->mapWidthValue->setText(QString::number(position));
}


void MainWindow::on_mapWidth_sliderReleased()
{
    levelCanvas_->setFixedWidth(ui->mapWidth->value());
}



void MainWindow::on_mapHeight_sliderMoved(int position)
{
    ui->mapHeightValue->setText(QString::number(position));
}


void MainWindow::on_mapHeight_valueChanged(int value)
{

}


void MainWindow::on_mapWidth_valueChanged(int value)
{

}


void MainWindow::on_saveButton_released()
{
    levelCanvas_->cleanItems();
    levelCanvas_->parseToJson();
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save as", ".lvl", "Level Files (*.lvl)");
    if(!filename.isEmpty()) {
        std::ofstream saveFile;
        QString extension = ".lvl";
        //Avoid ".lvl" redundance
        if(filename.contains(".lvl")) {
            extension = "";
        }
        saveFile.open(filename.toStdString() + extension.toStdString());
        saveFile << levelCanvas_->parseToJson();
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
        levelCanvas_->setProjectPath(projectPath_);

        // loading the level
        levelFile.open(filename.toStdString());
        std::string jsonString;
        levelFile >> jsonString;
        levelCanvas_->loadLevel(jsonString);
        ui->mapHeight->setValue(levelCanvas_->height());
        ui->mapWidth->setValue(levelCanvas_->width());
        levelFile.close();

        // loading the corresponding tiles in the comboBox
        std::ifstream tilesFile(fileDir.toStdString() + "/tiles.tiles");
        std::string strJson;
        tilesFile >> strJson;
        qDebug() << QString::fromStdString(strJson);
        nlohmann::json tilesJson = nlohmann::json::parse(strJson);
        for(auto& it: tilesJson) {
            Item newItem{it["Name"].get<std::string>(), "", it["Width"].get<int>(), it["Height"].get<int>()};
            availableItems_.emplace(newItem.name(), newItem);
            ui->comboBox->addItem(QString::fromStdString(newItem.name()));
            // Tile* newButton = levelCanvas_->loadTile(ui->pickItemArea, currentX, 0, it["Width"].get<int>(), it["Height"].get<int>(), QString::fromStdString(it["Name"].get<std::string>()));
            QPushButton *newButton = new QPushButton();
            QPixmap image(projectPath_+ "/images/" + QString::fromStdString(it["Name"].get<std::string>()) + ".png");
            QIcon icon(image);
            newButton->setIcon(icon);
            QSize size(it["Width"].get<int>(), it["Height"].get<int>());
            newButton->setIconSize(size);
            newButton->setStyleSheet("QPushButton:hover{background-color:blue;}");
            // newButton->setFixedSize(size);
            newButton->show();
            std::string itemName = it["Name"].get<std::string>();
            connect(newButton, &QPushButton::clicked, this, [&, itemName]{
                qDebug() << "selected " + QString::fromStdString(itemName);
                levelCanvas_->setSelectedItem(availableItems_[itemName]);
            });
            ui->horizontalLayout_6->addWidget(newButton);
        }
    }
}


void MainWindow::on_checkBox_toggled(bool checked)
{
    levelCanvas_->setGrid(checked);
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
        Item newItem{it["Name"].get<std::string>(),
                projectPath_.toStdString(),
                it["Width"].get<int>(),
                it["Height"].get<int>()};
        availableItems_.emplace(newItem.name(), newItem);
        ui->comboBox->addItem(QString::fromStdString(newItem.name()));
    }
}


void MainWindow::on_addItemButton_clicked()
{
    AddItemWindow addItemWindow(this);
    if(addItemWindow.exec() == QDialog::Accepted) {
        QString result(QString::fromStdString(addItemWindow.getNewItem()));

        // Add the new item to comboBox and availableItems_
        nlohmann::json jsonResult = nlohmann::json::parse(result.toStdString());
        qDebug() << "new item specs: " + result;
        ui->comboBox->addItem(QString::fromStdString(jsonResult["Name"].get<std::string>()));
        nlohmann::json parsedResult = nlohmann::json::parse(result.toStdString());
        Item newItem(parsedResult["Name"].get<std::string>(), "", parsedResult["Width"].get<int>(), parsedResult["Height"].get<int>());
        availableItems_.emplace(newItem.name(), newItem);

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

        //Make it the new current selected item
        setSelectedItem();
    }
}

