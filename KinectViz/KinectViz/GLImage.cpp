#include <iostream>    
//#include <GL\glew.h>
#include <GL/glut.h>
//#include <GL\GLU.h>
#include "FreeImage.h"
#include "GLImage.h"

// Loads an image and stores it in an OpenGL texture
bool loadTextureImage(std::string filename, TextureImage &tex) {
	FreeImage_Initialise();
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);  // image
	BYTE* bits(0);  // image data
	unsigned int width(0), height(0), bpp(0);
	GLuint gl_texID;
	
	// Get file extension
	fif = FreeImage_GetFileType(filename.c_str(), 0);
	if(fif == FIF_UNKNOWN)
		return false;

	// Load file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename.c_str());
	if(!dib)
		return false;

	// Get data
	bits = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	bpp = FreeImage_GetBPP(dib);
	if((bits == 0) || (width == 0) || (height == 0))
		return false;

	// Generate an OpenGL texture
	glGenTextures(1, &gl_texID);
	if (gl_texID < 0 || gl_texID > 100) {
		int err = glGetError();
		std::cout << "glGenTextures failed (err " << err << "): " << gluErrorString(err) << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Set the texture data.
	// Assume BGR, since all the images I've encountered have been BGR for now.
	if (bpp == 32)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits);
	else if (bpp == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bits);
	else
		return false;

	// Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	float scale = sqrtf((width*width) + (height*height));
	tex.normWidth = width / scale;
	tex.normHeight = height / scale;
	tex.id = gl_texID;

	return true;
}
