#include <DataTypes\DataTypes.h>
#include "Outline.h"
#include "Util.h"


namespace KinectViz {

void Outline::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) {
	// Create a dilated mask
	handsMask.CopyTo(expandedMask);
	dilateMask(expandedMask, 3);

	// Draw outline on pixels in expanded mask but not original mask
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			// Skip pixels that aren't where outline should be
			if (handsMask.data[y][x] != kMaskUnoccupied || expandedMask.data[y][x] == kMaskUnoccupied)
				continue;

			// Make sure the pixel belongs to a hand that's within the effect layer
			if (!handWithinLayer(kinectData, expandedMask.data[y][x]))
				continue;

			// Draw outline pixel
			blendAndSetPixel(image, x, y, outlineColor); 
		}
	}
}

}
