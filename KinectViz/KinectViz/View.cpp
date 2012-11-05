
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------


// Header
#include "View.h"

//Project
#include "Effects.h"
#include "Logger.h"

// KinectTable
#include "KinectTable/KinectTable.h"

// OpenNI
#include <XnOS.h>

// OpenGL
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL\glew.h>

	#include <GL/glut.h>
	#include <GL\GLU.h>

#endif


// Standard C++
#include <exception>
#include <math.h>
#include <vector>
#include <queue>

// Standard C
#include <Windows.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

bool showDebug = true;

extern bool enableTattoos;


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

namespace View
{

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

#define GL_WIN_SIZE_X 1280
#define GL_WIN_SIZE_Y 1000
//const bool noDrawArms = !showDebug;
	const bool noDrawArms = false;

// texture buffers
#define MAIN_TEXTURE_BANK GL_TEXTURE0
GLuint g_mainTextureId = 0;

// Enable/Disable fullscreen
bool g_enableFullscreen = false;

// GLSL bind vars
GLint g_shaderProgram = -1;
GLint g_textureSamplerHandle = -1;
GLint g_fadeFactorHandle = -1;
GLint g_tintHandle = -1;

template <typename T>
struct Rect
{
	T x;
	T y;
	T width;
	T height;

	Rect():
		x ( 0 ), y ( 0 ), width ( 0 ), height ( 0 )
	{}

	Rect(T x, T y, T w, T h):
		x ( x ), y ( y ), width ( w ), height ( h )
	{}
};


// viewport info
#define NUM_VIEW_PORTS 4
Rect<int> g_viewPorts[NUM_VIEW_PORTS];
unsigned int g_currentViewPortIndex = 0;

// input params
unsigned short g_depthCeil = 1590;
unsigned short g_depthFloor = 600;


InitParams initParams;
ColorImage topLeftImage;
ColorImage topRightImage;
ColorImage botLeftImage;


//---------------------------------------------------------------------------
// Private Method Declarations
//---------------------------------------------------------------------------


static void DeInit();
static void Idle();
static void Display();
static void Resize(int w, int h);
static void checkGlError(const char* op);
static char* dumpTextFile(const char* szFilename);
static GLuint loadShader(GLenum shaderType, const char* pSource);
static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
static void setShaders();
static void setViewport(unsigned int viewPortIndex);
static void blitTexture(GLuint textureBank, GLuint textureId, unsigned char* texData, long imgFormat, long imgType);
static void drawQuad();
static void drawTattoo(Tattoo tat);
static void drawImage(GLImage tat);


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



int Init(const InitParams& initParams_)
{
	
	initParams = initParams_;
	
	int argc = 0;
	char* argv[1];

	// GLUT Init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("KinectTable");
	//glutFullScreen();
	//glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(initParams.keyboardFunc);
	glutSpecialFunc(initParams.specialInputFunc);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Resize);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);


	// glew init
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Init texture gl side
	glGenTextures(1, &g_mainTextureId);
	glActiveTexture(MAIN_TEXTURE_BANK);
	glBindTexture(GL_TEXTURE_2D, g_mainTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, COLOR_RES_X, COLOR_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// init shaders
	setShaders();

	return 0;
}

void StopLoop()
{
	// Deinitialize
	DeInit();

	//Exit the OpenGL drawing thread
	exit(0);
	return;
}

void StartLoop()
{
	// Allow user application to initialize
	if(initParams.initFunc != NULL)
		initParams.initFunc();

	// Per frame code is in glutDisplay
	glutMainLoop();
	return;
}

void SetFullscreen()
{
	g_enableFullscreen = !g_enableFullscreen;
	if (g_enableFullscreen)
	{
		glutFullScreen();
	}
	else
	{
		glutPositionWindow(20,20);
		glutReshapeWindow(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	}
	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

void DeInit()
{
	// Allow user application to deinitialize
	if(initParams.deInitFunc != NULL)
		initParams.deInitFunc();

	return;
}

static void Idle()
{
	if(initParams.idleFunc != NULL)
		initParams.idleFunc();

	// Display the frame
	glutPostRedisplay();
	return;
}

static void Display()
{

	if(initParams.displayFunc != NULL)
		initParams.displayFunc();

	// opengl clear buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(showDebug) {
		if (!g_enableFullscreen)
		{
			// DRAW RGB
			setViewport(0);
			blitTexture(MAIN_TEXTURE_BANK, g_mainTextureId, (unsigned char*)topLeftImage.data, GL_RGB, GL_UNSIGNED_BYTE);
			drawQuad();

			// DRAW DEPTH
			setViewport(1);
			blitTexture(MAIN_TEXTURE_BANK, g_mainTextureId, (unsigned char*)topRightImage.data, GL_RGB, GL_UNSIGNED_BYTE);
			drawQuad();

			// VIDEO ARM
			setViewport(2);
			blitTexture(MAIN_TEXTURE_BANK, g_mainTextureId, (unsigned char*)botLeftImage.data, GL_RGB, GL_UNSIGNED_BYTE);
			drawQuad();

		}

		if (g_enableFullscreen)
			setViewport(0);
		else
			setViewport(3);
	} else {
		setViewport(0);
	}

	if (!showDebug || g_enableFullscreen) {
		// Main viewport
		if (!noDrawArms) {
			blitTexture(MAIN_TEXTURE_BANK, g_mainTextureId, (unsigned char*)topLeftImage.data, GL_RGB, GL_UNSIGNED_BYTE);
			drawQuad();
		}

		// Draw tattoos
		if (enableTattoos) {
			glActiveTexture(GL_TEXTURE0);
			std::vector<Tattoo> tats = getTattoos();
			for(std::vector<Tattoo>::iterator it = tats.begin(); it != tats.end(); it++) {
				drawTattoo(*it);
			}
		}
	} else {
		blitTexture(MAIN_TEXTURE_BANK, g_mainTextureId, (unsigned char*)topLeftImage.data, GL_RGB, GL_UNSIGNED_BYTE);
		drawQuad();
	}

	// Swap the OpenGL display buffers
	glutSwapBuffers();

	return;
}


static void Resize(int w, int h)
{
	if(initParams.resizeFunc != NULL)
		initParams.resizeFunc(w, h);


	if (g_enableFullscreen || !showDebug)
	{
		g_viewPorts[0].x = 0;
		g_viewPorts[0].y = 0;
		g_viewPorts[0].width = min(w, h*(4.0f/3.0f));
		g_viewPorts[0].height = h;
	}
	else
	{
		g_viewPorts[0].x = 0;
		g_viewPorts[0].y = h / 2;
		g_viewPorts[0].width = w / 2;
		g_viewPorts[0].height = h / 2;

		g_viewPorts[1].x = w / 2;
		g_viewPorts[1].y = h / 2;
		g_viewPorts[1].width = w / 2;
		g_viewPorts[1].height = h / 2;

		g_viewPorts[2].x = 0;
		g_viewPorts[2].y = 0;
		g_viewPorts[2].width = w / 2;
		g_viewPorts[2].height = h / 2;

		g_viewPorts[3].x = w / 2;
		g_viewPorts[3].y = 0;
		g_viewPorts[3].width = w / 2;
		g_viewPorts[3].height = h / 2;
	}

	return;
}



void SetTopLeftImage(const ColorImage& image)
{
	memcpy(topLeftImage.data, image.data, sizeof(topLeftImage.data));
	topLeftImage.rows = image.rows;
	topLeftImage.cols = image.cols;
	return;
}


void SetTopRightImage(const ColorImage& image)
{
	memcpy(topRightImage.data, image.data, sizeof(topRightImage.data));
	topRightImage.rows = image.rows;
	topRightImage.cols = image.cols;
	return;
}


void SetBottomLeftImage(const ColorImage& image)
{
	memcpy(botLeftImage.data, image.data, sizeof(botLeftImage.data));
	botLeftImage.rows = image.rows;
	botLeftImage.cols = image.cols;
	return;
}

static void checkGlError(const char* op)
{
	for (GLint error = glGetError(); error; error = glGetError())
	{
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}


static char* dumpTextFile(const char* szFilename)
{
	char* tmpBuf = NULL;
	int len = 0;
	FILE* file = fopen(szFilename, "rb");
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	tmpBuf = (char*)malloc(sizeof(char) * (len + 1));
	fread(tmpBuf, sizeof(char), len, file);
	tmpBuf[len] = '\0';

	return tmpBuf;
}


static GLuint loadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader)
	{
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen)
			{
				char* buf = (char*) malloc(infoLen);
				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}


static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader)
	{
		return 0;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader)
	{
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program)
	{
		glAttachShader(program, vertexShader);
		//checkGlError("glAttachShader");
		glAttachShader(program, pixelShader);
		//checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength)
			{
				char* buf = (char*) malloc(bufLength);
				if (buf)
				{
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}

	return program;
}


static void setShaders()
{
	char* vs = dumpTextFile("Shaders/stock.vert");
	char* fs = dumpTextFile("Shaders/stock.frag");

	g_shaderProgram = createProgram(vs, fs);

	free(vs);
	free(fs);

	glUseProgram(g_shaderProgram);
	checkGlError("glUseProgram(\"g_shaderProgram\"");

	// fetch glsl params	
	g_textureSamplerHandle = glGetUniformLocation(g_shaderProgram, "Tex");
	checkGlError("glGetUniformLocation(\"Tex\"");
	//LOGI("glGetUniformLocation(\"Tex\") = %d\n", g_textureSamplerHandle);

	// Fade factor
	g_fadeFactorHandle = glGetUniformLocation(g_shaderProgram, "fadeFactor");
	checkGlError("glGetUniformLocation(\"fadeFactor\")");

	// Tint
	g_tintHandle = glGetUniformLocation(g_shaderProgram, "tint");
	checkGlError("glGetUniformLocation(\"tint\")");
}



static void setViewport(unsigned int viewPortIndex)
{
	// Setup the OpenGL viewpoint
	Rect<int> rect = g_viewPorts[viewPortIndex];
	glViewport(rect.x, rect.y, rect.width, rect.height);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glOrtho(0, rect.width, rect.height, 0, -1.0, 1.0);

	g_currentViewPortIndex = viewPortIndex;
}


static void blitTexture(GLuint textureBank, GLuint textureId, unsigned char* texData, long imgFormat, long imgType)
{
	// Create the OpenGL texture map
	glActiveTexture(textureBank);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, COLOR_RES_X, COLOR_RES_Y, imgFormat, imgType, texData);
}

static void drawQuad()
{
	Rect<int> rect = g_viewPorts[g_currentViewPortIndex];
	glUniform1f(g_fadeFactorHandle, 1.0f);
	glUniform3f(g_tintHandle, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);

	int nXRes = DEPTH_RES_X;
	int nYRes = DEPTH_RES_Y;

	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);

	// upper right
	glTexCoord2f((float)nXRes/(float)COLOR_RES_X, 0);
	glVertex2f((float)rect.width, 0);

	// bottom right
	glTexCoord2f((float)nXRes/(float)COLOR_RES_X, (float)nYRes/(float)COLOR_RES_Y);
	glVertex2f((float)rect.width, (float)rect.height);

	// bottom left
	glTexCoord2f(0, (float)nYRes/(float)COLOR_RES_Y);
	glVertex2f(0, (float)rect.height);

	glEnd();
}

static void drawTattoo(Tattoo tat) {
	if(!tat.visible)
		return;
	
	glBindTexture(GL_TEXTURE_2D, tat.tex.id);
	glUniform1f(g_fadeFactorHandle, tat.alpha);
	glUniform3f(g_tintHandle, tat.tint.red, tat.tint.green, tat.tint.blue);

	Rect<int> rect = g_viewPorts[g_currentViewPortIndex];
	float xScale = rect.width / (float)COLOR_RES_X;
	float yScale = rect.height / (float)COLOR_RES_Y;

	// Transform tattoo
	glPushMatrix();
	glTranslatef((float)tat.x * xScale, (float)tat.y * yScale, 0.0f);
	glRotatef(tat.angle, 0.0f, 0.0f, 1.0f);
	glScalef(tat.scale * xScale, tat.scale * yScale, 1.0f);
	
	glBegin(GL_QUADS);
	// Texture Y coords are upside-down because FreeImage reads image in upside down
	// upper left
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-tat.tex.normWidth/2.0f, -tat.tex.normHeight/2.0f);

	// upper right
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(tat.tex.normWidth/2.0f, -tat.tex.normHeight/2.0f);

	// bottom right
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(tat.tex.normWidth/2.0f, tat.tex.normHeight/2.0f);

	// bottom left
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-tat.tex.normWidth/2.0f, tat.tex.normHeight/2.0f);

	glEnd();
	glPopMatrix();
	glUniform1f(g_fadeFactorHandle, 1.0f);
}

// Draw an overlay image
static void drawImage(GLImage img) {
	if(!img.visible)
		return;
	glBindTexture(GL_TEXTURE_2D, img.tex.id);
	glUniform1f(g_fadeFactorHandle, img.alpha);
	glUniform3f(g_tintHandle, img.tint.red, img.tint.green, img.tint.blue);

	// Transform tattoo
	glPushMatrix();
	glTranslatef(img.x, img.y, 0.0f);
	glRotatef(img.angle, 0.0f, 0.0f, 1.0f);
	glScalef(img.scale, img.scale, 1.0f);
	
	glBegin(GL_QUADS);
	// Texture Y coords are upside-down because FreeImage reads image in upside down
	// upper left
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-img.tex.normWidth/2.0f, -img.tex.normHeight/2.0f);

	// upper right
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(img.tex.normWidth/2.0f, -img.tex.normHeight/2.0f);

	// bottom right
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(img.tex.normWidth/2.0f, img.tex.normHeight/2.0f);

	// bottom left
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-img.tex.normWidth/2.0f, img.tex.normHeight/2.0f);
	
	glEnd();
	glPopMatrix();
	glUniform1f(g_fadeFactorHandle, 1.0f);
}


}