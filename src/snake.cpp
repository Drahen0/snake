#include "snake.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

const int width  = 690;
const int height = 420;

const int TILE_SIZE = 20;

const SDL_Color SDL_WHITE = { 0xff, 0xff, 0xff, 0xff };

Snake::Snake() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error(std::string("Impossible d'initialiser SDL ! Error: ") + SDL_GetError());

    if (TTF_Init() == -1)
        throw std::runtime_error(std::string("Impossible d'initialiser le TTF ! Error: ") + TTF_GetError());

    font = TTF_OpenFont("OpenSans-Regular.ttf", 24);
    if (font == nullptr)
        throw std::runtime_error(std::string("Impossible d'ouvrir la police d'ecriture ! Error: ") + TTF_GetError());

    window = SDL_CreateWindow(
        "Fat Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window)
        throw std::runtime_error(std::string("Impossible de créer la fenêtre ! Error: ") + SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    reset();
}

Snake::~Snake() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Snake::move() {
    if (dir.x == 0 && dir.y == 0)
        return false;

    Vec2d body   = bodies.front();
    Vec2d newpos = body + dir;

    if (newpos.x < 0 || newpos.x >= board.size() || newpos.y < 0 || newpos.y >= board[0].size()) {
        menu = Menu::GameOver;
        return true;
    }

    points--;
    if (board[newpos.x][newpos.y] == TileType::Apple) {
        add_apples(3);
        points += 101;
    } else if (board[newpos.x][newpos.y] == TileType::Snake) {
        menu = Menu::GameOver;
        return true;
    } else {
        board[bodies.back().x][bodies.back().y] = TileType::Empty;
        bodies.pop_back();
    }

    if (points < 0)
        points = 0;

    bodies.push_front(newpos);
    board[newpos.x][newpos.y] = TileType::Snake;
    return false;
}

void Snake::add_apples(int nbr) {
    int empty_tiles = 0;
    int apples      = 0;

    // Compte le nombre de cases vides et le nombre de pommes encore en jeu.
    for (auto col : board) {
        for (auto tile : col) {
            switch (tile) {
            case TileType::Apple:
                apples++;
                break;
            case TileType::Empty:
                empty_tiles++;
                break;
            }
        }
    }

    // Si c'était pas la dernière pomme, on ne rajoute pas de nouvelles pommes
    if (apples > 1)
        return; // Il reste encore des pommes sur le terrain

    for (int i = 0; i < nbr; ++i) {
        if (empty_tiles <= 0)
            break;

        int x, y;
        do {
            x = random.gen(0, board.size() - 1);
            y = random.gen(0, board[0].size() - 1);
        } while (board[x][y] != TileType::Empty);

        board[x][y] = TileType::Apple;
        --empty_tiles;
    }
}

void Snake::run() {
    SDL_Event e;
    SDL_StartTextInput(); // Permet d'écrire dans le chooseprofile.
    while (true) {
        while (SDL_PollEvent(&e))
            if (process_event(e))
                return;

        switch (menu) {
        case Menu::ChooseProfile:
            profile();
            break;
        case Menu::Game:
            update();
            break;

        case Menu::Pause:
            update_pause();
            break;

        case Menu::GameOver:
            update_gameover();
            break;
        }
        // Update screen
        SDL_RenderPresent(renderer);
        SDL_Delay(150);
    }
}

bool Snake::process_event(SDL_Event& evt) {
    if (evt.type == SDL_QUIT)
        return true;
    if (evt.type == SDL_TEXTINPUT) {
        username += evt.text.text;
        return false;
    }
    if (evt.type == SDL_KEYDOWN) {
        switch (evt.key.keysym.sym) {
        case SDLK_BACKSPACE:
            if (menu == Menu::ChooseProfile && username.length() >= 1)
                username.pop_back();
            break;
        case SDLK_KP_ENTER:
        case SDLK_RETURN:
            if (menu == Menu::ChooseProfile && username.length() >= 3) {
                menu = Menu::Game;
                SDL_StopTextInput();
            }
            break;
        case SDLK_s:
            if (menu == Menu::Pause)
                save();
            break;
        case SDLK_r:
            if (menu == Menu::Pause) {
                restore();
                menu = Menu::Game;
            }
            break;
        case SDLK_ESCAPE:
            switch (menu) {
            case Menu::Game:
                menu = Menu::Pause;
                break;
            case Menu::GameOver:
                reset();
                menu = Menu::Game;
                break;
            case Menu::Pause:
                menu = Menu::Game;
                break;
            }
            return false;
        case SDLK_UP:
            newdir = { 0, -1 };
            break;
        case SDLK_DOWN:
            newdir = { 0, 1 };
            break;
        case SDLK_LEFT:
            newdir = { -1, 0 };
            break;
        case SDLK_RIGHT:
            newdir = { 1, 0 };
            break;
        default:
            return false;
        }
    }

    return false;
}
void Snake::profile() {
    SDL_RenderClear(renderer);
    render_text("Username: " + username, SDL_WHITE, { 150, 200 });
}

void Snake::update() {
    // Empèche le joueur de se retourner sur lui-même
    if (bodies.size() == 1 || (newdir.x != -dir.x || newdir.y != -dir.y))
        dir = newdir;

    // bouge le snake
    if (menu == Menu::Game && move()) {
        SDL_RenderPresent(renderer);
        return;
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);

    auto newpos = bodies.front();
    render_text("Niveau: " + std::to_string((bodies.size() - 1) / 3) + " Points: " + std::to_string(points), SDL_WHITE,
        { 0, 0 });

    for (unsigned i = 0; i < board.size(); ++i) {
        for (unsigned j = 0; j < board[i].size(); ++j) {
            if (board[i][j] == TileType::Empty)
                continue;

            if (board[i][j] == TileType::Apple) {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
            } else if (i == newpos.x && j == newpos.y) {
                SDL_SetRenderDrawColor(renderer, 0x66, 0xEE, 0x66, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x30, 0xc0, 0x00, 0xFF);
            }

            SDL_Rect rect;
            rect.x = i * TILE_SIZE;
            rect.y = j * TILE_SIZE;
            rect.h = TILE_SIZE;
            rect.w = TILE_SIZE;
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}

void Snake::update_pause() {
    update();
    render_text("Pause", SDL_WHITE, { 10, height - 130 });
    render_text("Escape pour reprendre", SDL_WHITE, { 10, height - 130 + 30 });
    render_text("S pour sauvegarder", SDL_WHITE, { 10, height - 130 + 60 });
    render_text("R pour charger la partie", SDL_WHITE, { 10, height - 130 + 90 });
}
void Snake::update_gameover() {
    update();
    render_text("Game Over", SDL_WHITE, { width / 2, height / 2 });
}

void Snake::render_text(std::string textureText, SDL_Color textColor, Vec2d pos) {
    if (txtTexture != nullptr) {
        SDL_DestroyTexture(txtTexture);
        txtTexture = nullptr;
    }

    // Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);
    if (textSurface == NULL) {
        throw std::runtime_error(std::string("Impossible d'afficher du texte ! Error: ") + TTF_GetError());
    }

    // Create texture from surface pixels
    txtTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    // Set rendering space and render to screen
    SDL_Rect renderQuad = { pos.x, pos.y, textSurface->w, textSurface->h };
    // Set clip rendering dimensions

    // Render to screen
    SDL_RenderCopyEx(renderer, txtTexture, nullptr, &renderQuad, 0.0, nullptr, SDL_FLIP_NONE);

    // Get rid of old surface
    SDL_FreeSurface(textSurface);
}

void Snake::save() {
    std::string filename(username + ".txt");
    std::ofstream file;

    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir " << filename << std::endl;
        return;
    }

    int w = board.size();
    int h = board[0].size();

    file << "SNAKE\n" << w << ' ' << h << ' ' << points << '\n';
    for (int i = 0; i < w; ++i)
        for (int j = 0; j < h; ++j)
            file << int(board[i][j]) << (j + 1 == h ? '\n' : ' ');

    file << int(bodies.size()) << '\n';
    for (Vec2d body : bodies)
        file << body.x << ' ' << body.y << '\n';

    file.close();
    std::cout << "Sauvegarde réussie" << std::endl;
}

void Snake::restore() {
    reset();
    std::string filename(username + ".txt");
    std::ifstream file;

    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir " << filename << std::endl;
        return;
    }
    std::string header;
    int w, h;

    file >> header >> w >> h >> points;
    if (header != "SNAKE") {
        std::cerr << "Fichier corrompu" << std::endl;
        return;
    }
    if (w != board.size() || h != board[0].size()) {
        std::cerr << "Taille non compatible" << std::endl;
        return;
    }

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            int tmp;
            file >> tmp;
            board[i][j] = TileType(tmp);
        }
    }

    int count;
    file >> count;
    bodies.clear();
    for (int i = 0; i < count; ++i) {
        Vec2d body;
        file >> body.x >> body.y;
        bodies.push_back(body);
    }

    file.close();
}

void Snake::reset() {
    // Clear
    points = 0;
    dir    = Vec2d(0, 0);
    newdir = Vec2d(0, 0);

    bodies.clear();
    board.clear();

    // Initialise la matrice vide
    for (int i = 0; i < width / TILE_SIZE; ++i)
        // ajoute une colonne de taille height / TILE_SIZE avec la valeur "Empty"
        board.emplace_back(height / TILE_SIZE, TileType::Empty);

    // Ajouter la tête du snake au centre du jeu
    bodies.push_back({ width / TILE_SIZE / 2, height / TILE_SIZE / 2 });
    board[bodies.front().x][bodies.front().y] = TileType::Snake;
    add_apples(3);
}