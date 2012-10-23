#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class MotionBlur : public Effect {
public:
	MotionBlur();
	~MotionBlur() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask);


private:
	static const int historyLength = 20;
	ColorImage imgHistory[historyLength];
	GrayImage maskHistory[historyLength];
	int curBuffer;
};

}