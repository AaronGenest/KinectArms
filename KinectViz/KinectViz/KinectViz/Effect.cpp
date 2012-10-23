#include "Effect.h"


namespace KinectViz {

Effect::Effect() :
	enabled(false),
	minHeight(0),
	maxHeight(std::numeric_limits<int>::max())
{
}


Effect::~Effect() {
}


bool Effect::handWithinLayer(const KinectData& kinectData, const Hand& hand) const {
	const float height = kinectData.table.depth - hand.meanDepth;
	if (height >= minHeight && height <= maxHeight)
		return true;
	else
		return false;
}

}