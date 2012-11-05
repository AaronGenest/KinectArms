#include "Effect.h"
#include "Effects/Util.h"


namespace KinectViz {

Effect::Effect() :
	enabled(false),
	minHeight(0),
	maxHeight(std::numeric_limits<int>::max())
{
}


Effect::~Effect() {
}


void Effect::toggle() {
	enabled = !enabled;
}


bool Effect::handWithinLayer(const KinectData& kinectData, const Hand& hand) const {
	const float height = kinectData.table.depth - hand.meanDepth;
	if (height >= minHeight && height <= maxHeight)
		return true;
	else
		return false;
}


bool Effect::handWithinLayer(const KinectData& kinectData, int handId) const {
	// Make sure there's a hand with id specified by mask (should always be true)
	const Hand* hand = handById(kinectData, handId);
	if (hand == nullptr)
		return false;

	// Make sure the pixel belongs to a hand that's within the effect layer
	if (!handWithinLayer(kinectData, *hand))
		return false;
	else
		return true;
}

}