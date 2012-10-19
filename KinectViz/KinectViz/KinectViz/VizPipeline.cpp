#include <iostream>
#include <memory>
using namespace std;

#include <DataTypes/DataTypes.h>
#include "VizPipeline.h"
#include "Effects/PointerCircle.h"


VizPipeline::VizPipeline() {
	// Create default effects
	//shared_ptr<PointerCircle> pointerCircle(new PointerCircle());
	auto pointerCircle = new PointerCircle();

	// Register default effects.
	// Order of registering must be consistent with handles in DefaultEffectHandle.
	//registerEffect((Effect&)*pointerCircle);
	registerEffect(*pointerCircle);
}


VizPipeline::~VizPipeline() {
}


void VizPipeline::applyEffects(ColorImage& image) {
	for(auto effect = effects.begin(); effect != effects.end(); effect++) {
		if ((*effect)->enabled)
			(*effect)->applyEffect(image);
	}
}


int VizPipeline::registerEffect(Effect& effect) {
	//effects.push_back(shared_ptr<Effect>(&effect));
	effects.push_back(&effect);
	return effects.size();
}


Effect& VizPipeline::getEffect(int handle) {
	return *effects[handle];
}
