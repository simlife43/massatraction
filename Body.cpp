// === Food.cpp ===
#include "Body.h"

void Body::updateColorFromSpeed() {
        // Berechne Geschwindigkeit (Länge des Vektors)
        float speed = velocity.length();

        // Normiere Geschwindigkeit in Bereich [0.0, 1.0] – z. B. maxSpeed = 5.0
        float t = clamp(speed / MAXSPEED, 0.0f, 1.0f);

        // Interpolation von langsam (rot, dunkel) zu schnell (blau, hell)
        // Rot:   (R=255, G=0,   B=0)   bei t = 0
        // Blau:  (R=0,   G=128, B=255) bei t = 1
        Uint8 r = static_cast<Uint8>((1.0f - t) * 255);
        Uint8 g = static_cast<Uint8>(t * 255);
        Uint8 b = static_cast<Uint8>(t * 255);
        Uint8 a = 255;

        color = SDL_Color{r, g, b, a};
}

void Body::drawBody(SDL_Renderer* renderer) {
    // Körper zeichnen
    if(position.x>=0 && position.y>=0 && position.x<=WIDTH && position.y<=HEIGHT) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_Rect rect = {(int)(position.x - 1), (int)(position.y - 1), 2, 2};
        SDL_RenderFillRect(renderer, &rect);
    }
}