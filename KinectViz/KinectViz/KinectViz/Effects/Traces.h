#pragma once
#include <DataTypes/DataTypes.h>
#include "../Effect.h"


namespace KinectViz {

class Traces : public Effect {
public:
	Traces();
	~Traces() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed);

	int maxTraceAge;
	ColorPixel color;


private:
	void addTraces(KinectData& kinectData);
	void updateAndPruneTraces(int dt);
	void drawTraces(ColorImage& image);

	std::vector< std::vector<Point3i> > tracesPerHand;
};

}