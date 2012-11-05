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
		// Skip this hand if it's not in effect layer
		if (!handWithinLayer(kinectData, *hand))
			continue;

		// Find fingertip id closest to table and skip this hand if no fingertips are found
		int closestFinger = findClosestFingertip(kinectData, *hand);
		if (closestFinger == -1)
			continue;

		// Pick a circle size and alpha based on proximity to the table
		const int tipHeight = pixelHeight(kinectData, hand->fingerTips[closestFinger]);
		const float radius = std::max(tipHeight * radiusScale, minRadius);
		const float alpha = std::max(1.0f - ((radius - minRadius) / alphaScale), 0.25f);

		// Draw several circles with varying alpha/size to make it look nice
		for (int dr = -5; dr <= 5; dr++) {
			const float fade = ((float)(std::abs(dr))) + 1;
			drawCircle(image, kinectData, handsMask, hand->fingerTips[closestFinger], radius+dr, color, alpha/fade);
		}
	}
}

}