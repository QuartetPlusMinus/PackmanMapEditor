#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include "Window.h"
#include "Button.h"
#include "Tile.h"

using namespace sf;

const int mapWidth = 480;
const int mapHeight = 768;
const int sideMenuWidth = 300;

RenderWindow window(VideoMode(mapWidth + sideMenuWidth, mapHeight), "MapEditor");

Vector2i startingMousePos(0, 0);
Vector2i startingOffset(0, 0);

std::vector<Tile> map;

bool isMoving = false;
bool isDrawing = false;

enum toolType {
    drawing = 1,
    removing,
    wall,
    pass
} tool;

void saveMap();
void update();

ScrollingField mapSc(0, 0, mapWidth, mapHeight, 0, 0);
ScrollingField tilesetSc(mapWidth + 20, mapHeight - 420, 260, 380, 0, 0);

WinShape sideMenu(Vector2f(sideMenuWidth, mapHeight), Vector2f(mapWidth, 0), Color(120, 120, 120));
WinShape tilesetShape(Vector2f(tilesetSc.width, tilesetSc.height), Vector2f(tilesetSc.x, tilesetSc.y), Color(100, 100, 100));
WinShape collidableBox(Vector2f(32, 32), Color(0, 0, 255, 75));
SelectionBox selectionBox(Vector2f(32, 32), Vector2f(tilesetSc.x, tilesetSc.y), Color(0, 0, 255, 75));
Tileset tile("src/tileset.png", Vector2f(0, 0));
Tileset tileset("src/tileset.png", Vector2f(tilesetSc.x, tilesetSc.y));

ToolMenu toolbar(mapWidth + 20, 80);
NavMenu navbar(mapWidth, 0);

Grid grid(mapWidth, mapHeight, 1);

int main()
{
    tileset.TextureCutting(tilesetShape);

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

                    if(Mouse::getPosition(window).x < mapWidth) {
                        startingOffset = Vector2i(mapSc.offsetX, mapSc.offsetY);
                    } else if(Mouse::getPosition(window).x > tilesetSc.x && Mouse::getPosition(window).x < tilesetSc.x + tilesetSc.width &&
                              Mouse::getPosition(window).y > tilesetSc.y && Mouse::getPosition(window).y < tilesetSc.y + tilesetSc.height) {
                        startingOffset = Vector2i(tilesetSc.offsetX, tilesetSc.offsetY);
                    }
                } else if(Mouse::getPosition(window).x > tilesetSc.x && Mouse::getPosition(window).x < tilesetSc.x + tilesetSc.width &&
                          Mouse::getPosition(window).y > tilesetSc.y && Mouse::getPosition(window).y < tilesetSc.y + tilesetSc.height) {
                    selectionBox.spriteSelected.x = (Mouse::getPosition(window).x - tilesetSc.x + tilesetSc.offsetX) / 32;
                    selectionBox.spriteSelected.y = (Mouse::getPosition(window).y - tilesetSc.y + tilesetSc.offsetY) / 32;
                } else if(Mouse::getPosition(window).x < mapWidth) {
                    isDrawing = true;
                }
            } else if(event.type == Event::MouseButtonReleased) {
                isMoving = false;
                isDrawing = false;
            }
        }

        navbar.SelectedItem(navbar.selectedItemIndex, navbar.selected);

        update();

        window.clear(Color(50, 50, 50));

        for(int i = 0; i < map.size(); i++) {
            tile.setTextureRect(IntRect(map.at(i).sx * 32, map.at(i).sy * 32, 32, 32));
            tile.setPosition(Vector2f(map.at(i).x * 32 + mapSc.offsetX, map.at(i).y * 32 + mapSc.offsetY));
            tile.draw(window);
            if(map.at(i).isCollidable) {
                if(tool == wall || tool == pass) {
                    collidableBox.setPosition(Vector2f(map.at(i).x * 32 + mapSc.offsetX, map.at(i).y * 32 + mapSc.offsetY));
                    collidableBox.draw(window);
                }
            }
        }

        grid.draw(window);
        sideMenu.draw(window);
        tilesetShape.draw(window);
        tileset.draw(window);
        if(selectionBox.selectionPos.x != 0 && selectionBox.selectionPos.y != 0) {
            selectionBox.draw(window);
        }

        navbar.draw(window);
        toolbar.draw(window);

        window.display();
    }

    return 0;
}

//--------------------------------------------------
template <class T>
void sort(std::vector<Tile>& map, const T& comp) {
    if(map.size() < 1) {
        return;
    }
    for(int i = 0; i < map.size() - 1; ++i) {
        for(int j = 0; j < map.size() - i - 1; ++j) {
            if(comp(map[j], map[j + 1])) {
                std::swap(map[j], map[j + 1]);
            }
        }
    }
}

void saveMap() {
    sort(map,
        [](const Tile& first, const Tile& second) {
            return (first.y > second.y);
        }
    );
    sort(map,
        [](const Tile& first, const Tile& second) {
            return (first.x > second.x && first.y == second.y);
        }
    );

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

void update()
{
    navbar.whichItemSelected(window);
    if(navbar.selected) {
        navbar.SelectedItem(navbar.selectedItemIndex, navbar.selected);
        if(Mouse::isButtonPressed(Mouse::Left)) {
            switch(navbar.selectedItemIndex) {
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
        navbar.selected = false;
    }

    toolbar.whichItemSelected(window);
    if(toolbar.selected) {
        if(Mouse::isButtonPressed(Mouse::Left)) {
            toolbar.SelectedItem(toolbar.selectedItemIndex, toolbar.selected);
            switch(toolbar.selectedItemIndex) {
                case 0:
                    tool = drawing;
                    break;
                case 1:
                    tool = wall;
                    break;
                case 2:
                    tool = pass;
                    break;
                case 3:
                    tool = removing;
                    break;
            }
        }
        toolbar.selected = false;
    }

    if(Keyboard::isKeyPressed(Keyboard::Space)) {
        if(isMoving) {
            if(Mouse::getPosition(window).x > tilesetSc.x && Mouse::getPosition(window).x < tilesetSc.x + tilesetSc.width &&
               Mouse::getPosition(window).y > tilesetSc.y && Mouse::getPosition(window).y < tilesetSc.y + tilesetSc.height) {

                if(tileset.isLargerX) {
                    tilesetSc.offsetX = startingOffset.x + startingMousePos.x - Mouse::getPosition(window).x;
                }
                if(tileset.isLargerY) {
                    tilesetSc.offsetY = startingOffset.y + startingMousePos.y - Mouse::getPosition(window).y;
                }

                if(tilesetSc.offsetX < 0) {
                    tilesetSc.offsetX = 0;
                }
                if(tilesetSc.offsetY < 0) {
                    tilesetSc.offsetY = 0;
                }
                if(tilesetSc.offsetX > tileset.getTextureSize().x - tilesetSc.width) {
                    tilesetSc.offsetX = tileset.getTextureSize().x - tilesetSc.width;
                }
                if(tilesetSc.offsetY > tileset.getTextureSize().y - tilesetSc.height) {
                    tilesetSc.offsetY = tileset.getTextureSize().y - tilesetSc.height;
                }
            }
        } else {
            startingOffset.x = mapSc.offsetX;
            startingOffset.y = mapSc.offsetY;
        }
    } else {
        if(isDrawing
        && Mouse::getPosition(window).x < mapWidth && Mouse::getPosition(window).x >= 0
        && Mouse::getPosition(window).y < mapHeight && Mouse::getPosition(window).y >= 0) {
            if(tool == drawing) {
                bool isEmpty = false;
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32
                    && map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32) {
                       // isEmpty = true;
                       map.erase(map.begin() + i);
                   }
                }
                if(!isEmpty) {
                    map.push_back(Tile((Mouse::getPosition(window).x - mapSc.offsetX) / 32,
                        (Mouse::getPosition(window).y - mapSc.offsetY) / 32,
                        selectionBox.spriteSelected.x, selectionBox.spriteSelected.y, false));
                }
            } else if(tool == removing) {
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32
                    && map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32) {
                        map.erase(map.begin() + i);
                    }
                }
            } else if(tool == wall) {
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32
                    && map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32) {
                        map.at(i).isCollidable = true;
                    }
                }
            } else if(tool == pass) {
                for(int i = 0; i < map.size(); i++) {
                    if(map.at(i).x == (Mouse::getPosition(window).x - mapSc.offsetX) / 32
                    && map.at(i).y == (Mouse::getPosition(window).y - mapSc.offsetY) / 32) {
                        map.at(i).isCollidable = false;
                    }
                }
            }
        }
    }

    // корректное отображение картинки при скролле
    if(tileset.isLargerX && tileset.isLargerY) {
        tileset.setTextureRect(IntRect(tilesetSc.offsetX, tilesetSc.offsetY, tilesetSc.width, tilesetSc.height));
    } else if(tileset.isLargerX && !tileset.isLargerY) {
        tileset.setTextureRect(IntRect(tilesetSc.offsetX, 0, tilesetSc.width, tileset.getTextureSize().y));
    } else if(!tileset.isLargerX && tileset.isLargerY) {
        tileset.setTextureRect(IntRect(0, tilesetSc.offsetY, tileset.getTextureSize().x, tilesetSc.height));
    } else if(!tileset.isLargerX && !tileset.isLargerY) {
        tileset.setTextureRect(IntRect(0, 0, tileset.getTextureSize().x, tileset.getTextureSize().y));
    }

    selectionBox.setPosition(Vector2f(tilesetSc.x + selectionBox.spriteSelected.x * 32 - tilesetSc.offsetX, tilesetSc.y + selectionBox.spriteSelected.y * 32 - tilesetSc.offsetY));

    // корректное отображение выбраной плитки (selectionBox) в наборе плиток (tilesetSc)
    if(selectionBox.getPosition().x < tilesetSc.x && selectionBox.getPosition().x > tilesetSc.x - 32) {
        selectionBox.selectionPos.x = 1;
    }
    if(selectionBox.getPosition().y < tilesetSc.y && selectionBox.getPosition().y > tilesetSc.y - 32) {
        selectionBox.selectionPos.y = 1;
    }
    if(selectionBox.getPosition().x > tilesetSc.x + tilesetSc.width - 32 && selectionBox.getPosition().x < tilesetSc.x + tilesetSc.width + 32) {
        selectionBox.selectionPos.x = 3;
    }
    if(selectionBox.getPosition().y > tilesetSc.y + tilesetSc.height - 32 && selectionBox.getPosition().y < tilesetSc.y + tilesetSc.height + 32) {
        selectionBox.selectionPos.y = 3;
    }
    if(selectionBox.getPosition().x > tilesetSc.x && selectionBox.getPosition().x < tilesetSc.x + tilesetSc.width - 32) {
        selectionBox.selectionPos.x = 2;
    }
    if(selectionBox.getPosition().y > tilesetSc.y && selectionBox.getPosition().y < tilesetSc.y + tilesetSc.height - 32) {
        selectionBox.selectionPos.y = 2;
    }
    if(selectionBox.getPosition().x <= tilesetSc.x - 32 || selectionBox.getPosition().x >= tilesetSc.x + tilesetSc.width) {
        selectionBox.selectionPos.x = 0;
    }
    if(selectionBox.getPosition().y <= tilesetSc.y - 32 || selectionBox.getPosition().y >= tilesetSc.y + tilesetSc.height) {
        selectionBox.selectionPos.y = 0;
    }

    if(selectionBox.selectionPos.x == 1) { // в случае если selectionBox выходит за левую границу tilesetSc
        selectionBox.setSize(Vector2f(32 - tilesetSc.x + selectionBox.getPosition().x, selectionBox.getSize().y));
        selectionBox.setPosition(Vector2f(tilesetSc.x, selectionBox.getPosition().y));
    } else if(selectionBox.selectionPos.x == 3) { // в случае если selectionBox выходит за правую границу tilesetSc
        selectionBox.setSize(Vector2f(tilesetSc.x + tilesetSc.width - selectionBox.getPosition().x, selectionBox.getSize().y));
        selectionBox.setPosition(Vector2f(tilesetSc.x + tilesetSc.width - selectionBox.getSize().x, selectionBox.getPosition().y));
    }
    if(selectionBox.selectionPos.y == 1) { // в случае если selectionBox выходит за верхнюю границу tilesetSc
        selectionBox.setSize(Vector2f(selectionBox.getSize().x, 32 - tilesetSc.y + selectionBox.getPosition().y));
        selectionBox.setPosition(Vector2f(selectionBox.getPosition().x, tilesetSc.y));
    } else if(selectionBox.selectionPos.y == 3) { // в случае если selectionBox выходит за нижнюю границу tilesetSc
        selectionBox.setSize(Vector2f(selectionBox.getSize().x, tilesetSc.y + tilesetSc.height - selectionBox.getPosition().y));
        selectionBox.setPosition(Vector2f(selectionBox.getPosition().x, tilesetSc.y + tilesetSc.height - selectionBox.getSize().y));
    }
    // если selectionBox "обрезается"
    if(selectionBox.selectionPos.x == 2) {
        selectionBox.setSize(Vector2f(32, selectionBox.getSize().y));
    }
    if(selectionBox.selectionPos.y == 2) {
        selectionBox.setSize(Vector2f(selectionBox.getSize().x, 32));
    }
}

//--------------------------------------------------
