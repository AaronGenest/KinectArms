#include <DataTypes\DataTypes.h>
#include "MotionBlur.h"
#include "Util.h"


namespace KinectViz {

MotionBlur::MotionBlur() :
	curBuffer(0)
{
	// Initialize the buffers and clear them
	for (int i = 0; i < historyLength; i++) {
		imgHistory[i].cols = kImageWidth;
		imgHistory[i].rows = kImageHeight;
		maskHistory[i].cols = kImageWidth;
		maskHistory[i].rows = kImageHeight;

		memset(imgHistory[i].data, 0, sizeof(imgHistory[i].data));
		memset(maskHistory[i].data, kMaskUnoccupied, sizeof(maskHistory[i].data));
	}
}


void MotionBlur::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) {
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			// Leave the area where hand currently is alone
			if (handsMask.data[y][x] != kMaskUnoccupied)
				continue;

			unsigned int blendRed = 0;
			unsigned int blendGreen = 0;
			unsigned int blendBlue = 0;
			unsigned int blendSamplesUsed = 0;

			// Blend from each buffer
			for (int buf = 0; buf < historyLength; buf++) {
				// Include only pixels where hand was
				if (maskHistory[buf].data[y][x] != kMaskUnoccupied) {
					blendRed += imgHistory[buf].data[y][x].red;
					blendGreen += imgHistory[buf].data[y][x].green;
					blendBlue += imgHistory[buf].data[y][x].blue;
					blendSamplesUsed++;
				}
			}
			
			// If any blending to be done, average the current pixel and the old ones.
			// Current frame makes up half the contribution, with the other half coming from all the other frames combined.
			if (blendSamplesUsed > 0) {
				blendRed /= blendSamplesUsed;
				blendGreen /= blendSamplesUsed;
				blendBlue /= blendSamplesUsed;

				image.data[y][x].red = (image.data[y][x].red + blendRed) / 2;
				image.data[y][x].green = (image.data[y][x].green + blendGreen) / 2;
				image.data[y][x].blue = (image.data[y][x].blue + blendBlue) / 2;
			}
		}
	}

	// Store current frame in buffers and increment position in ring
	image.CopyTo(imgHistory[curBuffer]);
	handsMask.CopyTo(maskHistory[curBuffer]);
	curBuffer++;
	curBuffer = curBuffer % historyLength;
}

}
