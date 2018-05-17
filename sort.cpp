bool sortByX(const Tile& first, const Tile& second) {
    return (first.x > second.x);
}

bool sortByY(const Tile& first, const Tile& second) {
    return (first.y > second.y);
}

bool sortByXY(const Tile& first, const Tile& second) {
    return (first.x > second.x && first.y == second.y);
}

template <class T>
void sort(std::vector<Tile>& map, const T& func) {
    if(map.size() < 1) {
        return;
    }
    for(int i = 0; i < map.size() - 1; ++i) {
        for(int j = 1; j < map.size() - i - 1; ++j) {
            if(func(map[j], map[j + 1])) {
                std::swap(map[j], map[j + 1]);
            }
        }
    }
}
