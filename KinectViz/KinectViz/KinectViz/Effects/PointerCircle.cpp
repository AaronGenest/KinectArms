#include <DataTypes/DataTypes.h>

#include "PointerCircle.h"
#include "Util.h"


namespace KinectViz {

PointerCircle::PointerCircle() :
	color(ColorPixel::Blue),
	radiusScale(0.15f),
	alphaScale(80.0f),
	minRadius(5.0f)
{
}


void PointerCircle::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed) {
	for (auto hand = kinectData.hands.begin(); hand != kinectData.hands.end(); hand++) {
		if (hand->id == -1)
			continue;

		// Skip this hand if it's not in effect layer
		if (!handWithinLayer(kinectData, *hand))
			continue;

		// Find fingertip id closest to table and skip this hand if no fingertips are found
		int closestFinger = findClosestFingertip(kinectData, *hand);
		if (closestFinger == -1)
			continue;
		auto closestTip = hand->fingerTips[closestFinger];

		// Expand the per-hand buffers if needed
		while (hand->id >= xFilterPerHand.size()) {
			xFilterPerHand.push_back(OneEuroFilter(1.0f, 1.0f, 0.1f));
			yFilterPerHand.push_back(OneEuroFilter(1.0f, 1.0f, 0.1f));
		}

		// Add and filter the new data point
		const float filteredX = xFilterPerHand[hand->id].filter(closestTip.x, (float)timeElapsed);
		const float filteredY = yFilterPerHand[hand->id].filter(closestTip.y, (float)timeElapsed);
		const Point2Di filteredTip((int)filteredX, (int)filteredY);

		// Pick a circle size and alpha based on proximity to the table
		const int tipHeight = pixelHeight(kinectData, closestTip);
		const float radius = std::max(tipHeight * radiusScale, minRadius);
		const float alpha = std::max(1.0f - ((radius - minRadius) / alphaScale), 0.25f);

		// Draw several circles with varying alpha/size to make it look nice
		for (int dr = -5; dr <= 5; dr++) {
			const float fade = ((float)(std::abs(dr))) + 1;
			drawCircle(image, kinectData, handsMask, filteredTip, radius+dr, color, alpha/fade);
		}
	}
}

}