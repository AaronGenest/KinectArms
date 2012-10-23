#pragma once
#include <vector>

#include <DataTypes/DataTypes.h>
#include "Effect.h"

#include "Effects/PointerCircle.h"
#include "Effects/BackgroundRemover.h"
#include "Effects/Skeleton.h"
#include "Effects/Rainbow.h"
#include "Effects/Outline.h"
#include "Effects/Tint.h"


namespace KinectViz {

enum DefaultEffectHandle {
	kBackgroundRemover,
	kPointerCircle,
	kRainbow,
	kTint,
	kSkeleton,
	kOutline
};


class VizPipeline {
public:
	VizPipeline();
	~VizPipeline();

	void updateData(KinectData& data);
	void applyEffects();
	Effect& getEffect(int handle);
	template <class T> int registerEffect();

	const ColorImage& getImage() const;


private:
	void createHandsMask();


	std::vector< std::unique_ptr<Effect> > effects;

	KinectData* kinectData;
	ColorImage image;
	GrayImage handsMask;
};


template <class T>
int VizPipeline::registerEffect() {
	effects.push_back(unique_ptr<Effect>(new T()));
	return effects.size();
}

}