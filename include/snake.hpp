#pragma once
#include "random.hpp"
#include "vec2d.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <list>
#include <string>
#include <vector>

enum class TileType { Snake, Apple, Empty };
enum class Menu {  ChooseProfile, Game, Pause, GameOver };

class Snake {
    std::vector<std::vector<TileType>> board;
    std::list<Vec2d> bodies;
    Vec2d dir;
    Vec2d newdir;

    Menu menu  = Menu::ChooseProfile;
    int points = 0;
    std::string username;

public:
    Snake();
    ~Snake();

    bool move();
    void add_apples(int nbr);
    void save();
    void restore();
    void reset();

    void profile();
    void run();
    void update();
    void update_pause();
    void update_gameover();
    bool process_event(SDL_Event& e);
    void render_text(std::string text, SDL_Color color, Vec2d pos);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* txtTexture = nullptr;
    Random random;
};
