#pragma once
#include <DataTypes/DataTypes.h>


namespace KinectViz {

class Effect {
public:
	Effect();
	virtual ~Effect() = 0;

	virtual void applyEffect(ColorImage& image) = 0;

	bool enabled;
	int minHeight;
	int maxHeight;
};

}