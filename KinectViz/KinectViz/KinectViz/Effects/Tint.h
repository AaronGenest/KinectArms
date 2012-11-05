#pragma once
#include <map>

#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Tint : public Effect {
public:
	Tint() : defaultColor(ColorPixel(255,0,0)), defaultAlpha(0.5f) { }
	~Tint() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed);


	std::map<int, float> handAlphas;
	std::map<int, ColorPixel> handColors;
	ColorPixel defaultColor;
	float defaultAlpha;
};

}