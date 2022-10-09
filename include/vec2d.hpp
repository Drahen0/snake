#pragma once
#include <string>

struct Vec2d {
    int x, y;

    Vec2d();
    Vec2d(int x, int y);

    std::string toString();

    bool operator==(Vec2d& other);
    Vec2d operator+(Vec2d& other);
};