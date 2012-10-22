#pragma once
#include <vector>

#include <DataTypes/DataTypes.h>
#include "Effect.h"


namespace KinectViz {

enum DefaultEffectHandle {
	kPointerCircle = 0
};


class VizPipeline {
public:
	VizPipeline();
	~VizPipeline();

	void applyEffects(ColorImage& image);
	Effect& getEffect(int handle);

	template <class T> int registerEffect();


private:
	std::vector< std::unique_ptr<Effect> > effects;
};


template <class T>
int VizPipeline::registerEffect() {
	effects.push_back(unique_ptr<Effect>(new T()));
	return effects.size();
}

}