#ifndef PROPERTYROW_H
#define PROPERTYROW_H

#include <QWidget>

namespace Ui {
class PropertyRow;
}

class PropertyRow : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyRow(QWidget *parent = nullptr);
    PropertyRow(QWidget *parent, const QString& propertyName, const QString& propertyValue);
    ~PropertyRow();

private:
    Ui::PropertyRow *ui;
};

#endif // PROPERTYROW_H
