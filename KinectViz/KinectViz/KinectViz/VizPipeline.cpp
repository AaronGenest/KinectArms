#include <iostream>
#include <memory>
#include <stdexcept>
using namespace std;

#include <DataTypes/DataTypes.h>
#include <Util/Helpers.h>
#include "VizPipeline.h"

#include "Effects/PointerCircle.h"
#include "Effects/BackgroundRemover.h"
#include "Effects/Skeleton.h"
#include "Effects/Rainbow.h"
#include "Effects/Outline.h"
#include "Effects/Tint.h"
#include "Effects/MotionBlur.h"
#include "Effects/Traces.h"
#include "Effects/Shadow.h"
#include "Effects/Transparency.h"
#include "Effects/Tattoo.h"


namespace KinectViz {

VizPipeline::VizPipeline() :
	kinectData(nullptr),
	lastTime(Util::Helpers::GetSystemTime())
{
	// Register default effects.
	// Order of registering must be consistent with handles in DefaultEffectHandle.
	registerEffect<PointerCircle>();
	registerEffect<Rainbow>();
	registerEffect<Tint>();
	registerEffect<Skeleton>();
	registerEffect<Outline>();
	registerEffect<MotionBlur>();
	registerEffect<Traces>();
	registerEffect<Shadow>();
	registerEffect<Transparency>();
	registerEffect<Tattoo>();
	registerEffect<BackgroundRemover>();
}


VizPipeline::~VizPipeline() {
}


void VizPipeline::updateData(KinectData& data) {
	// Update Kinect data
	kinectData = &data;

	// Copy the color image
	memcpy(image.data, data.colorImage.data, sizeof(data.colorImage.data));
	image.rows = data.colorImage.rows;
	image.cols = data.colorImage.cols;

	// Calculate new hands mask
	createHandsMask();
}


void VizPipeline::applyEffects() {
	auto curTime = Util::Helpers::GetSystemTime();
	auto timeElapsed = (int)(curTime - lastTime);

#if 0
	cout << "FPS: " << 1000.0 / timeElapsed << endl;
#endif

	// Apply all enabled effects, in order that they're in the vector
	//for (int i = 0; i < 100; i++) {  // TEMP: for increasing the processing time so it can be measured
	for(auto effect = effects.begin(); effect != effects.end(); effect++) {
		if ((*effect)->enabled)
			(*effect)->applyEffect(image, *kinectData, handsMask, timeElapsed);
	}
	//}

	lastTime = curTime;
}


Effect& VizPipeline::getEffect(int handle) {
	if (handle >= effects.size())
		throw invalid_argument("Invalid effect handle");
	return *effects[handle];
}


const ColorImage& VizPipeline::getImage() const {
	return image;
}


void VizPipeline::createHandsMask() {
	// Mask for a single hand
	BinaryImage singleMask;
	singleMask.rows = kinectData->depthImage.rows;
	singleMask.cols = kinectData->depthImage.cols;
	memset(&singleMask.data, 0, sizeof(singleMask));

	// Mask for all hands
	handsMask.rows = kinectData->depthImage.rows;
	handsMask.cols = kinectData->depthImage.cols;
	memset(&handsMask.data, kMaskUnoccupied, sizeof(handsMask.data));

	// Make a composite mask from single hand blobs where a non-zero pixel indicates hand id occupying it
	for (auto hand = kinectData->hands.begin(); hand != kinectData->hands.end(); hand++) {
		hand->CreateArmBlob(singleMask, true);
		for (int y = 0; y < handsMask.rows; y++) {
			for (int x = 0; x < handsMask.cols; x++) {
				if (handsMask.data[y][x] == kMaskUnoccupied && singleMask.data[y][x])
					handsMask.data[y][x] = hand->id;
			}
		}
	}
}


// Note: definition of VizPipeline::registerEffect() templated method is in the header file.

}
