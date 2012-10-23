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


void PointerCircle::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) {
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

#if 0
	// Find fingertips closest to the table
	std::vector<Point3Di> locations;
	findClosestFingertip(data, locations);

	for(std::vector<Point3Di>::iterator it = locations.begin(); it != locations.end(); it++) {
		// Skip hands without fingertips
		if (it->x == -1 && it->y == -1 && it->z == -1)
			continue;

		// Pick a pointer size and alpha based on proximity to table
		float fingerPointerSize = it->z / 7.0f;
		if (fingerPointerSize < minPointerSize)
			fingerPointerSize = minPointerSize;
		float fingerPointerAlpha = 1.0f - ((fingerPointerSize - minPointerSize) / alphaScale);
		if (fingerPointerAlpha < 0.25f)
			fingerPointerAlpha = 0.25f;

		// Draw finger pointer ellipse/circle
		for (int i = -5; i <= 5; i++) {
			// TODO: tweak to make nicer maybe
			float alphaFade = ((float)(abs(i))) + 1;
			//alphaFade = alphaFade < 1.0f ? 1.0f : alphaFade;
			drawCircleOnImage(img, data, stencil, it->x, it->y, (unsigned int)fingerPointerSize+i, fingerPointerAlpha / alphaFade);
		}
	}
#endif
}

}