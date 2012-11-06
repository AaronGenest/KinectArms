#include <cmath>
#include "OneEuroFilter.h"


OneEuroFilter::OneEuroFilter(float minCutoffFreq, float derivCutoffFreq, float cutoffSlope) :
	minCutoffFreq(minCutoffFreq),
	derivCutoffFreq(derivCutoffFreq),
	cutoffSlope(cutoffSlope)
{
}

float OneEuroFilter::filter(float x, float timeElapsed) {
	float rate = 1.0f / (timeElapsed / 1000.0f);

	float dx;
	if (firstTime) {
		firstTime = false;
		dx = 0;
	} else {
		dx = (x - xFilter.getLast()) * rate;
	}

	const float edx = dxFilter.filter(dx, alpha(rate, derivCutoffFreq));
	const float cutoff = minCutoffFreq + (cutoffSlope * std::abs(edx));

	return xFilter.filter(x, alpha(rate, cutoff));
}

float OneEuroFilter::alpha(float rate, float cutoff) {
	float tau = 1.0f / (2 * cutoff);
	float te = 1.0f / rate;
	return 1.0f / (1.0f + (tau / te));
}


float LowPassFilter::filter(float x, float alpha) {
	float hatX;
	if (firstTime) {
		hatX = x;
		firstTime = false;
	} else {
		hatX = (alpha * x) + ((1 - alpha) * prevHatX);
	}

	prevHatX = hatX;
	return hatX;
}

