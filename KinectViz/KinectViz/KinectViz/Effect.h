#pragma once
#include <DataTypes/DataTypes.h>


namespace KinectViz {

const GrayPixel kMaskUnoccupied = 255;

class Effect {
public:
	Effect();
	virtual ~Effect() = 0;

	virtual void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) = 0;

	bool enabled;
	int minHeight;
	int maxHeight;


protected:
	bool handWithinLayer(const KinectData& kinectData, const Hand& hand) const;
	bool handWithinLayer(const KinectData& kinectData, int handId) const;
};

}