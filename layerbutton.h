#ifndef LAYERBUTTON_H
#define LAYERBUTTON_H

#include <QWidget>

namespace Ui {
class LayerButton;
}

class LayerButton : public QWidget
{
    Q_OBJECT

public:
    explicit LayerButton(QWidget *parent = nullptr);
    ~LayerButton();

private:
    Ui::LayerButton *ui;
};

#endif // LAYERBUTTON_H
