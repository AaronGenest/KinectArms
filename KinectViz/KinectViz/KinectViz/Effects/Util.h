#pragma once


namespace KinectViz {

int pixelHeight(const KinectData& kinectData, int x, int y);
const Hand* handById(const KinectData& kinectData, int id);
inline void blendAndSetPixel(ColorImage &img, int x, int y, ColorPixel color, float alpha = 1.0f);
void drawLine(ColorImage &image, int p1x, int p1y, int p2x, int p2y, ColorPixel color, float alpha = 1.0f);
void dilateMask(GrayImage &img, int amount);

}
