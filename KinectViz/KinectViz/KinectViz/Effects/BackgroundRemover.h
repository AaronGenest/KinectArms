#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class BackgroundRemover : public Effect {
public:
	BackgroundRemover() { }
	~BackgroundRemover() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, BinaryImage& handsMask);
};

}