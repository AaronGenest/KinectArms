#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class BackgroundRemover : public Effect {
public:
	BackgroundRemover() : backgroundColor(ColorPixel::White) { }
	~BackgroundRemover() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMas, int timeElapsedk);


	ColorPixel backgroundColor;
};

}