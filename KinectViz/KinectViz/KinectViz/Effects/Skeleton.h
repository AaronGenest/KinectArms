#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Skeleton : public Effect {
public:
	Skeleton() { }
	~Skeleton() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask);
};

}