#include <DataTypes/DataTypes.h>

#include "PointerCircle.h"


namespace KinectViz {

void PointerCircle::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask) {
	for (int x = 0; x < 640; x++)
		image.data[100][x].red = 255;
}

}