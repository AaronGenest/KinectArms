/*******************************************************************************
 * NOTE:
 * This is the old, ugly version of the effects system that was never meant for
 * use as an API. A rewrite is in the works.
 ******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <math.h>

// Project
#include "View.h"
#include "Effects.h"

// KinectTable
#include "Global/KinectTable_c.h"
#include "Util\Helpers.h"
#include "GLImage.h"

bool enableMotionBlur = false;
static float motionBlurMinHeight = 0.0f;
static float motionBlurMaxHeight = 800.0f;

 bool enableFingerPointer = false;
static float fingerPointerMinHeight = 000.0f;
static float fingerPointerMaxHeight = 800.0f;

bool enableTattoos = false;
bool tattooWithinLayer = false;
static float tattooMinHeight = 0.0f;
static float tattooMaxHeight = 800.0f;

bool enableTransparentHand = false;
static float transparentMinHeight = 0.0f;
static float transparentMaxHeight = 800.0f;

 bool enableFingerPearls = false;
PearlMode fingerPearlMode = PEARLMODE_LINES;
static float fingerPearlMinHeight = 0.0f;
static float fingerPearlMaxHeight = 800.0f;

 bool enableRainbowArm = false;
static float rainbowMinHeight = 0.0f;
static float rainbowMaxHeight = 800.0f;

bool enableShadow = false;
static float shadowMinHeight = 0.0f;
static float shadowMaxHeight = 800.0f;

 bool enableTint = false;
static float tintMinHeight = 0.0f;
static float tintMaxHeight = 800.0f;
std::vector<ColorPixel> tintColors;
std::vector<float> tintAlpha;

bool enableSkeleton = false;
bool removeBackground = false;
bool enableArmOutline = false;

static unsigned int fingerPearlInterval = 250;
float handAlphaScale = 700.0f;

const float PI = 3.14159265358979323846264f;
static const int motionBlurBufferSize = 20;
static const int maxDepth = 800;

static bool equalBlurWeight = false;
static bool twoPassBlur = false;
static bool useStaticBackground = true;
static float rainbowFrequency = 3.0f;
static float alphaScale = 80.0f;
static float tattooScale = 50000.0f;
static float minPointerSize = 5.0f;
static int pearlMaxAge = 3000;
static bool squigglyLines = false;
extern float handAlphaScale;

static std::vector< std::vector<Point3Di> > pearls;
std::vector<Tattoo> tattoos;
static ColorImage background;


/******************************
 * Setting effects parameters *
 *****************************/

void setMotionBlurParams(bool enabled, float minHeight, float maxHeight) {
	enableMotionBlur = enabled;
	motionBlurMinHeight = minHeight;
	motionBlurMaxHeight = maxHeight;
}
void setFingerPointerParams(bool enabled, float minHeight, float maxHeight) {
	enableFingerPointer = enabled;
	fingerPointerMinHeight = minHeight;
	fingerPointerMaxHeight = maxHeight;
}
void setTattoosParams(bool enabled, float minHeight, float maxHeight) {
	enableTattoos = enabled;
	tattooMinHeight = minHeight;
	tattooMaxHeight = maxHeight;
}
void setHandTransparencyParams(bool enabled, float minHeight, float maxHeight) {
	enableTransparentHand = enabled;
	transparentMinHeight = minHeight;
	transparentMaxHeight = maxHeight;
}
void setFingerTracesParams(bool enabled, float minHeight, float maxHeight, PearlMode mode, int interval) {
	enableFingerPearls = enabled;
	fingerPearlMinHeight = minHeight;
	fingerPearlMaxHeight = maxHeight;
	fingerPearlMode = mode;
	fingerPearlInterval = interval;
}
void setRainbowArmParams(bool enabled, float minHeight, float maxHeight) {
	enableRainbowArm = enabled;
	rainbowMinHeight = minHeight;
	rainbowMaxHeight = maxHeight;
}
void setShadowParams(bool enabled, float minHeight, float maxHeight) {
	enableShadow = enabled;
	shadowMinHeight = minHeight;
	shadowMaxHeight = maxHeight;
}
void setSkeletonParams(bool enabled) {
	enableSkeleton = enabled;
}
void setArmOutlineParams(bool enabled) {
	enableArmOutline = enabled;
}
void setTintParams(bool enabled, float minHeight, float maxHeight) {
	enableTint = enabled;
	tintMinHeight = minHeight;
	tintMaxHeight = maxHeight;
}
void setArmTint(ColorPixel color, float alpha, int handId) {
	tintColors.resize(handId + 1);
	tintColors[handId] = color;
	tintAlpha.resize(handId + 1);
	tintAlpha[handId] = alpha;
}


/*********************
 * Utility functions *
 *********************/

// ORs a binary image with itself and another image
void binaryImageOr(BinaryImage &dest, BinaryImage &other) {
	assert(dest.rows == other.rows && dest.cols == other.cols);
	for (int y = 0; y < dest.rows; y++) {
		for (int x = 0; x < dest.cols; x++) {
			dest.data[y][x] = dest.data[y][x] || other.data[y][x];
		}
	}
}

// Used for consistent calculation of how transparent the hand should be at a given depth
static inline float getHandTransparency(int tableDepth, int handDepth) {
	float regionAlpha = 1.0f - ((tableDepth + 20 - handDepth) / 700.0f);
	regionAlpha = (regionAlpha < 0.1f) ? 0.1f : regionAlpha;
	regionAlpha = (regionAlpha > 1.0f) ? 1.0f : regionAlpha;
	return regionAlpha;
}

// Set pixel to a new color (hardcoded as blue currently) and blend it with old pixel
static inline void blendAndSetPixel(ColorImage &img, int x, int y, float alpha = 1.0f, ColorPixel color = ColorPixel::Blue) {
	if (x >= 0 && x < img.cols && y >= 0 && y < img.rows) {
		const int blendRed = (color.red * alpha) + ((1.0f - alpha) * img.data[y][x].red);
		const int blendGreen = (color.green * alpha) + ((1.0f - alpha) * img.data[y][x].green);
		const int blendBlue = (color.blue * alpha) + ((1.0f - alpha) * img.data[y][x].blue);
		img.data[y][x] = ColorPixel(blendRed, blendGreen, blendBlue);
	}
}

// Like the other blendAndSetPixel, but uses a stencil, not drawing on pixels where the stencil is 0
static inline void blendAndSetPixel(ColorImage &img, KinectData &data, BinaryImage &stencil, int x, int y, float alpha = 1.0f, ColorPixel color = ColorPixel::Blue) {
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
}

// Find x,y,depth location of fingertip closest to table
bool findClosestFingertip(KinectData &data, std::vector<Point3Di> &locations) {
	if (data.hands.size() == 0)
		return false;
	if (data.hands[0].fingerTips.size() == 0)
		return false;

	// Find closest fingertip to table for each hand and push it into locations vector
	for (int handIndex = 0; handIndex < data.hands.size(); handIndex++) {
		Hand& hand = data.hands[handIndex];
		if (hand.fingerTips.size() == 0) {
			// If not fingertips for this hand, make it -1,-1,-1
			locations.push_back(Point3Di(-1, -1, -1));
			continue;
		}

		Point2Di &closestFingertip = hand.fingerTips[0];
		int closestTipHeight = data.table.depth - data.depthImage.data[closestFingertip.y][closestFingertip.x];

		// Find the fingertip closest to the table
		for(int j = 0; j < hand.fingerTips.size(); j++) {
			float tipHeight = data.table.depth - data.depthImage.data[hand.fingerTips[j].y][hand.fingerTips[j].x];
			if (tipHeight < closestTipHeight) {
				closestFingertip = hand.fingerTips[j];
				closestTipHeight = tipHeight;
			}
		}
		
		Point3Di loc(closestFingertip.x, closestFingertip.y, closestTipHeight);
		locations.push_back(loc);
	}

	return true;
}

// Draw circle on an image (algorithm taken from Wikipedia)
void drawCircleOnImage(ColorImage &img, KinectData &data, BinaryImage &stencil, unsigned int x0, unsigned int y0, unsigned int radius, float alpha = 1.0f) {
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	blendAndSetPixel(img, data, stencil, x0, y0 + radius, alpha);
	blendAndSetPixel(img, data, stencil, x0, y0 - radius, alpha);
	blendAndSetPixel(img, data, stencil, x0 + radius, y0, alpha);
	blendAndSetPixel(img, data, stencil, x0 - radius, y0, alpha);

	while(x < y)
	{
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if(f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;    
		blendAndSetPixel(img, data, stencil, x0 + x, y0 + y, alpha);
		blendAndSetPixel(img, data, stencil, x0 - x, y0 + y, alpha);
		blendAndSetPixel(img, data, stencil, x0 + x, y0 - y, alpha);
		blendAndSetPixel(img, data, stencil, x0 - x, y0 - y, alpha);
		blendAndSetPixel(img, data, stencil, x0 + y, y0 + x, alpha);
		blendAndSetPixel(img, data, stencil, x0 - y, y0 + x, alpha);
		blendAndSetPixel(img, data, stencil, x0 + y, y0 - x, alpha);
		blendAndSetPixel(img, data, stencil, x0 - y, y0 - x, alpha);
	}
}

// Draw a line - taken from http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx
#define setPixel(x, y) { blendAndSetPixel(img, data, stencil, x, y, alpha, color); }
void lineBresenham(ColorImage &img, KinectData &data, BinaryImage &stencil, int p1x, int p1y, int p2x, int p2y, float alpha = 1.0f, ColorPixel color = ColorPixel(220,150,0)) {
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

// Don't use - use the fast version below (left in here for now just in case)
void boxBlur(GrayImage &img, int size) {
	static GrayImage tmp;
	img.CopyTo(tmp);

	if (twoPassBlur) {
		// Horizontal pass
		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.rows; x++) {
				/*if (img.data[y][x] != 255 && img.data[y][x+2] != 255 && img.data[y][x-2] != 255 && img.data[y+2][x] != 255 && img.data[y-2][x] != 255) {
					tmp.data[y][x] = 255;
					continue;
				}*/

				int sum = 0;
				for (int d = -16; d <= 16; d += 4) {
					if(x + d > 0 && x + d < img.cols) {
						sum += img.data[y][x+d];
					}
				}
				tmp.data[y][x] = sum / 9;
			}
		}

		// Vertical pass
		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.rows; x++) {
				/*if (img.data[y][x] != 255 && img.data[y][x+2] != 255 && img.data[y][x-2] != 255 && img.data[y+2][x] != 255 && img.data[y-2][x] != 255) {
					tmp.data[y][x] = 255;
					continue;
				}*/
				
				int sum = 0;
				for (int d = -16; d <= 16; d += 4) {
					if(y + d > 0 && y + d < img.rows) {
						sum += img.data[y+d][x];
					}
				}
				tmp.data[y][x] = sum / 9;
			}
		}
	} else {
		// Two-dimensional kernel
		const int denominator = 81;
		//const int denominator = ((size/(size/4))*2 + 1);  // tried to allow sizes that aren't multiples of 4 to no luck
		size = ((size+2)/4) * 4;  // round it to a multiple of 4 (needed for skipping pixels in inner loops)
		const int multSize = (size<4) ? 4 : size;  // minimum of 4
		for (int y = multSize; y < img.rows - multSize; y++) {
			for (int x = multSize; x < img.cols - multSize; x++) {
				// Don't shade pixels occupied by arm or within two pixels of arm edge
				/*if (img.data[y][x] != 255 && img.data[y][x+2] != 255 && img.data[y][x-2] != 255 && img.data[y+2][x] != 255 && img.data[y-2][x] != 255) {
					tmp.data[y][x] = 255;
					continue;
				}*/

				int sum = 0;
				for (int dy = -multSize; dy <= multSize; dy+=(multSize/4)) {
					for (int dx = -multSize; dx <= multSize; dx+=(multSize/4)) {
						// If we take out the conditional and restrict the rows and cols iterated over in the outer loops, framerate goes way up.
						// It's probably a good tradeoff for not having shadows be calculated near image edges.
						//if(x + dx > 0 && x + dx < img.cols && y + dy > 0 && y + dy < img.rows)
							sum += img.data[y+dy][x+dx];
						//else
							//sum += img.data[y][x];
					}
				}
				tmp.data[y][x] = sum / denominator;
			}
		}
	}

	tmp.CopyTo(img);
}

// Box blur using summed area tables - constant time with respect to kernel size
// I think there's still a little more optimization to do, but it's fast enough now
// Based off http://www.openprocessing.org/sketch/2934
void fastBoxBlur(GrayImage &img, int size) {
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

// Erode a hand depth-stencil image (not general purpose). Kernel size is (size*2 + 1).
void erode(GrayImage &img, int size = 2) {
	static GrayImage tmp;
	img.CopyTo(tmp);

	// Two-dimensional kernel
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			//if (img.data[y][x] != 255 && img.data[y][x+2] != 255 && img.data[y][x-2] != 255 && img.data[y+2][x] != 255 && img.data[y-2][x] != 255) {
			//	tmp.data[y][x] = 255;
			//	continue;
			//}

			int min = 255;
			for (int dy = -size; dy <= size; dy += 1) {
				for (int dx = -size; dx <= size; dx += 1) {
					if (img.data[y+dy][x+dx] < min)
						min = img.data[y+dy][x+dx];
				}
			}
			tmp.data[y][x] = min;
		}
	}

	tmp.CopyTo(img);
}

// Dilate a binary image. Kernel size is (size*2 + 1).
void dilate(BinaryImage &img, int size = 4) {
	static BinaryImage tmp;
	img.CopyTo(tmp);

	// Two-dimensional kernel
	for (int y = size; y < img.rows - size; y++) {
		for (int x = size; x < img.cols - size; x++) {

			for (int dy = -size; dy <= size; dy += 1) {
				for (int dx = -size; dx <= size; dx += 1) {
					if (img.data[y+dy][x+dx]) {
						tmp.data[y][x] = true;
						goto donePixel;
					}
				}
			}
			donePixel:;
		}
	}

	tmp.CopyTo(img);
}


/**********************
 * Effect application *
 *********************/

void applyVizEffects(KinectData &data, ColorImage &colorImage) {
	static BinaryImage handStencil;
	static BinaryImage multiHandStencil;
	handStencil.rows = data.colorImage.rows;
	handStencil.cols = data.colorImage.cols;
	multiHandStencil.rows = data.colorImage.rows;
	multiHandStencil.cols = data.colorImage.cols;
	memset(&handStencil.data, 0, sizeof(handStencil.data));
	memset(&multiHandStencil.data, 0, sizeof(multiHandStencil.data));

	// Find stencils for all arms
	for (int i = 0; i < data.hands.size(); i++) {
		data.hands[i].CreateArmBlob(handStencil, true);
		binaryImageOr(multiHandStencil, handStencil);
	}
	
	// Make image of only arms for remote
	/*for (int y = 0; y < data.colorImage.rows; y++) {
		for (int x = 0; x < data.colorImage.cols; x++) {
			if (!multiHandStencil.data[y][x]) {
				data.colorImage.data[y][x] = ColorPixel::Black;
				data.depthImage.data[y][x] = 0;
			}
		}
	}*/

	/*
	if (isHost)
		table->net.sendDataToAll(data);
	if (!isHost) {
		auto remoteDatas = table->net.popAllDataMessages();
		if (remoteDatas.size() > 0) {
			std::cout << "!";
			remoteDatas.front()->CopyTo(remoteData);
		} else {
			std::cout << ".";
		}
		remoteData.CopyTo(data);
	}
	*/

	//Export images to file system
	//const int framesPerSecond = 32;
	//TakeSnapshot(framesPerSecond * 5, "C:\\Users\\Mike\\Desktop", data.colorImage, data.depthImage, data.testImage);

	memcpy(colorImage.data, data.colorImage.data, sizeof(colorImage.data));
	colorImage.rows = data.colorImage.rows;
	colorImage.cols = data.colorImage.cols;

	// Find stencils for all arms
	memset(&handStencil.data, 0, sizeof(handStencil.data));
	memset(&multiHandStencil.data, 0, sizeof(multiHandStencil.data));
	for (int i = 0; i < data.hands.size(); i++) {
		data.hands[i].CreateArmBlob(handStencil, true);
		binaryImageOr(multiHandStencil, handStencil);
	}

	// Remove background, keeping only arms
	if (removeBackground) {
		for (int y = 0; y < colorImage.rows; y++) {
			for (int x = 0; x < colorImage.cols; x++) {
				if (!multiHandStencil.data[y][x])
					colorImage.data[y][x] = ColorPixel::White;
			}
		}
	}

	// Set background for effects after a few frames
	static int frameCount = 0;
	if (frameCount == 3)
		setBackgroundImage(colorImage);
	frameCount++;

	// Add effects

	// Find time elapased since last update
	unsigned long long curTime = Util::Helpers::GetSystemTime();
	static unsigned long long prevTime = curTime;
	static unsigned long long pearlCreateTimer = 0;
	int timeElapsed = curTime - prevTime;
	//std::cout << "Framerate: " << 1.0f / (timeElapsed * 0.001f) << std::endl;
	prevTime = curTime;

	if(data.hands.size() > 0) {  // TODO: maybe simplify the dataParams conditions for the effects
		// Get pixels occupied by hands
		for (int i = 0; i < data.hands.size(); i++) {
			data.hands[i].CreateArmBlob(handStencil, true);  // TODO: wasted recalculation - we do it above too for taking out background
			float handHeight = data.table.depth - data.depthImage.data[data.hands[i].palmCenter.y][data.hands[i].palmCenter.x];
			//binaryImageOr(multiHandStencil, handStencil);  // We already have this above now
			//Util::Helpers::ShowImage(multiHandStencil);

			// Tint the arm
			if (enableTint && handHeight > tintMinHeight && handHeight < tintMaxHeight) {
				applyTint(colorImage, data, handStencil, data.hands[i].id);
			}

			// Make hand transparent
			// TODO: if hands cross, both get merged in stencil. Not good.
			if (enableTransparentHand && handHeight > transparentMinHeight && handHeight < transparentMaxHeight) {
				float regionAlpha = (data.table.depth + 20 - data.depthImage.data[data.hands[i].centroid.y][data.hands[i].centroid.x]) / handAlphaScale;
				regionAlpha = (regionAlpha < 0.1f) ? 0.1f : regionAlpha;
				regionAlpha = (regionAlpha > 1.0f) ? 1.0f : regionAlpha;
				makeRegionTransparent(colorImage, handStencil, regionAlpha);
			}

			// Draw shadows
			if (enableShadow && handHeight > shadowMinHeight && handHeight < shadowMaxHeight)
				addShadows(colorImage, handStencil, data.depthImage, data.table.depth, (int)handHeight);
		}

		// TODO: currently bases height layer on first hand only for many effects - fix somehow
		float handHeight = data.table.depth - data.depthImage.data[data.hands[0].palmCenter.y][data.hands[0].palmCenter.x];

		// Motion blur
		if (enableMotionBlur)
			addMotionBlur(colorImage, multiHandStencil, handHeight > motionBlurMinHeight && handHeight < motionBlurMaxHeight);

		// Rainbow arm
		if (enableRainbowArm && handHeight > rainbowMinHeight && handHeight < rainbowMaxHeight) {
			drawRainbowArm(colorImage, data, multiHandStencil);
		}

		// Create finger pearls
		if (enableFingerPearls && handHeight > fingerPearlMinHeight && handHeight < fingerPearlMaxHeight) {
			if (fingerPearlMode != PEARLMODE_LINES) {
				// If not lines, draw every so often
				pearlCreateTimer += timeElapsed;
				if (pearlCreateTimer > fingerPearlInterval) {
					createFingerPearl(colorImage, data, fingerPearlMode, timeElapsed);
					//pearlCreateTimer -= timeElapsed;
					pearlCreateTimer -= fingerPearlInterval;
				}
			} else {
				// If lines only, draw every frame
				createFingerPearl(colorImage, data, fingerPearlMode, timeElapsed);
			}
		}

		// Finger pointer circle/ellipse
		if (data.hands.size() > 0 && data.hands[0].fingerTips.size() > 0 && enableFingerPointer && handHeight > fingerPointerMinHeight && handHeight < fingerPointerMaxHeight) {
			drawFingerPointer(colorImage, data, multiHandStencil);
		}
		
		// Check whether tattoos are in their layer, and set global for View stuff to read
		tattooWithinLayer = (handHeight > tattooMinHeight && handHeight < tattooMaxHeight);
		
		// Update tattoos
		updateTattoos(data, enableTransparentHand && handHeight > transparentMinHeight && handHeight < transparentMaxHeight);

		// Draw skeleton
		if (enableSkeleton)
			drawSkeleton(colorImage, data);

		// Draw arm outline
		if (enableArmOutline)
			drawArmOutline(colorImage, multiHandStencil);
	}

	if (data.hands.size() == 0)  // prevent tattoo from showing after hand leaves area (never gets updated in loop above)
		tattooWithinLayer = false;

	// Update and draw finger pearls
	updateFingerPearls(timeElapsed);
	if (enableFingerPearls) {
		drawFingerPearls(colorImage, data, multiHandStencil, fingerPearlMode);
	}
}


/***************
 * Arm tinting *
 ***************/

void applyTint(ColorImage &img, KinectData &data, BinaryImage &stencil, int id) {
	if (id < 0) {
		std::cerr << "Attempting to apply tint to invalid hand id (probably due to arm base not being detected)." << std::endl;
		return;
	}

	const float alpha = tintAlpha.size() > id ? tintAlpha[id] : 0.5f;
	const ColorPixel color = tintAlpha.size() > id ? tintColors[id] : ColorPixel(255, 0, 0);

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (stencil.data[y][x])
				blendAndSetPixel(img, x, y, alpha, color);
		}
	}
}


/******************
 * Finger pointer *
 ******************/

// Draw circle under pointing finger
void drawFingerPointer(ColorImage &img, KinectData &data, BinaryImage &stencil) {
	// Find fingertips closest to the table
	std::vector<Point3Di> locations;
	findClosestFingertip(data, locations);

	for(std::vector<Point3Di>::iterator it = locations.begin(); it != locations.end(); it++) {
		// Skip hands without fingertips
		if (it->x == -1 && it->y == -1 && it->z == -1)
			continue;

		// Pick a pointer size and alpha based on proximity to table
		float fingerPointerSize = it->z / 7.0f;
		if (fingerPointerSize < minPointerSize)
			fingerPointerSize = minPointerSize;
		float fingerPointerAlpha = 1.0f - ((fingerPointerSize - minPointerSize) / alphaScale);
		if (fingerPointerAlpha < 0.25f)
			fingerPointerAlpha = 0.25f;

		// Draw finger pointer ellipse/circle
		for (int i = -5; i <= 5; i++) {
			// TODO: tweak to make nicer maybe
			float alphaFade = ((float)(abs(i))) + 1;
			//alphaFade = alphaFade < 1.0f ? 1.0f : alphaFade;
			drawCircleOnImage(img, data, stencil, it->x, it->y, (unsigned int)fingerPointerSize+i, fingerPointerAlpha / alphaFade);
		}
	}
}

/*****************
 * Finger pearls *
 *****************/

// Low pass filter for oneEuro filter
/*float lowPassFilter(float x, float alpha) { //!!
	static float firstTime = true;
	static float prev = x;
	
	float hatx = (alpha * x) + ((1.0f - alpha) * prev);
	prev = hatx;
	return hatx;
}*/

// Low pass filter for One Euro filter
Point2Df lowPassFilter(float x, float y, float alpha) {
	static float prevX = x;
	static float prevY = y;
	
	float hatx = (alpha * x) + ((1.0f - alpha) * prevX);
	float haty = (alpha * y) + ((1.0f - alpha) * prevY);
	prevX = hatx;
	prevY = haty;
	return Point2Df(hatx, haty);
}

// Derivative low pass filter for One Euro filter
Point2Df lowPassFilterDeriv(float x, float y, float alpha) {
	static float prevX = x;
	static float prevY = y;
	
	const float hatx = (alpha * x) + ((1.0f - alpha) * prevX);
	const float haty = (alpha * y) + ((1.0f - alpha) * prevY);
	prevX = hatx;
	prevY = haty;
	return Point2Df(hatx, haty);
}

// Calculates alpha value for filters
float filtAlpha(float rate, float cutoff) {
	const float tau = 1.0f / (2.0f * PI * cutoff);
	const float te = 1.0f / rate;
	return 1.0f / (1.0f + (tau/te));
}

// One Euro filter, based on algorithm from paper by Gery Casiez, Nicolas Roussel, and Daniel Vogel (CHI 2012)
Point2Df oneEuroFilter(float x, float y, float rate, float minCutoff, float slope, float dCutoff) {
	static bool firstTime = true;
	static float prevX = x;
	static float prevY = y;
	float dx, dy;

	if (firstTime) {
		firstTime = false;
		dx = 0;
		dy = 0;
	} else {
		dx = (x - prevX) * rate;
		dy = (y - prevY) * rate;
	}

	const Point2Df ed = lowPassFilterDeriv(dx, dy, filtAlpha(rate, dCutoff));
	const float cutoffX = minCutoff + (slope * std::abs(ed.x));
	const float cutoffY = minCutoff + (slope * std::abs(ed.y));
	const Point2Df ret = lowPassFilter(x, y, filtAlpha(rate, cutoffX));
	prevX = ret.x;
	prevY = ret.y;
	return ret;
}

// Add a new finger pearl at fingertip locations
void createFingerPearl(ColorImage &img, KinectData &data, PearlMode mode, int elapsedTime) {
	// Each element of locations vector is a different hand
	std::vector<Point3Di> locations;
	findClosestFingertip(data, locations);

	// Expand to fit right number if hands (this currently never shrinks)
	while (locations.size() > pearls.size()) {
		pearls.push_back(std::vector<Point3Di>());
	}
	
	// For each hand, make a pearl at its fingertip
	//static float skippedLast = false;
	for (int i = 0; i < locations.size(); i++) {
		if (locations[i].x == -1)  // No fingertips for this hand
			continue;
		if (pearls[i].size() > 0) {
			float dist = std::sqrtf(std::powf(std::abs(locations[i].x - pearls[i].back().x), 2.0f) + std::powf(std::abs(locations[i].y - pearls[i].back().y), 2.0f));  // distance between points
			if (mode == PEARLMODE_LINES && dist > 140.0f /*&& !skippedLast*/) { // if distance between this pearl and last is too great, it probably glitched out, so ignore it
				//skippedLast = true;
				continue;
			}
		}

		// Filter new point through One Euro filter and add it
		if (mode == PEARLMODE_LINES) {
			// Inputs to the filter
			const float rate = 1.0f / (elapsedTime * 0.001f);
			const float minCutoff = 0.9f;
			const float slope = 0.005f;
			const float dCutoff = 0.9f;
			const Point2Df filt = oneEuroFilter(locations[i].x, locations[i].y, rate, minCutoff, slope, dCutoff);
			pearls[i].push_back(Point3Di(filt.x, filt.y, 0));
		} else
			pearls[i].push_back(Point3Di(locations[i].x, locations[i].y, 0));
		//skippedLast = false;
	}
}

// Age finger pearls and remove ones past their lifetime
void updateFingerPearls(int timeElapsed) {
	for (int i = 0; i < pearls.size(); i++) {  // hands
		std::vector<Point3Di>::iterator it = pearls[i].begin();
		while(it != pearls[i].end()) {
			it->z += timeElapsed;

			// Remove old pearls
			if(it->z > pearlMaxAge)
				it = pearls[i].erase(it);
			else
				it++;
		}
	}
}

// Draws finger pearls and lines connecting them, if desired.
// If doing lines only, we create pearls on every frame rather than every certain amount of time.
// Due to this, points will be closer together, so we change the randomness of the line endpoints to scale not as much with distance.
// TODO: maybe give each hand a different color
void drawFingerPearls(ColorImage &img, KinectData &data, BinaryImage &stencil, PearlMode mode) {
	// Draw the pearls (4x4 square)
	if (mode == PEARLMODE_BOTH || mode == PEARLMODE_POINTS) {
		for(std::vector< std::vector<Point3Di> >::const_iterator hand = pearls.begin(); hand < pearls.end(); hand++) {
			for(std::vector<Point3Di>::const_iterator it = hand->begin(); it < hand->end(); it++) {
				for (int dy = -2; dy <= 2; dy++)
					for (int dx = -2; dx <= 2; dx++)
						if (!stencil.data[it->y + dy][it->x + dx])
							blendAndSetPixel(img, it->x + dx, it->y + dy, 1.0f - (it->z / (float)pearlMaxAge));
			}
		}
	}

	// Draw the trails connecting them
	if (mode == PEARLMODE_BOTH || mode == PEARLMODE_LINES) {
		for(std::vector< std::vector<Point3Di> >::const_iterator hand = pearls.begin(); hand < pearls.end(); hand++) {
			if (hand->size() < 2)
				return;
			for (int i = 0; i < hand->size() - 1; i++) {
				const Point3Di &it = (*hand)[i];
				const Point3Di &next = (*hand)[i+1];
				int randX = 1;
				int randY = 1;

				if (squigglyLines) {
					// Compute a random amount to nudge line endpoints by based on distance between them
					int dist = (int)std::sqrtf(std::powf(std::abs(next.x - it.x), 2.0f) + std::powf(std::abs(next.y - it.y), 2.0f));  // distance between points
					int distScale = mode == PEARLMODE_LINES ? 2 : 7;  // TODO: tweak these and maybe make them global constants
					dist = (dist / distScale < 1) ? 1 : (dist / distScale);  // prevent division by 0
					int randX = std::rand() % dist;
					int randY = std::rand() % dist;
				}

				// Draw the lines
				lineBresenham(img, data, stencil, it.x + randX, it.y + randY, next.x + (squigglyLines?-randX:randX), next.y + (squigglyLines?-randY:randY), 1.0f - (it.z / (float)pearlMaxAge));
				lineBresenham(img, data, stencil, it.x, it.y, next.x, next.y, 1.0f - (it.z / (float)pearlMaxAge));
				lineBresenham(img, data, stencil, it.x - randX, it.y - randY, next.x + (squigglyLines?-randX:randX), next.y - (squigglyLines?-randY:randY), 1.0f - (it.z / (float)pearlMaxAge));
			}
		}
	}
}

/***************
 * Motion blur *
 ***************/

// Perform hand motion blur on image
void addMotionBlur(ColorImage &img, BinaryImage &handStencil, bool draw) {
	static ColorImage imgBuf[motionBlurBufferSize];
	static BinaryImage stencilBuf[motionBlurBufferSize];
	static int curBuffer = 0;

	// Initialize and clear buffers the first time
	if (imgBuf[0].rows == 0) {  // rely on this being 0 to detect first time
		for (int i = 0; i < motionBlurBufferSize; i++) {
			imgBuf[i].rows = img.rows;
			imgBuf[i].cols = img.cols;
			stencilBuf[i].rows = img.rows;
			stencilBuf[i].cols = img.cols;

			memset(imgBuf[i].data, 0, sizeof(imgBuf[i].data));
			memset(stencilBuf[i].data, 0, sizeof(stencilBuf[i].data));
		}
	}

	// Iterate image pixels
	if (draw) {
		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				// Leave the area where hand currently is alone
				if (handStencil.data[y][x])
					continue;

				unsigned int blendRed = 0;
				unsigned int blendGreen = 0;
				unsigned int blendBlue = 0;
				unsigned int blendSamplesUsed = 0;

				// Blend from each buffer
				for (int buf = 0; buf < motionBlurBufferSize; buf++) {
					// Include only pixels where hand was
					if (stencilBuf[buf].data[y][x]) {
						blendRed += imgBuf[buf].data[y][x].red;
						blendGreen += imgBuf[buf].data[y][x].green;
						blendBlue += imgBuf[buf].data[y][x].blue;
						blendSamplesUsed++;
						//img.data[y][x] = ColorPixel::Blue;  // uncomment to clearly show which pixels are stenciled
					}
				}
			
				// If any blending to be done, average the current pixel and the old ones
				// If equal blur weight is set, then current frame contribution will be only has high as any previous frame.
				// If not set, then current frame makes up half the contribution, with the other half coming from all the other frames combined.
				if (blendSamplesUsed > 0) {
					if (!equalBlurWeight) {
						blendRed /= blendSamplesUsed;
						blendGreen /= blendSamplesUsed;
						blendBlue /= blendSamplesUsed;
					}

					img.data[y][x].red = (img.data[y][x].red + blendRed) / (equalBlurWeight ? blendSamplesUsed+1 : 2);
					img.data[y][x].green = (img.data[y][x].green + blendGreen) / (equalBlurWeight ? blendSamplesUsed+1 : 2);
					img.data[y][x].blue = (img.data[y][x].blue + blendBlue) / (equalBlurWeight ? blendSamplesUsed+1 : 2);
				}
			}
		}
	}

	// Store new frame in buffer
	// This is done after blending above so it effectively increases the number of frames blended
	img.CopyTo(imgBuf[curBuffer]);
	handStencil.CopyTo(stencilBuf[curBuffer]);
	curBuffer++;
	curBuffer = curBuffer % motionBlurBufferSize;
}

/*********************
 * Hand transparency *
 *********************/

void setBackgroundImage(ColorImage &img) {
	background.rows = img.rows;
	background.cols = img.cols;
	img.CopyTo(background);
}

// Make region of image transparent, blending either the background inital snapshot, or the last known values
void makeRegionTransparent(ColorImage &img, BinaryImage &region, float alpha) {
	static ColorImage lastKnown;
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (!region.data[y][x]) {
				lastKnown.data[y][x] = img.data[y][x];
			} else {
				blendAndSetPixel(img, x, y, alpha, useStaticBackground ? background.data[y][x] : lastKnown.data[y][x]);
			}
		}
	}
}

/************************
 * Layering rainbow arm *
 ************************/

// Convert HSV to RGB color value
// Based on http://wiki.beyondunreal.com/HSV-RGB_Conversion
static ColorPixel HSVToRGB(const Point3Df HSV) {
	float Min, Chroma, Hdash, X;
	Point3Df RGB;
 
	Chroma = HSV.y * HSV.z;
	Hdash = HSV.x / 60.0f;
	float modFactor = fmod(Hdash, 2.0f);
	X = Chroma * (1.0f - abs(modFactor - 1.0f));
 
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
 
	Min = HSV.z - Chroma;
	RGB.x += Min;
	RGB.y += Min;
	RGB.z += Min;

	RGB.x *= 255;
	RGB.y *= 255;
	RGB.z *= 255;
 
	ColorPixel RGBi((int)RGB.x, (int)RGB.y, (int)RGB.z);
	return RGBi;
}

// Overlays a transparent rainbow-layered color blob on top of the arms
void drawRainbowArm(ColorImage &img, KinectData &data, BinaryImage &stencil) {
	static ColorPixel depthToRgb[maxDepth];

	// Build depth to RGB lookup table the first time
	if (depthToRgb[0].red == 0) {
		for (int i = 0; i < maxDepth; i++) {
			float hue = fmod((i / (float)maxDepth) * 360.0f * rainbowFrequency, 360.0f);
			depthToRgb[i] = HSVToRGB(Point3Df(hue, 1.0f, 1.0f));
		}
	}

	// Iterate image pixels
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			// If pixel unoccupied by hand, skip it
			if (!stencil.data[y][x])
				continue;

			// Blend the color with the arm
			float alpha = 0.25f;
			ColorPixel &color = depthToRgb[(int)(data.table.depth - data.depthImage.data[y][x])];
			blendAndSetPixel(img, x, y, alpha, color);
		}
	}
}

/****************
 * Shadow stuff *
 ****************/

// Draw shadows under hands
void addShadows(ColorImage &img, BinaryImage &stencil, DepthImage &depth, int tableDepth, const int meanHeight) {
	static GrayImage depthStencil;
	depthStencil.rows = img.rows;
	depthStencil.cols = img.cols;
	memset(depthStencil.data, 255, sizeof(depthStencil.data));

	// Build grayscale image of (0-255) scale depth of hand regions
	// Do iteration in reverse due to the direction of shadow offset
	for (int y = img.rows-1; y >= 0; y--) {
		for (int x = img.cols-1; x >= 0; x--) {
			int height = tableDepth - depth.data[y][x];
			height = 255.0f * (height / (float)maxDepth);  // height off table, 0-255
			int offset = height / 2;  // how far the shadow is offset - making this divided by an integer introduces artifacts

			if (x + offset >= img.cols || y + offset >= img.rows)
				continue;
			if (!stencil.data[y][x])
				continue;

			if (height < 80)
				height = 80;
			depthStencil.data[y+offset][x+offset] = height;
		}
	}

	// Blur the grayscale image
	const int blurSize = meanHeight / 10 + 1;
	erode(depthStencil, 1);  // gets rid of the noise from building the depthStencil table
	fastBoxBlur(depthStencil, blurSize);

	// Draw the blurred shadow image
	for (int y = 0; y < depthStencil.rows; y++) {
		for (int x = 0; x < depthStencil.cols; x++) {
			// Don't shadow the hand itself
			if (!stencil.data[y+2][x+2]) {  // give a little leeway in the shadow direction
			//if (!stencil.data[y][x]) {
				img.data[y][x].red *= (depthStencil.data[y][x] / 255.0f);
				img.data[y][x].green *= (depthStencil.data[y][x] / 255.0f);
				img.data[y][x].blue *= (depthStencil.data[y][x] / 255.0f);
				/*img.data[y][x].red = (depthStencil.data[y][x]);
				img.data[y][x].green = (depthStencil.data[y][x]);
				img.data[y][x].blue = (depthStencil.data[y][x]);*/
			}
		}
	}
}

/*****************
 * Hand skeleton *
 *****************/

// Overlay the detected skeleton for the arm
void drawSkeleton(ColorImage &img, KinectData &data) {
	// Need an empty stencil for line drawing - we don't want hand to cover skeleton up
	static BinaryImage stencil;
	if (stencil.rows == 0) {
		stencil.cols = 640;
		stencil.rows = 480;
		memset(stencil.data, 0, sizeof(stencil.data));
	}

	// Draw each hand's skeleton
	for (std::vector<Hand>::const_iterator it = data.hands.begin(); it < data.hands.end(); it++) {
		if (it->armBase.x == -1 || it->palmCenter.x == -1)
			continue;

		// Arm base to palm
		lineBresenham(img, data, stencil, it->armBase.x, it->armBase.y, it->palmCenter.x, it->palmCenter.y, 1.0f, ColorPixel::White);

		// Palm to fingertips
		for (std::vector<Point2Di>::const_iterator finger = it->fingerTips.begin(); finger < it->fingerTips.end(); finger++) {
			lineBresenham(img, data, stencil, it->palmCenter.x, it->palmCenter.y, finger->x, finger->y, 1.0f, ColorPixel::White);
		}
	}
}

/****************
 * Arm outlines *
 ****************/

void drawArmOutline(ColorImage &img, BinaryImage &stencil) {
	static BinaryImage expandedStencil;
	stencil.CopyTo(expandedStencil);

	dilate(expandedStencil);

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (!stencil.data[y][x] && expandedStencil.data[y][x])
				blendAndSetPixel(img, x, y, 1.0f, ColorPixel(0, 64, 123)); 
		}
	}
}

/********************
 * Tattoo functions *
 ********************/

static bool makeTattoo(std::string file) {
	// Load a tattoo image
	TextureImage tex;
	if (!loadTextureImage(file, tex)) {
		std::cerr << "Failed to load tattoo texture." << std::endl;
		return false;
	}

	// Create a tattoo for this image
	Tattoo tattoo;
	tattoo.visible = false;
	tattoo.tex = tex;
	tattoos.push_back(tattoo);
	return true;
}

bool loadTattoos() {
	if(!makeTattoo("tattoo.png"))
		return false;
	if(!makeTattoo("tattoo2.png"))
		return false;

	return true;
}

// Update tattoos based on hands
// TODO: maybe revise how to assign tattoos to arm
void updateTattoos(KinectData &data, bool doTransparency) {
	for(std::vector<Tattoo>::iterator it = tattoos.begin(); it != tattoos.end(); it++)
		it->visible = false;
	if (data.hands.size() == 0)
		return;

	//for (std::vector<Hand>::const_iterator it = data.hands.begin(); it < data.hands.end(); it++) {
	for (int i = 0; i < data.hands.size(); i++) {
		// Find direction/angle from arm centroid to palm
		float dx = data.hands[i].palmCenter.x - data.hands[i].armBase.x;
		//float dx = it->palmCenter.x - it->centroid.x;  // TODO: change to base once base works again
		float dy = data.hands[i].palmCenter.y - data.hands[i].armBase.y;
		//float dy = it->palmCenter.y - it->centroid.y;
		float angle = (atan2f(dy, dx) / PI) * 180.0f;
		
		// If there are more hands than tattoos, make another tattoo with default image
		if (i >= tattoos.size())
			makeTattoo("tattoo.png"); 

		// Assign tattoo parameters
		// TODO: figure out how to scale with depth (probably not linear 1:1)
		// TODO: The alpha is same as hand-transparency. They should probably be tied together rather than being calculated separately
		// TODO: currently reversed tattoo order as a hack to at least keep the first arm consistent
		//const unsigned int tatIndex = data.hands.size() - i - 1;
		const unsigned int tatIndex = i;
		tattoos[tatIndex].visible = true;
		tattoos[tatIndex].scale = tattooScale / data.hands[i].meanDepth;
		tattoos[tatIndex].x = data.hands[i].palmCenter.x;
		tattoos[tatIndex].y = data.hands[i].palmCenter.y;
		tattoos[tatIndex].angle = angle;
		if (doTransparency) {
			tattoos[tatIndex].alpha = getHandTransparency(data.table.depth, data.depthImage.data[data.hands[i].centroid.y][data.hands[i].centroid.x]);
		} else {
			tattoos[tatIndex].alpha = 1.0f;
		}
	}
}

const std::vector<Tattoo> getTattoos() {
	return tattoos;
}
