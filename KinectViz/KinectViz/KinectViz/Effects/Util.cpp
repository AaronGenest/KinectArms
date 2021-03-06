#include <DataTypes\DataTypes.h>
#include "Util.h"
#include "../Effect.h"


namespace KinectViz {

// Blend pixel old color with a new color
inline void blendAndSetPixel(ColorImage &img, int x, int y, ColorPixel color, float alpha) {
	if (x >= 0 && x < img.cols && y >= 0 && y < img.rows) {
		const int blendRed = (color.red * alpha) + ((1.0f - alpha) * img.data[y][x].red);
		const int blendGreen = (color.green * alpha) + ((1.0f - alpha) * img.data[y][x].green);
		const int blendBlue = (color.blue * alpha) + ((1.0f - alpha) * img.data[y][x].blue);
		img.data[y][x] = ColorPixel(blendRed, blendGreen, blendBlue);
	}
}

inline void blendAndSetPixel(ColorImage &img, const GrayImage& mask, int x, int y, ColorPixel color, float alpha) {
	if (mask.data[y][x] == kMaskUnoccupied)
		blendAndSetPixel(img, x, y, color, alpha);
}


// Find height of a pixel above the table (as opposed to distance from the Kinect)
int pixelHeight(const KinectData& kinectData, int x, int y) {
	return kinectData.table.depth - kinectData.depthImage.data[y][x];
}

int pixelHeight(const KinectData& kinectData, Point2Di p) {
	return kinectData.table.depth - kinectData.depthImage.data[p.y][p.x];
}


// Find a hand by its id
const Hand* handById(const KinectData& kinectData, int id) {
	for (auto hand = kinectData.hands.begin(); hand != kinectData.hands.end(); hand++)
		if (hand->id == id)
			return &(*hand);
	return nullptr;
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


// Dilate (a standard image processing operation) a hands mask
void dilateMask(GrayImage &img, int amount) {
	static GrayImage tmp;
	img.CopyTo(tmp);

	for (int y = amount; y < img.rows - amount; y++) {
		for (int x = amount; x < img.cols - amount; x++) {
			// Examine neighboring pixels
			for (int dy = -amount; dy <= amount; dy++) {
				for (int dx = -amount; dx <= amount; dx++) {
					if (img.data[y+dy][x+dx] != kMaskUnoccupied) {
						tmp.data[y][x] = img.data[y+dy][x+dx];
						goto donePixel;
					}
				}
			}
			donePixel:;
		}
	}

	tmp.CopyTo(img);
}


// Erode (a standard image processing operation) a grayscale image
void erode(GrayImage &img, int amount) {
	static GrayImage tmp;
	img.CopyTo(tmp);

	// Two-dimensional kernel
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			// Examine neighboring pixels
			int min = 255;
			for (int dy = -amount; dy <= amount; dy++) {
				for (int dx = -amount; dx <= amount; dx++) {
					if (img.data[y+dy][x+dx] < min)
						min = img.data[y+dy][x+dx];
				}
			}
			tmp.data[y][x] = min;
		}
	}

	tmp.CopyTo(img);
}


// Find fingertip id closest to the table for a hand
int findClosestFingertip(const KinectData &data, const Hand& hand) {
	if (hand.fingerTips.size() == 0)
		return -1;

	int closest = 0;
	int closestHeight = pixelHeight(data, hand.fingerTips.front());

	for (int i = 1; i < hand.fingerTips.size(); i++) {
		int iterHeight = pixelHeight(data, hand.fingerTips[i].x, hand.fingerTips[i].y);
		if (iterHeight < closestHeight) {
			closest = i;
			closestHeight = iterHeight;
		}
	}

	return closest;
}


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


// Draw circle on an image (algorithm taken from Wikipedia)
#define setPixel(x, y) { blendAndSetPixel(image, mask, x, y, color, alpha); }
void drawCircle(ColorImage& image, const KinectData& kinectData, const GrayImage& mask, Point2Di center, float radius, ColorPixel color, float alpha) {
	const int x0 = center.x;
	const int y0 = center.y;
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	setPixel(x0, y0 + radius);
	setPixel(x0, y0 - radius);
	setPixel(x0 + radius, y0);
	setPixel(x0 - radius, y0);

	while(x < y)
	{
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if(f >= 0)  {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;    
		setPixel(x0 + x, y0 + y);
		setPixel(x0 - x, y0 + y);
		setPixel(x0 + x, y0 - y);
		setPixel(x0 - x, y0 - y);
		setPixel(x0 + y, y0 + x);
		setPixel(x0 - y, y0 + x);
		setPixel(x0 + y, y0 - x);
		setPixel(x0 - y, y0 - x);
	}
}
#undef setPixel


// Box blur using summed area tables - constant time with respect to kernel size.
// Based off http://www.openprocessing.org/sketch/2934
void boxBlur(GrayImage &img, int size) {
	static GrayImage tmp;
	static int sum[480][640];
	img.CopyTo(tmp);

	// Top row and left column
	sum[0][0] = img.data[0][0];
	for (int x = 1; x < img.cols; ++x) {
		sum[0][x] = sum[0][x-1] + img.data[0][x];
	}
	for (int y = 1; y < img.rows; ++y) {
		sum[y][0] = sum[y-1][0] + img.data[y][0];
	}

	// Rest of summed area table
	for (int y = 1; y < img.rows; y++) {
		for (int x = 1; x < img.cols; x++) {
			sum[y][x] = tmp.data[y][x] + sum[y][x-1] + sum[y-1][x] - sum[y-1][x-1];
		}
	}

	int denom = 1.0f / (float)(((2*size) + 1) * ((2*size) + 1));
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			const int xKernSize = std::min(x, size);
			const int yKernSize = std::min(y, size);
			const int xMin = x - xKernSize;
			const int yMin = y - yKernSize;
			const int pSum = sum[y][x] - sum[yMin][x] - sum[y][xMin] + sum[yMin][xMin];
			const int avg = pSum / std::max(xKernSize * yKernSize, 1);
			tmp.data[y][x] = avg;
		}
	}

	tmp.CopyTo(img);
}

}