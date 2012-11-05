#include <DataTypes\DataTypes.h>
#include "Skeleton.h"
#include "Util.h"


namespace KinectViz {

void Skeleton::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed) {
	// Draw each hand's skeleton
	for (std::vector<Hand>::const_iterator hand = kinectData.hands.begin(); hand < kinectData.hands.end(); hand++) {
		if (hand->armBase.x == -1 || hand->palmCenter.x == -1)
			continue;
		
		if (!handWithinLayer(kinectData, *hand))
			continue;

		// Arm base to palm
		drawLine(image, hand->armBase.x, hand->armBase.y, hand->palmCenter.x, hand->palmCenter.y, ColorPixel::White);

		// Palm to fingertips
		for (std::vector<Point2Di>::const_iterator finger = hand->fingerTips.begin(); finger < hand->fingerTips.end(); finger++) {
			drawLine(image, hand->palmCenter.x, hand->palmCenter.y, finger->x, finger->y, ColorPixel::White);
		}
	}
}

}
