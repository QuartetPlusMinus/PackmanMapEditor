class Tile {
public:
    int x, y;
    int sx, sy;
    bool isCollidable;

    Tile(int x, int y, int sx, int sy, bool isCollidable);
    Tile();
};

class ScrollingField {
public:
    int x, y;
    int width, height;
    int offsetX, offsetY;

    ScrollingField(int x, int y, int w, int h, int offsetX, int offsetY);
};

class Menu {
public:
	Menu(int x, int y);

	void draw(sf::RenderWindow &window);
    void SelectedItem(int selectedItemIndex, bool selected);

    int selectedItemIndex;
    bool selected;

private:
	sf::Font font;
    sf::RectangleShape navbar;
    sf::Text menu[3];
};
