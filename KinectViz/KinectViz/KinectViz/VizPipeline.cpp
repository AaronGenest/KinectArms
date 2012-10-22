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


namespace KinectViz {

VizPipeline::VizPipeline() :
	kinectData(nullptr)
{
	// Register default effects.
	// Order of registering must be consistent with handles in DefaultEffectHandle.
	registerEffect<BackgroundRemover>();
	registerEffect<PointerCircle>();
	registerEffect<Skeleton>();
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
	for(auto effect = effects.begin(); effect != effects.end(); effect++) {
		if ((*effect)->enabled)
			(*effect)->applyEffect(image, *kinectData, handsMask);
	}
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
	memset(&handsMask.data, 0, sizeof(handsMask.data));

	// Make a composite mask from single hand blobs
	for (auto hand = kinectData->hands.begin(); hand != kinectData->hands.end(); hand++) {
		hand->CreateArmBlob(singleMask, true);
		for (int y = 0; y < handsMask.rows; y++) {
			for (int x = 0; x < handsMask.cols; x++) {
				handsMask.data[y][x] = handsMask.data[y][x] || singleMask.data[y][x];
			}
		}
	}
}


// Note: definition of VizPipeline::registerEffect() templated method is in the header file.

}
