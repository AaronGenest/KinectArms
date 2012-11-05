/*******************************************************************************
 * NOTE:
 * A rewrite of this old effects system is in the KinectViz directory.
 * It supports all the old effects except for tattoos which remain here for now.
 ******************************************************************************/

#include "View.h"
#include "Effects.h"
#include "GLImage.h"


bool enableTattoos = false;

static float tattooScale = 50000.0f;
std::vector<Tattoo> tattoos;


void setTattoosParams(bool enabled) {
	enableTattoos = enabled;
}

/**********************
 * Effect application *
 *********************/

// This was stripped of all the effects except tattoos. Use the new effects system for the rest.
void applyVizEffects(KinectData &data, ColorImage &colorImage) {
	memcpy(colorImage.data, data.colorImage.data, sizeof(colorImage.data));
	colorImage.rows = data.colorImage.rows;
	colorImage.cols = data.colorImage.cols;
		
	// Update tattoos
	updateTattoos(data, false);
}


/********************
 * Tattoo functions *
 ********************/

static bool makeTattoo(std::string file) {
	// Load a tattoo image
	TextureImage tex;
	if (!loadTextureImage(file, tex)) {
		return false;
	}

	// Create a tattoo for this image
	Tattoo tattoo;
	tattoo.visible = false;
	tattoo.tex = tex;
	tattoos.push_back(tattoo);
	return true;
}

bool loadTattoos() {
	if(!makeTattoo("tattoo.png"))
		return false;
	if(!makeTattoo("tattoo2.png"))
		return false;

	return true;
}

// Update tattoos based on hands
// TODO: maybe revise how to assign tattoos to arm
void updateTattoos(KinectData &data, bool doTransparency) {
	for(std::vector<Tattoo>::iterator it = tattoos.begin(); it != tattoos.end(); it++)
		it->visible = false;
	if (data.hands.size() == 0)
		return;

	//for (std::vector<Hand>::const_iterator it = data.hands.begin(); it < data.hands.end(); it++) {
	for (int i = 0; i < data.hands.size(); i++) {
		// Find direction/angle from arm centroid to palm
		float dx = data.hands[i].palmCenter.x - data.hands[i].armBase.x;
		//float dx = it->palmCenter.x - it->centroid.x;  // TODO: change to base once base works again
		float dy = data.hands[i].palmCenter.y - data.hands[i].armBase.y;
		//float dy = it->palmCenter.y - it->centroid.y;
		float angle = (atan2f(dy, dx) / 3.141592653589) * 180.0f;
		
		// If there are more hands than tattoos, make another tattoo with default image
		if (i >= tattoos.size())
			makeTattoo("tattoo.png"); 

		// Assign tattoo parameters
		// TODO: figure out how to scale with depth (probably not linear 1:1)
		// TODO: The alpha is same as hand-transparency. They should probably be tied together rather than being calculated separately
		// TODO: currently reversed tattoo order as a hack to at least keep the first arm consistent
		//const unsigned int tatIndex = data.hands.size() - i - 1;
		const unsigned int tatIndex = i;
		tattoos[tatIndex].visible = true;
		tattoos[tatIndex].scale = tattooScale / data.hands[i].meanDepth;
		tattoos[tatIndex].x = data.hands[i].palmCenter.x;
		tattoos[tatIndex].y = data.hands[i].palmCenter.y;
		tattoos[tatIndex].angle = angle;
		tattoos[tatIndex].alpha = 1.0f;
	}
}

const std::vector<Tattoo> getTattoos() {
	return tattoos;
}
