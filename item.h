#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <map>

class Item
{
private:
    std::string name_{""};
    std::string imagePath_{""};
    int width_{0};
    int height_{0};
    std::map<std::string, std::string> properties_;
public:
    Item();
    Item(std::string name, std::string imagePath, int width, int height);
    ~Item() = default;

    void setName(std::string name) {name_ = name;}
    void setImagePath(std::string path) {imagePath_ = path;}
    void setHeight(int height) {height_ = height;}
    void setWidth(int width) {width_ = width;}
    void setProperty(const std::string& propertyName, const std::string& value);

    std::string name() const {return name_;}
    int width() const{return width_;}
    int height() const{return height_;}
};

#endif // ITEM_H
