#include "layerbutton.h"
#include "ui_layerbutton.h"

LayerButton::LayerButton(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LayerButton)
{
    ui->setupUi(this);
}

LayerButton::~LayerButton()
{
    delete ui;
}
