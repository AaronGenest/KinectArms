#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Transparency : public Effect {
public:
	Transparency();
	~Transparency() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed);

	float alphaScale;


private:
	ColorImage savedBackground;
	int frameCount;
	const int saveBackgroundOnFrame;
};

}