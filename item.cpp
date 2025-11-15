#include "item.h"

Item::Item() {}

Item::Item(std::string name, std::string imagePath, int width, int height):name_(name), imagePath_(imagePath), width_(width), height_(height){}

void Item::setProperty(const std::string& propertyName, const std::string& value) {
    properties_.emplace(propertyName, value);
}
