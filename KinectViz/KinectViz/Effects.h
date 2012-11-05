#pragma once
#include "Global/KinectTable_c.h"
#include "GLImage.h"


struct Point3Di {
	static float Dot(Point3Di a, Point3Di b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
	static Point3Di Cross(Point3Di a, Point3Di b) { return Point3Di(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
	//float Magnitude() { return std::sqrt(x*x + y*y + z*z); }

	Point3Di()
		: x(0), y(0), z(0)
	{ return; }

	Point3Di(int x, int y, int z)
		: x(x), y(y), z(z)
	{ return; }

	Point3Di(const Point3Di& point, int z)
		: x(point.x), y(point.y), z(z)
	{ return; }

	Point3Di(const Point3Di& point)
		: x(point.x), y(point.y), z(point.z)
	{ return; }

	Point3Di& operator=(const Point3Di& a) {
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	int x;
	int y;
	int z;
};

void applyVizEffects(KinectData &data, ColorImage &colorImage);

void setTattoosParams(bool enabled, float minHeight, float maxHeight);
bool loadTattoos();
void updateTattoos(KinectData &data, bool doTransparency = false);

const std::vector<Tattoo> getTattoos();
