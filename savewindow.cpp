#include "savewindow.h"
#include "ui_savewindow.h"

SaveWindow::SaveWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SaveWindow)
{
    ui->setupUi(this);
}

SaveWindow::~SaveWindow()
{
    delete ui;
}

void SaveWindow::on_ConfirmBox_rejected()
{
    close();
}


void SaveWindow::on_ConfirmBox_accepted()
{
    //Save the map into the provided file path
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save as", "", "Level Files (*.lvl)");
    if(!filename.isEmpty()) {
        std::ofstream saveFile;
        std::cout << "filename: " << filename.toStdString() << std::endl;
        saveFile.open(filename.toStdString() + ".lvl");
        saveFile << jsonData_;
        saveFile.close();
        close();
    }
}

void SaveWindow::setJson(std::string json) {
    jsonData_ = json;
}

