#pragma once

#include <Windows.h>
#include <array>

#include "settings.h"

const Settings settings = loadSettings("settings.json");

class Star {
public:
	float orgx;
	float orgy;
	float x;
	float y;
	float speedx;
	float speedy;
	float radius;
	std::array<float, 4> color;

	Star(const Star&) = delete;
	Star();
	Star(float x, float y, float speed, float angle, float radius, std::array<float, 4> color);

	void render(const int nSegments) const;
	void realRender(const int nSegments) const;
	void emptyRender(const int nSegments) const {}
	static void (Star::* renderFunc)(const int) const;

	void move(const float dt, const POINT mousePos) noexcept;
	void moveWithMouse(const float dt, const POINT mousePos) noexcept;
	void normalMove(const float dt, const POINT mousePos) noexcept;
	static void (Star::* moveFunc)(const float, const POINT) noexcept;

};
