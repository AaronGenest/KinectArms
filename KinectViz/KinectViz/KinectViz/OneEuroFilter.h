#pragma once

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


// One Euro filter, based on algorithm from paper by Gery Casiez, Nicolas Roussel, and Daniel Vogel (CHI 2012)
class OneEuroFilter {
public:
	OneEuroFilter(float minCutoff, float minDerivCutoff, float cutoffSlope);
	~OneEuroFilter() { }

	float filter(float x, float timeElapsed);

	float minCutoffFreq;
	float derivCutoffFreq;
	float cutoffSlope;


private:
	float alpha(float rate, float cutoff);

	bool firstTime;
	LowPassFilter xFilter;
	LowPassFilter dxFilter;
};
