#pragma once
#include <vector>

#include <DataTypes/DataTypes.h>
#include "Effect.h"


enum DefaultEffectHandle {
	kPointerCircle = 0
};


class VizPipeline {
public:
	VizPipeline();
	~VizPipeline();

	void applyEffects(ColorImage& image);
	int registerEffect(Effect& effect);
	Effect& getEffect(int handle);


private:
	//std::vector< std::shared_ptr<Effect> > effects;
	std::vector<Effect*> effects;
};
