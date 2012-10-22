// Project
#include <iostream>
#include "View.h"
#include "Effects.h"
#include "KinectViz/VizPipeline.h"

// KinectTable
#include "Global/KinectTable_c.h"
#include "Util/Drawing.h"
#include "Util/Helpers.h"

// Effect flag toggles
extern bool enableMotionBlur;
extern bool enableFingerPointer;
extern bool enableTattoos;
extern bool enableTransparentHand;
extern bool enableFingerPearls;
extern bool enableRainbowArm;
extern bool enableShadow;
extern bool enableTint;
extern bool enableSkeleton;
extern bool removeBackground;
extern bool enableArmOutline;
extern bool enableTransparentHand;

// depth lookup table
#define MAX_DEPTH 10000
static unsigned short s_depth[MAX_DEPTH];

KinectTable::Client* client = NULL;
extern bool showDebug;

KinectViz::VizPipeline viz;


//---------------------------------------------------------------------------
// Util Functions
//---------------------------------------------------------------------------


// builds a lookup table for depth2rgb
static void raw2depth()
{
		int i;
		for ( i=0; i<MAX_DEPTH; i++)
		{
			//float dist =100/(-0.0030711016*i + 3.3309495161); //ros distance (cm)
			//float dist2 = 12.36 * tanf(i/ 2842.5 + 1.1863);//St�phane Magnenat distance (cm)
			float v = (float)i/MAX_DEPTH; //for visualization purposes only
			v = powf(v, 2);
			v = v*36*256;
			s_depth[i] = (unsigned short)v;
		}
}

void Binary2Color(const BinaryImage& binaryImage, ColorImage& colorImage)
{
	const ColorPixel blackPixel(0,0,0);
	const ColorPixel whitePixel(255,255,255);

	for (size_t y=0; y<binaryImage.rows; y++)
	{
		for(size_t x=0; x<binaryImage.cols; x++)
		{
			colorImage.data[y][x] = binaryImage.data[y][x] == true ? whitePixel : blackPixel;
		}
	}

	colorImage.rows = binaryImage.rows;
	colorImage.cols = binaryImage.cols;

	return;
}

void depth2color(const DepthImage& depthImage, ColorImage& colorImage)
{
	for (size_t y=0; y<depthImage.rows; y++)
	{
		for(size_t x=0; x<depthImage.cols; x++)
		{   
			int pval = s_depth[depthImage.data[y][x]];
			int lb = pval & 0xff;

			ColorPixel& colorPixel = colorImage.data[y][x];

			switch (pval>>8)
			{
				case 0:
						colorPixel.red = 255;
						colorPixel.green = 255-lb;
						colorPixel.blue = 255-lb;
						break;
				case 1:
						colorPixel.red = 255;
						colorPixel.green = lb;
						colorPixel.blue = 0;
						break;
				case 2:
						colorPixel.red = 255-lb;
						colorPixel.green = 255;
						colorPixel.blue = 0;
						break;
				case 3:
						colorPixel.red = 0;
						colorPixel.green = 255;
						colorPixel.blue = lb;
						break;
				case 4:
						colorPixel.red = 0;
						colorPixel.green = 255-lb;
						colorPixel.blue = 255;
						break;
				case 5:
						colorPixel.red = 0;
						colorPixel.green = 0;
						colorPixel.blue = 255-lb;
						break;
				default:
						colorPixel.red = 0;
						colorPixel.green = 0;
						colorPixel.blue = 0;
						break;
			}
		}
	}

	colorImage.rows = depthImage.rows;
	colorImage.cols = depthImage.cols;

	return;
}


void HighlightPoints(const ColorImage& input, const std::vector<Point2Di> points, const ColorPixel& color, ColorImage& output)
{
	for(int i=0; i<points.size(); i++)
	{
		const Point2Di& point = points[i];
		input.data[point.y][point.x] = color;
	}

	return;
}

void HighlightRegions(const ColorImage& colorImage, const BinaryImage& highlightImage, const ColorImage& highlightedColorImage)
{
	const ColorPixel greenPixel(0,255,0);

	for(size_t y=0; y<colorImage.rows; y++)
	{
		for(size_t x=0; x<colorImage.cols; x++)
		{
			highlightedColorImage.data[y][x] = highlightImage.data[y][x] ? greenPixel : colorImage.data[y][x];
		}
	}

	return;
}

bool CreateGraphicalImage(KinectData& data, ColorImage& testImage)
{
	// Use color image as background
	if(data.available.colorImageEnable)
	{
		// Copy over color image
		data.colorImage.CopyTo(testImage);
	}


	// Create table image
	static BinaryImage tableBlob;
	data.table.CreateTableBlob(tableBlob);
	
	
	// Draw arms on image
	if(data.available.handsEnable)
	{	
		// Draw boundaries around arms
		static const ColorPixel armColors[] = { ColorPixel(ColorPixel::Black), ColorPixel(ColorPixel::Red), ColorPixel(ColorPixel::Green), ColorPixel(ColorPixel::Blue) };
		for(int i=0; i<data.hands.size(); i++)
		{
			const std::vector<Point2Di>& boundary = data.hands[i].boundary;

			if(data.hands[i].id + 1 < sizeof(armColors) / sizeof(ColorPixel));
			{
				for(int j=0; j<boundary.size(); j++)
				{
					const Point2Di& point = boundary[j];
					testImage.data[point.y][point.x] = armColors[data.hands[i].id + 1];
				}
			}
		}
		
		
		// Draw the finger tips
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			Util::Drawing::DrawPoints(testImage, hand.fingerTips, ColorPixel(ColorPixel::Green), 3);
		}

		// Draw the finger bases
		/*for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			Util::Drawing::DrawPoints(testImage, hand.fingerBases, ColorPixel(ColorPixel::Red), 3);
		}*/

		// Draw the arm base
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.armBase.x != -1 && hand.armBase.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.armBase, ColorPixel(ColorPixel::Blue), 5);
		}

		
		// Draw the hand palm
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.palmCenter.x != -1 && hand.palmCenter.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.palmCenter, ColorPixel(255, 0, 255), 5);
		}

		// Centroid
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.centroid.x != -1 && hand.centroid.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.centroid, ColorPixel(255, 0, 0), 3);
		}
		
	}
	

	return true;
}


//---------------------------------------------------------------------------
// GlutCode
//---------------------------------------------------------------------------

void Display(void)
{
	static ColorImage grayImage;
	memset(grayImage.data, 0x80, sizeof(grayImage.data));
	grayImage.rows = grayImage.maxRows;
	grayImage.cols = grayImage.maxCols;

	// Get data maps from Kinect sensor
	static KinectData data;
	static KinectData remoteData;
	bool dataChanged = false;
	try
	{
		dataChanged = client->GetData(data);
	}
	catch(...)
	{
		fprintf(stderr, "Error: Could not get data!");
		return;
	}
	
	if(dataChanged == false)
		return;

	KinectDataParams& dataParams = data.available;
	dataParams.colorImageEnable = true;
	dataParams.depthImageEnable = true;
	dataParams.handsEnable = true;
	dataParams.testImageEnable = false;
	dataParams.validityImageEnable = false;

	//Get visual representation of the depth image
	static ColorImage visualDepthImage;
	if (showDebug)
		depth2color(data.depthImage, visualDepthImage);

	// Apply effects (new version)
	viz.updateData(data);
	viz.getEffect(KinectViz::kBackgroundRemover).enabled = true;
	viz.getEffect(KinectViz::kPointerCircle).enabled = true;
	viz.getEffect(KinectViz::kSkeleton).enabled = true;
	viz.applyEffects();

	// Get test image
	static ColorImage testImage;
	if (showDebug)
		CreateGraphicalImage(data, testImage);

	// Set the images for viewing
	View::SetTopLeftImage(viz.getImage());
	View::SetTopRightImage(visualDepthImage);
	View::SetBottomLeftImage(testImage);

	// Limit framerate
#if 0
	static const float frameLimit = 121.0f;
	static unsigned long long lastTime = Util::Helpers::GetSystemTime();
	unsigned long long curTime;
	do {
		curTime = Util::Helpers::GetSystemTime();
	} while(curTime - lastTime < (1.0f/frameLimit) * 1000);
	lastTime = curTime;
#endif
	
	return;
}


void SpecialInput(int key, int x, int y)
{
}

void Keyboard (unsigned char key, int x, int y)
{
	switch (key) {
	case '3': enableMotionBlur = !enableMotionBlur; break;
	case '1': enableFingerPointer = !enableFingerPointer; break;
	case '2': enableFingerPearls = !enableFingerPearls; break;
	case '4': enableTattoos = !enableTattoos; break;
	case '5': enableRainbowArm = !enableRainbowArm; break;
	case '6': enableShadow = !enableShadow; break;
	case '7': enableTint = !enableTint; break;
	case '0': enableSkeleton = !enableSkeleton; break;
	case '8': enableTransparentHand = !enableTransparentHand; break;
	case '9': enableArmOutline = !enableArmOutline; break;
	case 'b': removeBackground = !removeBackground; break;
	case 'd': showDebug = !showDebug; break;
		case 27:  // escape
			View::StopLoop();
			break;
		case 'f':
			View::SetFullscreen();
			break;
		case 't':
			client->RecalculateTableCorners();
			break;
		default:
			break;
	}
}

void Init()
{
	return;
}

void DeInit()
{
	// Clean up //
	return;
}

int InitLocalSession(const SessionParameters& params)
{
	try {
		client = KinectTableConnectLocalPublic(&params, "127.0.0.1");
	}
	catch(Errno err)
	{
		// Check if the session started correctly
		switch(err)
		{
			case -1: fprintf(stderr, "Error: Could not initialize Kinect sensor.\n"); return -1;
			case ENODEV: fprintf(stderr, "Error: Could not initialize Kinect sensor.\n"); return -1;
			case ENOTSUP: fprintf(stderr, "Error: Could not initialize Kinect sensor.  Session is not supported.\n"); return -1;
			default: assert(false); break;
		}
	}

	return 0;
}


//---------------------------------------------------------------------------
// Entry Point
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	//Set up the session parameters
	SessionParameters params;
	params.dataParams.colorImageEnable = true;
	params.dataParams.depthImageEnable = true;
	params.dataParams.validityImageEnable = false;
	params.dataParams.testImageEnable = showDebug;
	params.dataParams.handsEnable = true;

	// Print out menu
	int ret = InitLocalSession(params);
	if(ret != 0) {
		return -1;
	}	

	// Set up the initialization parameters for viewing
	static View::InitParams initParams;
	initParams.initFunc = Init;
	initParams.deInitFunc = DeInit;
	initParams.idleFunc = NULL;
	initParams.displayFunc = Display;
	initParams.keyboardFunc = Keyboard;
	initParams.specialInputFunc = SpecialInput;
	initParams.resizeFunc = NULL;
	
	// Initialize the viewing
	View::Init(initParams);

	// build a depth lookup table
	raw2depth();

	// Load effect texture images
	if(!loadTattoos())
		return 0;

	// Start the application
	View::StartLoop();

	return 0;
}
