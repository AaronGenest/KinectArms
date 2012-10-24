#include <DataTypes\DataTypes.h>
#include "Tint.h"
#include "Util.h"


namespace KinectViz {

void Tint::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) {
	if (kinectData.hands.size() == 0)
		return;

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			// Find hand occupied by this pixel
			auto handId = handsMask.data[y][x];
			if (handId == kMaskUnoccupied)
				continue;

			// Check if hand is within effect layer
			if (!handWithinLayer(kinectData, handId))
				continue;

			// Use assigned color/alpha if set, otherwise use defaults
			const float alpha = handAlphas.count(handId) > 0 ? handAlphas[handId] : defaultAlpha;
			const ColorPixel color = handColors.count(handId) > 0 ? handColors[handId] : defaultColor;

			blendAndSetPixel(image, x, y, color, alpha);
		}
	}
}

}
