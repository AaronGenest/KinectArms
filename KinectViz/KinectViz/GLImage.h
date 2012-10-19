#pragma once
#include <string>

struct TextureImage {
	int id;
	float normWidth;
	float normHeight;
};

struct TintColor {
	TintColor() { red = 1.0f; green = 1.0f; blue = 1.0f; }
	TintColor(float red, float green, float blue) { this->red = red; this->blue = blue; this->green = green; }
	float red;
	float green;
	float blue;
};

struct GLImage {
	int x;
	int y;
	float scale;
	float angle;
	float alpha;
	TintColor tint;
	bool visible;

	TextureImage tex;
};

typedef struct GLImage Tattoo;

bool loadTextureImage(std::string filename, TextureImage &tex);
