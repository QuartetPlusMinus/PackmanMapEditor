#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include "Tile.cpp"

using namespace sf;

const int mapWidth = 480;
const int mapHeight = 768;

std::vector<Tile> map;

void loadMap(char* file) {
    map.clear();
    std::ifstream in;
    in.open(file);
    Tile tmp;

    while(!in.eof()) {
        in >> tmp.x;
        in >> tmp.y;
        in >> tmp.sx;
        in >> tmp.sy;
        in >> tmp.isCollidable;
        map.push_back(tmp);
    }
    in.close();
}


int main() {
    RenderWindow window(VideoMode(mapWidth, mapHeight), "Map");

    Texture tileset;
    tileset.loadFromFile("tileset.png");

    Sprite tile(tileset);

    loadMap("out.txt");

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear(Color(153, 221, 255));

        for(int i = 0; i < map.size(); i++) {
            tile.setTextureRect(IntRect(map.at(i).sx * 32, map.at(i).sy * 32, 32, 32));
            tile.setPosition(map.at(i).x * 32, map.at(i).y * 32);
            window.draw(tile);
        }

        window.display();
    }

    return 0;
}


// int mapIter = 0;
// for(int iterY = 0; iterY < mapHeight / 32; ++iterY) {
//     for(int iterX = 0; iterX < mapWidth / 32; ++iterX) {
//         if(mapIter > map.size()) {
//             break;
//         }
//         tile.setTextureRect(IntRect(map.at(mapIter).sx * 32, map.at(mapIter).sy * 32, 32, 32));
//         tile.setPosition(Vector2f(iterX * 32, iterY * 32));
//         window.draw(tile);
//         ++mapIter;
//     }
// }

// for(int i = 0; i < map.size(); i++) {
//     tile.setTextureRect(IntRect(map.at(i).sx * 32, map.at(i).sy * 32, 32, 32));
//     tile.setPosition(map.at(i).x * 32, map.at(i).y * 32);
//     window.draw(tile);
// }
