#include <DataTypes\DataTypes.h>
#include "BackgroundRemover.h"


namespace KinectViz {

void BackgroundRemover::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed) {
	// Set any non-hand pixel to white
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (handsMask.data[y][x] == kMaskUnoccupied)
				image.data[y][x] = backgroundColor;
		}
	}
}

}
