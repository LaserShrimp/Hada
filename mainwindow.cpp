#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open document", "", "Level Files (*.lvl)");
    if(!filename.isEmpty()) {
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

