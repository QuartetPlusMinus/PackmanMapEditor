// g++ -c Editor.cpp && g++ Editor.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system && ./sfml-app

#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include "Tile.cpp"
#include "sort.cpp"

using namespace sf;

const int mapWidth = 480;
const int mapHeight = 768;

RenderWindow window(VideoMode(mapWidth + 300, mapHeight), "MapEditor");

Menu menu(mapWidth + 10, 10);

RectangleShape menuShape;
RectangleShape tilesetShape;
RectangleShape line;
RectangleShape area;
RectangleShape selectionBox;
RectangleShape collidableBox;

Texture texture;
Sprite tileset;

bool isLargerX = false;
bool isLargerY = false;
bool isMoving = false;
bool isDrawing = false;
int tool = 1;

ScrollingField mapSc(0, 0, mapWidth, mapHeight, 0, 0); // 480, 768
ScrollingField menuSc(mapWidth + 20, mapHeight - 380 - 70, 260, 380, 0, 0);

Vector2i startingMousePos(0, 0);
Vector2i startingOffset(0, 0);
Vector2i spriteSelected(0, 0);
Vector2i selectionPos(2, 2);

std::vector<Tile> map;
Sprite tile;
//-----------------------------

void setUpGUI() {
    menuShape.setFillColor(Color(120, 120, 120));
    menuShape.setSize(Vector2f(300, mapHeight));
    menuShape.setPosition(Vector2f(mapWidth, 0));

    tilesetShape.setFillColor(Color(100, 100, 100));
    tilesetShape.setSize(Vector2f(menuSc.width, menuSc.height));
    tilesetShape.setPosition(Vector2f(menuSc.x, menuSc.y));

    selectionBox.setFillColor(Color(0, 0, 255, 75));
    selectionBox.setSize(Vector2f(32, 32));
    selectionBox.setPosition(Vector2f(menuSc.x, menuSc.y));

    collidableBox.setFillColor(Color(0, 0, 255, 75));
    collidableBox.setSize(Vector2f(32, 32));

    line.setFillColor(Color(255, 255, 255));
    area.setFillColor(Color(255, 255, 255, 40));

    texture.loadFromFile("tileset.png");
    tileset.setTexture(texture);
    tileset.setPosition(Vector2f(menuSc.x, menuSc.y));
    tile.setTexture(texture);

    if(texture.getSize().x > tilesetShape.getSize().x)  {
        isLargerX = true;
    }
    if(texture.getSize().y > tilesetShape.getSize().y) {
        isLargerY = true;
    }

    if(isLargerX && !isLargerY) {
        tileset.setTextureRect(IntRect(0, 0, tilesetShape.getSize().x, texture.getSize().y));
    }
    if(!isLargerX && isLargerY) {
        tileset.setTextureRect(IntRect(0, 0, texture.getSize().x, tilesetShape.getSize().y));
    }
    if(isLargerX && isLargerY) {
        tileset.setTextureRect(IntRect(0, 0, tilesetShape.getSize().x, tilesetShape.getSize().y));
    }
}

void saveMap() {
    sort(map, sortByY);
    sort(map, sortByXY);

    std::ofstream out;
    out.open("out.txt", std::ios_base::trunc);
    for(int i = 0; i < map.size(); i++) {
        out << map.at(i).x << " "
            << map.at(i).y << " "
            << map.at(i).sx << " "
            << map.at(i).sy << " "
            << map.at(i).isCollidable;

        if(i != map.size() - 1) {
            out << std::endl;
        }
    }
    out.close();
}

void update() {
    if(sf::IntRect(480, 0, 100, 50).contains(Mouse::getPosition(window))) {
        menu.selectedItemIndex = 0;
        menu.selected = true;
    }
    if(sf::IntRect(480 + 100, 0, 100, 50).contains(Mouse::getPosition(window))) {
        menu.selectedItemIndex = 1;
        menu.selected = true;
    }
    if(sf::IntRect(480 + 200, 0, 100, 50).contains(Mouse::getPosition(window))) {
        menu.selectedItemIndex = 2;
        menu.selected = true;
    }
    if(menu.selected) {
        menu.SelectedItem(menu.selectedItemIndex, menu.selected);

        if(Mouse::isButtonPressed(Mouse::Left)) {
            switch(menu.selectedItemIndex) {
                case 0:
                    saveMap();
                    break;
                case 1:
                    map.clear();
                    break;
                case 2:
                    window.close();
                    break;
            }
        }

        menu.selected = false;
    }

    if(Keyboard::isKeyPressed(Keyboard::Space)) {
        if(isMoving) {
            /*if(Mouse::getPosition(window).x < mapWidth) {
                mapSc.offsetX = startingOffset.x + Mouse::getPosition(window).x - startingMousePos.x;
                mapSc.offsetY = startingOffset.y + Mouse::getPosition(window).y - startingMousePos.y;
            } else*/
            if(Mouse::getPosition(window).x > menuSc.x && Mouse::getPosition(window).x < menuSc.x + menuSc.width
                && Mouse::getPosition(window).y > menuSc.y && Mouse::getPosition(window).y < menuSc.y + menuSc.height) {

                if(isLargerX) menuSc.offsetX = startingOffset.x + startingMousePos.x - Mouse::getPosition(window).x;
                if(isLargerY) menuSc.offsetY = startingOffset.y + startingMousePos.y - Mouse::getPosition(window).y;

                if(menuSc.offsetX < 0) menuSc.offsetX = 0;
                if(menuSc.offsetY < 0) menuSc.offsetY = 0;
                if(menuSc.offsetX > texture.getSize().x - menuSc.width) menuSc.offsetX = texture.getSize().x - menuSc.width;
                if(menuSc.offsetY > texture.getSize().y - menuSc.height) menuSc.offsetY = texture.getSize().y - menuSc.height;
            }
        } else {
            startingOffset.x = mapSc.offsetX;
            startingOffset.y = mapSc.offsetY;
        }
    } else {
        if(isDrawing && Mouse::getPosition(window).x < mapWidth) {
            if(tool == 1) {
                bool c = false;
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32 &&
                       map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32) {
                       c = true;
                   }
                }
                if(!c) {
                    map.push_back(
                        Tile((Mouse::getPosition(window).x - mapSc.offsetX) / 32,
                        (Mouse::getPosition(window).y - mapSc.offsetY) / 32,
                        spriteSelected.x, spriteSelected.y, false));
                    }
            } else if(tool == 2) {
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32 &&
                       map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32)
                       map.erase(map.begin() + i);
                }
            } else if(tool == 3) {
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32 &&
                       map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32)
                       map.at(i).isCollidable = true;
                }
            } else if(tool == 4) {
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32 &&
                       map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32)
                       map.at(i).isCollidable = false;
                }
            }
        }
    }

    if(isLargerX && isLargerY) tileset.setTextureRect(IntRect(menuSc.offsetX, menuSc.offsetY, menuSc.width, menuSc.height));
    else if(isLargerX && !isLargerY) tileset.setTextureRect(IntRect(menuSc.offsetX, 0, menuSc.width, texture.getSize().y));
    else if(!isLargerX && isLargerY) tileset.setTextureRect(IntRect(0, menuSc.offsetY, texture.getSize().x, menuSc.height));
    else if(!isLargerX && !isLargerY) tileset.setTextureRect(IntRect(0, 0, texture.getSize().x, texture.getSize().y));

    selectionBox.setPosition(menuSc.x + spriteSelected.x * 32 - menuSc.offsetX, menuSc.y + spriteSelected.y * 32 - menuSc.offsetY);

    if(selectionBox.getPosition().x < menuSc.x && selectionBox.getPosition().x > menuSc.x - 32) selectionPos.x = 1;
    if(selectionBox.getPosition().x > menuSc.x + menuSc.width - 32 && selectionBox.getPosition().x < menuSc.x + menuSc.width + 32) selectionPos.x = 3;
    if(selectionBox.getPosition().y < menuSc.y && selectionBox.getPosition().y > menuSc.y - 32) selectionPos.y = 1;
    if(selectionBox.getPosition().y > menuSc.y + menuSc.height - 32 && selectionBox.getPosition().y < menuSc.y + menuSc.height + 32) selectionPos.y = 3;
    if(selectionBox.getPosition().x > menuSc.x && selectionBox.getPosition().x < menuSc.x + menuSc.width - 32) selectionPos.x = 2;
    if(selectionBox.getPosition().y > menuSc.y && selectionBox.getPosition().y < menuSc.y + menuSc.height - 32) selectionPos.y = 2;
    if(selectionBox.getPosition().x <= menuSc.x - 32 || selectionBox.getPosition().x >= menuSc.x + menuSc.width) selectionPos.x = 0;
    if(selectionBox.getPosition().y <= menuSc.y - 32 || selectionBox.getPosition().y >= menuSc.y + menuSc.height) selectionPos.y = 0;

    if(selectionPos.x == 1) {
        selectionBox.setSize(Vector2f(32 - menuSc.x + selectionBox.getPosition().x, selectionBox.getSize().y));
        selectionBox.setPosition(Vector2f(menuSc.x, selectionBox.getPosition().y));
    } else if(selectionPos.x == 3) {
        selectionBox.setSize(Vector2f(menuSc.x + menuSc.width - selectionBox.getPosition().x, selectionBox.getSize().y));
        selectionBox.setPosition(Vector2f(menuSc.x + menuSc.width - selectionBox.getSize().x, selectionBox.getPosition().y));
    }
    if(selectionPos.y == 1) {
        selectionBox.setSize(Vector2f(selectionBox.getSize().x, 32 - menuSc.y + selectionBox.getPosition().y));
        selectionBox.setPosition(Vector2f(selectionBox.getPosition().x, menuSc.y));
    } else if(selectionPos.y == 3) {
        selectionBox.setSize(Vector2f(selectionBox.getSize().x, menuSc.y + menuSc.height - selectionBox.getPosition().y));
        selectionBox.setPosition(Vector2f(selectionBox.getPosition().x, menuSc.y + menuSc.height - selectionBox.getSize().y));
    }
    if(selectionPos.x == 2) selectionBox.setSize(Vector2f(32, selectionBox.getSize().y));
    if(selectionPos.y == 2) selectionBox.setSize(Vector2f(selectionBox.getSize().x, 32));
}


int main() {
    setUpGUI();

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if(event.type == Event::MouseButtonPressed) {
                if(Keyboard::isKeyPressed(Keyboard::Space)) {
                    isMoving = true;
                    startingMousePos = Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

                    if(Mouse::getPosition(window).x < mapWidth) { // 468
                        startingOffset = Vector2i(mapSc.offsetX, mapSc.offsetY);
                    } else if(Mouse::getPosition(window).x > menuSc.x && Mouse::getPosition(window).x < menuSc.x + menuSc.width &&
                              Mouse::getPosition(window).y > menuSc.y && Mouse::getPosition(window).y < menuSc.y + menuSc.height) {
                        startingOffset = Vector2i(menuSc.offsetX, menuSc.offsetY);
                    }
                } else if(Mouse::getPosition(window).x > menuSc.x && Mouse::getPosition(window).x < menuSc.x + menuSc.width &&
                          Mouse::getPosition(window).y > menuSc.y && Mouse::getPosition(window).y < menuSc.y + menuSc.height) {
                    spriteSelected.x = (Mouse::getPosition(window).x - menuSc.x + menuSc.offsetX) / 32;
                    spriteSelected.y = (Mouse::getPosition(window).y - menuSc.y + menuSc.offsetY) / 32;
                } else if(Mouse::getPosition(window).x < mapWidth) {
                    isDrawing = true;
                }
            } else if(event.type == Event::MouseButtonReleased) {
                isMoving = false;
                isDrawing = false;
            } else if(event.type == Event::KeyPressed) {
                if(event.key.code == Keyboard::Num1) tool = 1;
                else if(event.key.code == Keyboard::Num2) tool = 2;
                else if(event.key.code == Keyboard::Num3) tool = 3;
                else if(event.key.code == Keyboard::Num4) tool = 4;
                else if(event.key.code == Keyboard::S) saveMap();
            }
        }

        menu.SelectedItem(menu.selectedItemIndex, menu.selected);

        update();

        window.clear(Color(50, 50, 50));

        for(int i = 0; i < map.size(); i++) {
            tile.setTextureRect(IntRect(map.at(i).sx * 32, map.at(i).sy * 32, 32, 32));
            tile.setPosition(Vector2f(map.at(i).x * 32 + mapSc.offsetX, map.at(i).y * 32 + mapSc.offsetY));
            window.draw(tile);
            if(map.at(i).isCollidable) {
                if(tool == 3 || tool == 4) {
                    collidableBox.setPosition(Vector2f(map.at(i).x * 32 + mapSc.offsetX, map.at(i).y * 32 + mapSc.offsetY));
                    window.draw(collidableBox);
                }
            }
        }

        line.setSize(Vector2f(1, mapHeight));
        for(int i = 0; i < 15; i++) {
            line.setPosition(Vector2f(i * 32 + (mapSc.offsetX % 32), 0));
            window.draw(line);
        }
        line.setSize(Vector2f(mapWidth, 1));
        for(int i = 0; i < 24; i++) {
            line.setPosition(Vector2f(0, i * 32 + (mapSc.offsetY % 32)));
            window.draw(line);
        }

        // area.setSize(Vector2f(480, 768));
        // area.setPosition(Vector2f(mapSc.offsetX, mapSc.offsetY));
        // window.draw(area);

        window.draw(menuShape);
        window.draw(tilesetShape);
        window.draw(tileset);
        if(selectionPos.x != 0 && selectionPos.y != 0) {
            window.draw(selectionBox);
        }

        menu.draw(window);

        window.display();
    }

    return 0;
}
