#include "snake.hpp"
#include <iostream> // std::cout

int main(int argc, char* args[]) {
    std::cout << "Lancement du snake !!" << std::endl;
    try {
        Snake snake;
        snake.run();
    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        return 1;
    }

    return 0;
}