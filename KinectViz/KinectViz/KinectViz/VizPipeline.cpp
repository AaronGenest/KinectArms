#include <iostream>
#include <memory>
using namespace std;

#include <DataTypes/DataTypes.h>
#include "VizPipeline.h"
#include "Effects/PointerCircle.h"


namespace KinectViz {

VizPipeline::VizPipeline() {
	// Register default effects.
	// Order of registering must be consistent with handles in DefaultEffectHandle.
	registerEffect<PointerCircle>();
}


VizPipeline::~VizPipeline() {
}


void VizPipeline::applyEffects(ColorImage& image) {
	for(auto effect = effects.begin(); effect != effects.end(); effect++) {
		if ((*effect)->enabled)
			(*effect)->applyEffect(image);
	}
}


Effect& VizPipeline::getEffect(int handle) {
	return *effects[handle];
}


// Note: definition of VizPipeline::registerEffect() templated method is in the header file.

}
