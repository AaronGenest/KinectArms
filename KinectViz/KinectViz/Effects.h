#pragma once
#include "Global/KinectTable_c.h"
#include "GLImage.h"

enum PearlMode {
	PEARLMODE_POINTS,
	PEARLMODE_LINES,
	PEARLMODE_BOTH
};

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

void setTintParams(bool enabled, float minHeight, float maxHeight);
void setArmTint(ColorPixel color, float alpha, int handId);
void setMotionBlurParams(bool enabled, float minHeight, float maxHeight);
void setFingerPointerParams(bool enabled, float minHeight, float maxHeight);
void setTattoosParams(bool enabled, float minHeight, float maxHeight);
void setHandTransparencyParams(bool enabled, float minHeight, float maxHeight);
void setFingerTracesParams(bool enabled, float minHeight, float maxHeight, PearlMode mode = PEARLMODE_LINES, int interval = 250);
void setRainbowArmParams(bool enabled, float minHeight, float maxHeight);
void setShadowParams(bool enabled, float minHeight, float maxHeight);
void setSkeletonParams(bool enabled);
void setArmOutlineParams(bool enabled);

void addShadows(ColorImage &img, BinaryImage &stencil, DepthImage &depth, int tableDepth, int meanHeight);
bool findClosestFingertip(KinectData &data, Point3Di &location);
void drawFingerPointer(ColorImage &img, KinectData &data, BinaryImage &stencil);
void addMotionBlur(ColorImage &img, BinaryImage &handStencil, bool draw = true);
void updateFingerPearls(int timeElapsed);
void drawFingerPearls(ColorImage &img, KinectData &data, BinaryImage &stencil, PearlMode mode = PEARLMODE_BOTH);
void createFingerPearl(ColorImage &img, KinectData &data, PearlMode mode, int elapsedTime);
void setBackgroundImage(ColorImage &img);
void makeRegionTransparent(ColorImage &img, BinaryImage &region, float alpha = 0.5f);
void drawRainbowArm(ColorImage &img, KinectData &data, BinaryImage &stencil);
bool loadTattoos();
void updateTattoos(KinectData &data, bool doTransparency = false);
void drawSkeleton(ColorImage &img, KinectData &data);
void drawArmOutline(ColorImage &img, BinaryImage &stencil);
void applyTint(ColorImage &img, KinectData &data, BinaryImage &stencil, int id);

const std::vector<Tattoo> getTattoos();
