#include "Window.h"

WinShape::WinShape(sf::Vector2f size, sf::Color color) {
    Shape.setSize(size);
    Shape.setFillColor(color);
}

WinShape::WinShape(sf::Vector2f size, sf::Vector2f position, sf::Color color) {
    Shape.setSize(size);
    Shape.setPosition(position);
    Shape.setFillColor(color);
}

void WinShape::draw(sf::RenderWindow& window) {
    window.draw(Shape);
}

sf::Vector2f WinShape::getSize() {
    return Shape.getSize();
}

sf::Vector2f WinShape::getPosition() {
    return Shape.getPosition();
}

void WinShape::setSize(sf::Vector2f size) {
    Shape.setSize(size);
}

void WinShape::setPosition(sf::Vector2f position) {
    Shape.setPosition(position);
}

Tileset::Tileset(const char* picture, sf::Vector2f position) :
    isLargerX(false),
    isLargerY(false)
    {
        if(!texture.loadFromFile(picture)) {

        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }

sf::Vector2u Tileset::getTextureSize() {
    return texture.getSize();
}

void Tileset::setTextureRect(sf::IntRect rect) {
    sprite.setTextureRect(rect);
}

void Tileset::setPosition(sf::Vector2f position) {
    sprite.setPosition(position);
}

void Tileset::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Tileset::TextureCutting(WinShape tilesetShape) {
    if(texture.getSize().x > tilesetShape.getSize().x)  {
        isLargerX = true;
    }
    if(texture.getSize().y > tilesetShape.getSize().y) {
        isLargerY = true;
    }

    if(isLargerX && !isLargerY) {
        sprite.setTextureRect(sf::IntRect(0, 0, tilesetShape.getSize().x, texture.getSize().y));
    } else if(!isLargerX && isLargerY) {
        sprite.setTextureRect(sf::IntRect(0, 0, texture.getSize().x, tilesetShape.getSize().y));
    } else if(isLargerX && isLargerY) {
        sprite.setTextureRect(sf::IntRect(0, 0, tilesetShape.getSize().x, tilesetShape.getSize().y));
    }
}

Grid::Grid(int w, int h, int th) :
    width(w),
    height(h),
    thick(th)
{
    line.setFillColor(sf::Color(255, 255, 255));
}

void Grid::draw(sf::RenderWindow& window) {
    line.setSize(sf::Vector2f(width, thick));
    for(int i = 0; i < height / 32; i++) {
        line.setPosition(sf::Vector2f(0, i * 32));
        window.draw(line);
    }
    line.setSize(sf::Vector2f(thick, height));
    for(int i = 0; i < width / 32; i++) {
        line.setPosition(sf::Vector2f(i * 32, 0));
        window.draw(line);
    }
}
