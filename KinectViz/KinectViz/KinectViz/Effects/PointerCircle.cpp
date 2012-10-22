#include <DataTypes/DataTypes.h>

#include "PointerCircle.h"


namespace KinectViz {

void PointerCircle::applyEffect(ColorImage& image) {
	for (int x = 0; x < 640; x++)
		image.data[0][x].red = 255;
}

}