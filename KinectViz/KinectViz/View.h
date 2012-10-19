#ifndef VIEW_H
#define VIEW_H


//
// These functions handle all the viewing of the data for the example.
//


// Includes //
#include "Global/KinectTable_c.h"


// Namespaces //


namespace View
{
	
	typedef void (*InitFunc)();
	typedef void (*DeInitFunc)();
	typedef void (*KeyboardFunc)(unsigned char key, int x, int y);
	typedef void (*SpecialInputFunc)(int key, int x, int y);
	typedef void (*DisplayFunc)(void);
	typedef void (*IdleFunc)(void);
	typedef void (*ResizeFunc)(int w, int h);


	typedef struct tagInitParams
	{
		InitFunc initFunc;
		DeInitFunc deInitFunc;
		KeyboardFunc keyboardFunc;
		SpecialInputFunc specialInputFunc;
		DisplayFunc displayFunc;
		IdleFunc idleFunc;
		ResizeFunc resizeFunc;
		
	} InitParams;


	// Methods //

	// Initializes the view.
	int Init(const InitParams& initParams);

	// Starts the drawing loop
	void StartLoop();

	// Stops the drawing loop
	void StopLoop();

	
	// Sets the window to fullscreen
	void SetFullscreen();


	//  Sets the image to be drawn
	void SetTopLeftImage(const ColorImage& image);

	//  Sets the image to be drawn
	void SetTopRightImage(const ColorImage& image);

	//  Sets the image to be drawn
	void SetBottomLeftImage(const ColorImage& image);

	
	// Toggles the paint effect
	void TogglePaint();

	// Toggles the ripples effect
	void ToggleRipples();

	// Toggles the motion blur effect
	void ToggleMotionBlur();

	// Toggles the depth filters
	void ToggleDepthFilter();

	// Update finger pointer
	void setFingerPointer(int x, int y, int size, int alpha);
}


#endif