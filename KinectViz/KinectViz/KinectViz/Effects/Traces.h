#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Traces : public Effect {
public:
	Traces() { }
	~Traces() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask);


private:
};

}