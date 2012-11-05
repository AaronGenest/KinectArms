#pragma once
#include <DataTypes/DataTypes.h>


namespace KinectViz {

const GrayPixel kMaskUnoccupied = 255;
const int kImageWidth = 640;
const int kImageHeight = 480;


class Effect {
public:
	Effect();
	virtual ~Effect() = 0;

	virtual void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed) = 0;

	bool enabled;
	int minHeight;
	int maxHeight;


protected:
	bool handWithinLayer(const KinectData& kinectData, const Hand& hand) const;
	bool handWithinLayer(const KinectData& kinectData, int handId) const;
};


struct Point3i {
	static float Dot(Point3i a, Point3i b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
	static Point3i Cross(Point3i a, Point3i b) { return Point3i(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
	//float Magnitude() { return std::sqrt(x*x + y*y + z*z); }

	Point3i() : x(0), y(0), z(0) { }
	Point3i(int x, int y, int z) : x(x), y(y), z(z) { }
	Point3i(const Point3i& point) : x(point.x), y(point.y), z(point.z) { }

	Point3i& operator=(const Point3i& a) {
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}


	int x;
	int y;
	int z;
};

}