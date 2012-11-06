#include <iostream>
#include <DataTypes\DataTypes.h>
#include "Traces.h"
#include "Util.h"


namespace KinectViz {

Traces::Traces() :
	maxTraceAge(3000),
	color(220, 150, 0)
{
}

void Traces::applyEffect(ColorImage& image, KinectData& kinectData, const GrayImage& handsMask, int timeElapsed) {
	// Add new trace points
	addTraces(kinectData, timeElapsed);
	updateAndPruneTraces(timeElapsed);
	drawTraces(image);
}

void Traces::addTraces(KinectData& kinectData, int timeElapsed) {
	for (auto hand = kinectData.hands.cbegin(); hand != kinectData.hands.cend(); hand++) {
		// If hand isn't detected properly, don't add a trace.
		// This is especially important since we can't index -1 on tracesPerHand.
		if (hand->id == -1)
			continue;

		// Skip this hand if it's not in effect layer
		if (!handWithinLayer(kinectData, *hand))
			continue;

		// Find fingertip id closest to table and skip this hand if no fingertips are found
		int closestFinger = findClosestFingertip(kinectData, *hand);
		if (closestFinger == -1)
			continue;

		// Expand the per-hand buffers if needed
		while (hand->id >= tracesPerHand.size()) {
			tracesPerHand.push_back(std::vector<Point3i>());
			xFilterPerHand.push_back(OneEuroFilter(1.0f, 1.0f, 0.1f));
			yFilterPerHand.push_back(OneEuroFilter(1.0f, 1.0f, 0.1f));
		}

		// TODO: if trace is far from last position, reset the history

		// Filter the new point and add a trace
		const Point2Di tip = hand->fingerTips[closestFinger];
		const float filteredX = xFilterPerHand[hand->id].filter(tip.x, (float)timeElapsed);
		const float filteredY = yFilterPerHand[hand->id].filter(tip.y, (float)timeElapsed);
		const Point3i tipWithTime(filteredX, filteredY, 0);
		tracesPerHand[hand->id].push_back(tipWithTime);
	}
}

void Traces::updateAndPruneTraces(int dt) {
	for (auto traceHand = tracesPerHand.begin(); traceHand != tracesPerHand.end(); traceHand++) {
		auto trace = traceHand->begin();
		while (trace != traceHand->end()) {
			// Age the trace
			trace->z += dt;

			// Remove old ones
			if (trace->z > maxTraceAge)
				trace = traceHand->erase(trace);
			else
				trace++;
		}
	}
}

void Traces::drawTraces(ColorImage& image) {
	// Iterate through hands
	for (auto traceHand = tracesPerHand.begin(); traceHand != tracesPerHand.end(); traceHand++) {
		// Need at least two points to connect
		if (traceHand->size() < 2)
			continue;

		// Iterate traces for this hand
		for (auto trace = traceHand->cbegin(); trace + 1 != traceHand->cend(); trace++) {
			auto nextTrace = trace + 1;

			// Draw a line from trace to trace, and two slightly offset for visual effect
			const float alpha = 1.0f - (trace->z / (float)maxTraceAge);
			//drawLine(image, trace->x+1, trace->y+1, nextTrace->x+1, nextTrace->y+1, color, alpha);
			drawLine(image, trace->x+1, trace->y+1, nextTrace->x-1, nextTrace->y-1, color, alpha);
			drawLine(image, trace->x, trace->y, nextTrace->x, nextTrace->y, color, alpha);
			//drawLine(image, trace->x-1, trace->y-1, nextTrace->x-1, nextTrace->y-1, color, alpha);
			drawLine(image, trace->x-1, trace->y-1, nextTrace->x+1, nextTrace->y+1, color, alpha);
		}
	}
}

}
