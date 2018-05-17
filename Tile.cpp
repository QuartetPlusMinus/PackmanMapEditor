#include "Tile.h"

Tile::Tile(int _x, int _y, int _sx, int _sy, bool _isCollidable) :
    x(_x),
    y(_y),
    sx(_sx),
    sy(_sy),
    isCollidable(_isCollidable)
    {}

Tile::Tile() :
    x(0),
    y(0),
    sx(0),
    sy(0),
    isCollidable(false)
    {}

ScrollingField::ScrollingField(int _x, int _y, int _w, int _h, int _offsetX, int _offsetY) :
    x(_x),
    y(_y),
    width(_w),
    height(_h),
    offsetX(_offsetX),
    offsetY(_offsetY)
    {}

Menu::Menu(int x, int y) : selectedItemIndex(-1), selected(false) {
    if (!font.loadFromFile("arial.ttf")) {
        // std::cout << "Font Not Found\n";
    }

    navbar.setFillColor(sf::Color(0, 0, 255, 20));
    navbar.setSize(sf::Vector2f(300, 50));
    navbar.setPosition(sf::Vector2f(480, 0));

    menu[0].setFont(font);
    menu[0].setCharacterSize(20);
    menu[0].setColor(sf::Color::White);
    menu[0].setString("Save");
    menu[0].setPosition(sf::Vector2f(x + 20, y));

    menu[1].setFont(font);
    menu[1].setCharacterSize(20);
    menu[1].setColor(sf::Color::White);
    menu[1].setString("New map");
    menu[1].setPosition(sf::Vector2f(x + 100, y));

    menu[2].setFont(font);
    menu[2].setCharacterSize(20);
    menu[2].setColor(sf::Color::White);
    menu[2].setString("Exit");
    menu[2].setPosition(sf::Vector2f(x + 220, y));

    selectedItemIndex = 0;
}

void Menu::draw(sf::RenderWindow& window) {
    for (int i = 0; i < 3; i++) {
        window.draw(menu[i]);
    }
    window.draw(navbar);
}

void Menu::SelectedItem(int selectedItemIndex, bool selected) {
    if(selected) {
        menu[selectedItemIndex].setColor(sf::Color::Red);
    } else {
        menu[selectedItemIndex].setColor(sf::Color::White);
    }
}
