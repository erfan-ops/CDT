#include "star.h"
#include "rendering.h"
#include <cmath>


static const float mouseKeepDistanceSqr = settings.mouseDistance * settings.mouseDistance;

void (Star::* Star::renderFunc)(const int) const = nullptr;
void (Star::* Star::moveFunc)(const float, const POINT) noexcept = nullptr;

Star::Star(float x, float y, float speed, float angle, float radius, std::array<float, 4> color)
    : x(x), y(y), orgx(x), orgy(y), speedx(cosf(angle)*speed), speedy(sinf(angle)*speed), radius(radius), color(color) {}
Star::Star()
    : x(0.0f), y(0.0f), orgx(0.0f), orgy(0.0f), speedx(0.0f), speedy(0.0f), radius(0.0f), color({0.0f, 0.0f , 0.0f , 0.0f}) {}

void Star::move(const float dt, const POINT mousePos) noexcept {
    (this->*moveFunc)(dt, mousePos);
}

void Star::normalMove(const float dt, const POINT mousePos) noexcept {
    this->orgx += this->speedx * dt;
    this->orgy += this->speedy * dt;

    float xdis = this->orgx - this->x;
    float ydis = this->orgy - this->y;

    this->x += xdis * dt;
    this->y += ydis * dt;
}

void Star::moveWithMouse(const float dt, const POINT mousePos) noexcept {
    float mouseDX = static_cast<float>(mousePos.x) - this->x;
    float mouseDY = static_cast<float>(mousePos.y) - this->y;
    float mouseDisSqr = mouseDX * mouseDX + mouseDY * mouseDY;
    if (mouseDisSqr && mouseDisSqr < mouseKeepDistanceSqr) {
        float mouseDis = std::sqrtf(mouseDisSqr);
        float ratio = settings.mouseDistance / mouseDis;
        float newX = mouseDX * ratio;
        float newY = mouseDY * ratio;
        this->x = mousePos.x - newX;
        this->y = mousePos.y - newY;
    }

    this->normalMove(dt, mousePos);
}


void Star::render(const int nSegments) const {
    // Call the function assigned to renderFunc
    (this->*renderFunc)(nSegments);
}

// Actual drawing logic
void Star::realRender(const int nSegments) const {
    filledCircle(x, y, radius, color[0], color[1], color[2], color[3], nSegments);
}

