#pragma once

// Support class for the OneEuroFilter
class LowPassFilter {
public:
	LowPassFilter() : firstTime(true) { }
	~LowPassFilter() { }

	float getLast() { return prevHatX; }
	float filter(float x, float alpha);


private:
	bool firstTime;
	float prevHatX;
};


/*
 * One Euro filter, based on algorithm from paper by Gery Casiez, Nicolas Roussel, and Daniel Vogel (CHI 2012).
 * Three parameters must be set and tuned based on your application:
 * - minCutoffFreq: minimum cutoff frequency in Hz
 * - derivCutoffFreq: minimum cutoff frequency in Hz for the value derivative
 * - cutoffSlope: the slope of the cutoff line (also refered to as beta in the paper)
 * Reasonable starting parameters are 1.0 for minCutoffFreq and derivCutoffFreq, and 0.1 for cutoffSlope
 * if your values range up to around 100-1000. If your values are lower than this, decrease cutoffSlope.
 */
class OneEuroFilter {
public:
	OneEuroFilter(float minCutoffFreq, float derivCutoffFreq, float cutoffSlope);
	~OneEuroFilter() { }

	// Filters a new data point (x) and returns the filtered value. Expects timeElapsed to be time in milliseconds since last call to filter().
	float filter(float x, float timeElapsed);

	// Tweakable parameters
	float minCutoffFreq;
	float derivCutoffFreq;
	float cutoffSlope;


private:
	float alpha(float rate, float cutoff);

	bool firstTime;
	LowPassFilter xFilter;
	LowPassFilter dxFilter;
};
