// === Game.h ===
#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include "Body.h"

struct WorldParameter {
    int bodies_count;
    float G; 
    bool reflexion_on;
    bool histogramm_on;
    int massive_mass;
    float mindistance; 
    bool mass_merge;
};

class Game {
public:
    WorldParameter world;

    Game();
    ~Game();
    bool init(const char* title);
    void run();
    void clean();

private:
    void handleEvents();
    void update();
    void render();
    float randomFloatMinusHalfToHalf();
    Vector2 computeTotalMomentum() const;
    void removeCloseBodies(float minDistance);
    std::vector<int> computeHistogram(const int binCount);
    void drawHistogram(int x, int y, int width, int height);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    std::vector<Body> bodies;
    std::vector<int> histogram;
    float magnitude;

    int gridSize;
    int screen_width;
    int screen_height;
    TTF_Font* font;
    int score;
    bool paused;
};
