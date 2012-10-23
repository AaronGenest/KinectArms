#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Tattoo : public Effect {
public:
	Tattoo() { }
	~Tattoo() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask);
};

}