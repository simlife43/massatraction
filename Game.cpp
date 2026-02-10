// === Game.cpp ===
#include "Game.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <random>

Game::Game() : window(nullptr), renderer(nullptr), running(false), gridSize(20) {}
Game::~Game() {}  // hier sollen wir aufräumen ;)

bool Game::init(const char* title) {
    screen_width = WIDTH;
    screen_height = HEIGHT;

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        return false;
    window = SDL_CreateWindow(title, 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        screen_width, screen_height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    running = (window && renderer);

    // init random generator
    std::srand(std::time(nullptr));

    // init fonts
    if (TTF_Init() < 0) 
        return false;
    font = TTF_OpenFont("Font/FreeSans.ttf", 24); // oder eine andere TTF im Ordner
    if (!font) {
        std::cout<<"cant load FreeSans.ttf"<<std::endl;
        return false;
    }
    paused = true;

    // create massive body
    Body bmassiv;
    bmassiv.position = Vector2{WIDTH/2.0, HEIGHT/2.0};
    bmassiv.velocity = Vector2{0.0, 0.0};
    bmassiv.mass = world.massive_mass;
    bmassiv.color = SDL_Color{255, 255, 255, 255};
    bodies.push_back(bmassiv); 
    // create bodies
    for(int i=0; i<world.bodies_count; i++ ){
        float vx = randomFloatMinusHalfToHalf();
        float vy = randomFloatMinusHalfToHalf();
        Body b2;
        b2.position = Vector2{float(std::rand()%WIDTH), float(std::rand()%HEIGHT)};
        b2.velocity = Vector2{vx, vy};
        b2.mass = 5;
        b2.color = SDL_Color{255, 0, 0, 255};
        bodies.push_back(b2);
    }
    return running;
}

std::vector<int> Game::computeHistogram(const int binCount) {
    std::vector<int> histogram(binCount, 0);

    for (const auto& body : bodies) {
        float speed = body.velocity.length();
        int bin = std::min(static_cast<int>((speed / MAXSPEED) * binCount), binCount - 1);
        histogram[bin]++;
    }

    return histogram;
}


float Game::randomFloatMinusHalfToHalf() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
    return dist(gen);
}

void Game::removeCloseBodies(float minDistance) {
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); /* kein ++ */) {
            float dist = bodies[i].position.distance(bodies[j].position);
            if (dist < minDistance) {
                float totalMass = bodies[i].mass + bodies[j].mass;
                Vector2 newVelocity = {
                    (bodies[i].velocity.x * bodies[i].mass + bodies[j].velocity.x * bodies[j].mass) / totalMass,
                    (bodies[i].velocity.y * bodies[i].mass + bodies[j].velocity.y * bodies[j].mass) / totalMass
                };
                bodies[i].mass = totalMass;
                bodies[i].velocity = newVelocity;
                bodies.erase(bodies.begin() + j); // entferne b
            } else {
                ++j;
            }
        }
    }
}

Vector2 Game::computeTotalMomentum() const {
    Vector2 total = {0.0f, 0.0f};

    for (const auto& body : bodies) {
        total.x += body.velocity.x * body.mass;
        total.y += body.velocity.y * body.mass;
    }

    return total;
}

void Game::drawHistogram(int x, int y, int width, int height) {
    int binCount = histogram.size();
    int binWidth = width / binCount;
    int maxValue = *std::max_element(histogram.begin(), histogram.end());

    if (maxValue == 0) return;

    for (int i = 0; i < binCount; ++i) {
        int barHeight = static_cast<int>((histogram[i] / static_cast<float>(maxValue)) * height);
        SDL_Rect rect = {
            x + i * binWidth,
            y + height - barHeight,
            binWidth - 1, // 1px spacing
            barHeight
        };

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 5); // hellblau
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_SPACE:
                    paused = !paused;
                    break;
                default:
                    //snake.changeDirection(e.key.keysym.sym);
                    break;
            }
        }
    }
}

void Game::update() {

    if (paused) {
        SDL_Delay(200);
        return;
    }
    
    Vector2 p = computeTotalMomentum();
    // berechne ob massen zueiander zu nah sind und füge sie zusammen
    if(world.mass_merge)
        removeCloseBodies(world.mindistance);

    // Kräfte berechnen
    std::vector<Vector2> accelerations(bodies.size());
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = 0; j < bodies.size(); ++j) {
            if (i == j) continue;
            Vector2 dir = bodies[j].position - bodies[i].position;
            float dist = std::max(dir.length(), 5.0f); //mindistance);
            Vector2 forceDir = dir.normalized();
            float forceMag = world.G * bodies[i].mass * bodies[j].mass / (dist * dist);
            Vector2 acc = forceDir * (forceMag / bodies[i].mass);
            accelerations[i] += acc;
        }
    }

    // Bewegung aktualisieren mit Randreflexion
    for (size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].velocity += accelerations[i];
        bodies[i].position += bodies[i].velocity;

        if(world.reflexion_on) {
            // Randreflexion
            if (bodies[i].position.x < 5 || bodies[i].position.x > WIDTH - 5) {
                bodies[i].velocity.x *= -1;
                bodies[i].position.x = clamp(bodies[i].position.x, 5.0f, (float)(WIDTH - 5));
            }
            if (bodies[i].position.y < 5 || bodies[i].position.y > HEIGHT - 5) {
                bodies[i].velocity.y *= -1;
                bodies[i].position.y = clamp(bodies[i].position.y, 5.0f, (float)(HEIGHT - 5));
            }
        }
    }

    magnitude = std::sqrt(p.x * p.x + p.y * p.y);

    if(world.histogramm_on)
        histogram = computeHistogram(binCount);
}

void Game::render() {
    // delete the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // draw bodies
    for (auto& body : bodies) {
        body.updateColorFromSpeed();
        body.drawBody(renderer);
    }

    // show bodies count and magnitude
    SDL_Color white = {255, 255, 255, 255};
    std::string scoreText = "Cnt: " + std::to_string(bodies.size()) + " Magnitude: " + std::to_string(magnitude);
    SDL_Surface* surface = TTF_RenderText_Blended(font, scoreText.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int textW, textH;
    SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
    SDL_Rect dstRect = {screen_width - textW - 10, 10, textW, textH};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    // draw Paused
    if (paused) {
        SDL_Color yellow = {255, 255, 0, 255};
        SDL_Surface* pauseSurf = TTF_RenderText_Blended(font, "PAUSED", yellow);
        SDL_Texture* pauseTex = SDL_CreateTextureFromSurface(renderer, pauseSurf);
        int tw, th;
        SDL_QueryTexture(pauseTex, nullptr, nullptr, &tw, &th);
        SDL_Rect dst = {tw / 2, th / 2, tw, th};
        SDL_RenderCopy(renderer, pauseTex, nullptr, &dst);
        SDL_FreeSurface(pauseSurf);
        SDL_DestroyTexture(pauseTex);
    }

    // draw histogram
    if(world.histogramm_on) {
        drawHistogram(0, HEIGHT - histogramHeight, WIDTH, histogramHeight);
    }

    SDL_RenderPresent(renderer);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::clean() {
    std::cout<<"max score: "<<score<<std::endl;
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

