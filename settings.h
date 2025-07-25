#pragma once

#include <string>
#include <vector>
#include <array>

using Color = std::array<float, 4>;

// Define a struct to represent your settings
struct Settings {
	float targetFPS{120};
	std::vector<Color> backGroundColors;

	struct Stars {
		float radius;
		Color color;
		int count;
		float minSpeed;
		float maxSpeed;
		int nSegments;
		bool draw;
	} stars;

	bool moveFromMouse;
	float mouseDistance;
	float offsetBounds;
};

// Function to load settings from a JSON file
Settings loadSettings(const std::string& filename);
