#include <iostream>

class Window {
public:
    virtual void draw() = 0;
    void getS() {
        std::cout << "Window\n" << a;
    }
protected:
    int a;
};

class WinS : public Window {
    WinS(int A) : a(A) {}
    void draw() {
        std::cout << "Im WinS\n";
    }
};


int main() {
    WinS w(4);
    w.draw();
    // w.getS();

    return 0;
}
