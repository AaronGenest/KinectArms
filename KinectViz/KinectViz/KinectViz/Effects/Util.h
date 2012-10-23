#pragma once
#include <DataTypes/DataTypes.h>


namespace KinectViz {

int pixelHeight(const KinectData& kinectData, int x, int y);
int pixelHeight(const KinectData& kinectData, Point2Di p);

const Hand* handById(const KinectData& kinectData, int id);
int findClosestFingertip(const KinectData &data, const Hand& hand);

inline void blendAndSetPixel(ColorImage &img, int x, int y, ColorPixel color, float alpha = 1.0f);
inline void blendAndSetPixel(ColorImage &img, const GrayImage& mask, int x, int y, ColorPixel color, float alpha = 1.0f);
void drawLine(ColorImage &image, int p1x, int p1y, int p2x, int p2y, ColorPixel color, float alpha = 1.0f);
void drawCircle(ColorImage& image, const KinectData& kinectData, const GrayImage& mask, Point2Di center, float radius, ColorPixel color, float alpha = 1.0f);

void dilateMask(GrayImage &img, int amount);

}
