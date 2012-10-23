#include <cmath>

#include <DataTypes\DataTypes.h>
#include "Rainbow.h"
#include "Util.h"


namespace KinectViz {

Rainbow::Rainbow() :
	alpha(0.25f),
	frequency(3.0f)
{
	// Build depth-to-color table
	for (int i = 0; i < maxDepth; i++) {
		float hue = std::fmod((i / (float)maxDepth) * 360.0f * frequency, 360.0f);
		depthToRgb[i] = hsvToRgb(Point3Df(hue, 1.0f, 1.0f));
	}
}


void Rainbow::applyEffect(ColorImage& image, KinectData& kinectData, BinaryImage& handsMask) {
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			// If pixel unoccupied by hand, skip it
			if (!handsMask.data[y][x])
				continue;

			// Blend the color with the arm
			ColorPixel &color = depthToRgb[(int)(kinectData.table.depth - kinectData.depthImage.data[y][x])];
			blendAndSetPixel(image, x, y, color, alpha);
		}
	}
}


// Based on http://wiki.beyondunreal.com/HSV-RGB_Conversion
ColorPixel Rainbow::hsvToRgb(const Point3Df hsv) {
	float Min, Chroma, Hdash, X;
	Point3Df RGB;
 
	Chroma = hsv.y * hsv.z;
	Hdash = hsv.x / 60.0f;
	float modFactor = std::fmod(Hdash, 2.0f);
	X = Chroma * (1.0f - std::abs(modFactor - 1.0f));
 
	if(Hdash < 1.0f) {
		RGB.x = Chroma;
		RGB.y = X;
	} else if(Hdash < 2.0f) {
		RGB.x = X;
		RGB.y = Chroma;
	} else if(Hdash < 3.0f) {
		RGB.y = Chroma;
		RGB.z = X;
	} else if(Hdash < 4.0f) {
		RGB.y= X;
		RGB.z = Chroma;
	} else if(Hdash < 5.0f) {
		RGB.x = X;
		RGB.z = Chroma;
	} else if(Hdash < 6.0f) {
		RGB.x = Chroma;
		RGB.z = X;
	}
 
	Min = hsv.z - Chroma;
	RGB.x += Min;
	RGB.y += Min;
	RGB.z += Min;

	RGB.x *= 255;
	RGB.y *= 255;
	RGB.z *= 255;
 
	ColorPixel RGBi((int)RGB.x, (int)RGB.y, (int)RGB.z);
	return RGBi;
}

}
