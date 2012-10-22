#include <DataTypes\DataTypes.h>
#include "Util.h"


namespace KinectViz {

// Blend pixel old color with a new color
inline void blendAndSetPixel(ColorImage &img, int x, int y, ColorPixel color, float alpha = 1.0f) {
	if (x >= 0 && x < img.cols && y >= 0 && y < img.rows) {
		const int blendRed = (color.red * alpha) + ((1.0f - alpha) * img.data[y][x].red);
		const int blendGreen = (color.green * alpha) + ((1.0f - alpha) * img.data[y][x].green);
		const int blendBlue = (color.blue * alpha) + ((1.0f - alpha) * img.data[y][x].blue);
		img.data[y][x] = ColorPixel(blendRed, blendGreen, blendBlue);
	}
}

// Like the other blendAndSetPixel, but uses a stencil, not drawing on pixels where the stencil is 0
/*void blendAndSetPixel(ColorImage &img, KinectData &data, BinaryImage &stencil, int x, int y, float alpha = 1.0f, ColorPixel color = ColorPixel::Blue) {
	// If pixel is not in stenciled region then just draw it. If it is, we need to partially occlude it if hand is transparent.
	if(!stencil.data[y][x])
		blendAndSetPixel(img, x, y, alpha, color);
	else if (enableTransparentHand) {
		// Check each hand to see if pixel lies within it
		for (int i = 0; i < data.hands.size(); i++) {
			if (stencil.data[data.hands[i].centroid.y][data.hands[i].centroid.x]) {  // is it this hand that pixel is within?
				float regionAlpha = getHandTransparency(data.table.depth, data.depthImage.data[data.hands[i].centroid.y][data.hands[i].centroid.x]);
				blendAndSetPixel(img, x, y, alpha * regionAlpha, color);
				break;
			}
		}
	}
}*/

// Draw a line - taken from http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx
#define setPixel(x, y) { blendAndSetPixel(image, x, y, color, alpha); }
void drawLine(ColorImage &image, int p1x, int p1y, int p2x, int p2y, ColorPixel color, float alpha) {
	int F, x, y;

	if (p1x > p2x) { // Swap points if p1 is on the right of p2
		std::swap(p1x, p2x);
		std::swap(p1y, p2y);
	}

	// Handle trivial cases separately for algorithm speed up.
	// Trivial case 1: m = +/-INF (Vertical line)
	if (p1x == p2x) {
		if (p1y > p2y)  // Swap y-coordinates if p1 is above p2
			std::swap(p1y, p2y);

		x = p1x;
		y = p1y;
		while (y <= p2y) {
			setPixel(x, y);
			y++;
		}
		return;
	}
	// Trivial case 2: m = 0 (Horizontal line)
	else if (p1y == p2y) {
		x = p1x;
		y = p1y;

		while (x <= p2x) {
			setPixel(x, y);
			x++;
		}
		return;
	}

	int dy            = p2y - p1y;  // y-increment from p1 to p2
	int dx            = p2x - p1x;  // x-increment from p1 to p2
	int dy2           = (dy << 1);  // dy << 1 == 2*dy
	int dx2           = (dx << 1);
	int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
	int dy2_plus_dx2  = dy2 + dx2;

	if (dy >= 0) {   // m >= 0
		// Case 1: 0 <= m <= 1 (Original case)
		if (dy <= dx)    {
			F = dy2 - dx;    // initial F

			x = p1x;
			y = p1y;
			while (x <= p2x) {
				setPixel(x, y);
				if (F <= 0)
					F += dy2;
				else {
					y++;
					F += dy2_minus_dx2;
				}
				x++;
			}
		}
		// Case 2: 1 < m < INF (Mirror about y=x line
		// replace all dy by dx and dx by dy)
		else {
			F = dx2 - dy;    // initial F

			y = p1y;
			x = p1x;
			while (y <= p2y) {
				setPixel(x, y);
				if (F <= 0)
					F += dx2;
				else {
					x++;
					F -= dy2_minus_dx2;
				}
				y++;
			}
		}
	}
	else    // m < 0
	{
		// Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
		if (dx >= -dy)
		{
			F = -dy2 - dx;    // initial F

			x = p1x;
			y = p1y;
			while (x <= p2x)
			{
				setPixel(x, y);
				if (F <= 0)
					F -= dy2;
				else {
					y--;
					F -= dy2_plus_dx2;
				}
				x++;
			}
		}
		// Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
		// about y=x line, replace all dx by -dy and dy by dx)
		else    
		{
			F = dx2 + dy;    // initial F

			y = p1y;
			x = p1x;
			while (y >= p2y)
			{
				setPixel(x, y);
				if (F <= 0)
					F += dx2;
				else {
					x++;
					F += dy2_plus_dx2;
				}
				y--;
			}
		}
	}
}
#undef setPixel

}