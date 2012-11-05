#include <DataTypes\DataTypes.h>
#include "Transparency.h"
#include "Util.h"


namespace KinectViz {

Transparency::Transparency() :
	frameCount(0),
	saveBackgroundOnFrame(15),
	alphaScale(500.0f)
{
}

void Transparency::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) {
	// Save the background after some set number of frames, giving the Kinect time to adjust exposure
	frameCount++;
	if (frameCount == saveBackgroundOnFrame) {
		savedBackground.cols = image.cols;
		savedBackground.rows = image.rows;
		image.CopyTo(savedBackground);
	} else if (frameCount < saveBackgroundOnFrame) {
		return;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			// Find hand occupied by this pixel
			auto handId = handsMask.data[y][x];
			if (handId == kMaskUnoccupied)
				continue;

			// Check if hand is within effect layer
			if (!handWithinLayer(kinectData, handId))
				continue;

			const float alpha = std::min(std::max(
				pixelHeight(kinectData, kinectData.hands[handId].centroid) / alphaScale,
				0.1f), 1.0f);
			blendAndSetPixel(image, x, y, savedBackground.data[y][x], alpha);
		}
	}
}

}
