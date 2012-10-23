#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Shadow : public Effect {
public:
	Shadow() { }
	~Shadow() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask);
};

}