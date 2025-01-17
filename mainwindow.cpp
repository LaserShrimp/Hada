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

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSelectedItem);
    setSelectedItem();
}

void MainWindow::setSelectedItem() {
    levelCanvas_->setSelectedItem(ui->comboBox->currentText());
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

        levelFile.open(filename.toStdString());
        std::string jsonString;
        levelFile >> jsonString;
        levelCanvas_->loadLevel(jsonString);
        ui->mapHeight->setValue(levelCanvas_->height());
        ui->mapWidth->setValue(levelCanvas_->width());
        levelFile.close();
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
    AddItemWindow* addItemWindow = new AddItemWindow(this);
    addItemWindow->show();
    if(addItemWindow->exec() == QDialog::Accepted) {
        QString result(QString::fromStdString(addItemWindow->getNewItem()));

        // Add the new item to comboBox
        nlohmann::json jsonResult = nlohmann::json::parse(result.toStdString());
        qDebug() << "new item specs: " + result;
        ui->comboBox->addItem(QString::fromStdString(jsonResult["Name"].get<std::string>()));

        // save the new item into items file
        if(projectPath_.isEmpty()) {
            projectPath_ = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly);
        }
        std::ofstream tilesFile(projectPath_.toStdString() + "/tiles.tiles", std::ios::app);
        tilesFile << result.toStdString();
        tilesFile.close();
    }
}

