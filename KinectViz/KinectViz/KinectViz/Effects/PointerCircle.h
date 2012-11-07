#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"
#include "../OneEuroFilter.h"


namespace KinectViz {

class PointerCircle : public Effect {
public:
	PointerCircle();
	~PointerCircle() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed);


	float radiusScale;
	float alphaScale;
	float minRadius;
	ColorPixel color;


private:
	std::vector<OneEuroFilter> xFilterPerHand;
	std::vector<OneEuroFilter> yFilterPerHand;
};

}