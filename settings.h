#pragma once

#include <string>
#include <vector>
#include <array>

// Define a struct to represent your settings
struct Settings {
	float targetFPS{120};
	std::vector<std::array<float, 4>> backGroundColors;

	struct Stars {
		float radius;
		std::array<float, 4> color;
		int count;
		float minSpeed;
		float maxSpeed;
		int nSegments;
		bool draw;
	} stars;

	float offsetBounds;
};

// Function to load settings from a JSON file
Settings loadSettings(const std::string& filename);
