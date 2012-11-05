#include <algorithm>
#include <DataTypes\DataTypes.h>
#include "Shadow.h"
#include "Util.h"


namespace KinectViz {

Shadow::Shadow() :
	maxShadowHeight(800),
	minScaledHeight(80)
{
	depthMap.cols = kImageWidth;
	depthMap.rows = kImageHeight;
}


void Shadow::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed) {
	if (kinectData.hands.size() == 0)
		return;

	// Build a map of scale depth of hand regions.
	// Iterate in reverse due to direction of shadow offset.
	memset(depthMap.data, 255, sizeof(depthMap.data));
	for (int y = image.rows-1; y >= 0; y--) {
		for (int x = image.cols-1; x >= 0; x--) {
			// Skip non-hand pixels and pixels for hand that isn't in effect layer
			if (handsMask.data[y][x] == kMaskUnoccupied)
				continue;
			if (!handWithinLayer(kinectData, handsMask.data[y][x]))
				continue;

			// 0-255 scaled value of height
			const float height = 255.0f * (pixelHeight(kinectData, x, y) / (float)maxShadowHeight);

			// Offset of shadow pixel - skip pixels that fall off image when offset
			const int offset = height / 2;
			if (x + offset >= image.cols || y + offset >= image.rows)
				continue;

			depthMap.data[y+offset][x+offset] = (GrayPixel)(std::max((int)height, minScaledHeight));
		}
	}

	// Blur the depth map image.
	// NOTE: for now, blur amount is based only on the first hand and applies to all hands.
	// In the future, doing a pass of each hand separately then combining would be a good idea.
	const int blurSize = (pixelHeight(kinectData, kinectData.hands[0].palmCenter) / 10) + 1;
	erode(depthMap, 1);  // gets rid of the noise from building the depthStencil table
	boxBlur(depthMap, blurSize);

	// Draw the blurred shadow image
	for (int y = 0; y < depthMap.rows; y++) {
		for (int x = 0; x < depthMap.cols; x++) {
			// Don't shadow the hand itself
			if (handsMask.data[y+2][x+2] == kMaskUnoccupied) {  // give a little leeway in the shadow direction
				image.data[y][x].red *= (depthMap.data[y][x] / 255.0f);
				image.data[y][x].green *= (depthMap.data[y][x] / 255.0f);
				image.data[y][x].blue *= (depthMap.data[y][x] / 255.0f);
			}
		}
	}
}

}
