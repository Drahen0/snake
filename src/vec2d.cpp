#include "vec2d.hpp"

Vec2d::Vec2d() {
    this->x = 0;
    this->y = 0;
}
Vec2d::Vec2d(int x, int y) {
    this->x = x;
    this->y = y;
}

std::string Vec2d::toString() { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }

bool Vec2d::operator==(Vec2d& other) { return x == other.x && y == other.y; }

Vec2d Vec2d::operator+(Vec2d& other) {
    Vec2d vec = Vec2d(x + other.x, y + other.y);
    return vec;
}