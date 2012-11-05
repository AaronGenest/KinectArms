#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Outline : public Effect {
public:
	Outline() : outlineColor(ColorPixel(0, 63, 123)) { }
	~Outline() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed);


	ColorPixel outlineColor;


private:
	GrayImage expandedMask;
};

}