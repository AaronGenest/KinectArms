#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Rainbow : public Effect {
public:
	Rainbow();
	~Rainbow() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed);


	float alpha;


private:
	ColorPixel hsvToRgb(const Point3Df hsv);


	static const int maxDepth = 1000;
	const float frequency;
	ColorPixel depthToRgb[maxDepth];  // size of array is based on maximum height above table to support
};

}