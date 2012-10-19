//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "DataExporter.h"

// Project

// Standard
#include <exception>
#include <stdio.h>
#include <windows.h>
#include <assert.h>

// Standard C++
#include <fstream>
#include <sstream>



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------

using namespace std;
using namespace DataTypes;


namespace Util
{


//---------------------------------------------------------------------------
// Data Declarations
//---------------------------------------------------------------------------


typedef unsigned char uchar;

typedef struct
{
	uchar blue;
	uchar green;
	uchar red;

} BitmapPixel;


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------



void DataExporter::ExportColorImageToBitmap(const char* filename, const ColorImage& image)
{
	const size_t rows = image.rows;
	const size_t cols = image.cols;

	//Create image data with proper color order  (ColorPixel holds the colors in reverse order)
	static Types<BitmapPixel>::FlatImage bitmapPixels;
	for(size_t y=0; y<rows; y++)
	{
		for(size_t x=0; x<cols; x++)
		{
			int i = y*cols + x;
			bitmapPixels.data[i].green = image.data[y][x].green;
			bitmapPixels.data[i].red = image.data[y][x].red;
			bitmapPixels.data[i].blue = image.data[y][x].blue;
		}
	}

	//Get some image properties
	int bytesPerPixel = sizeof(BitmapPixel);
	int paddedColumnBytes = cols*bytesPerPixel + cols*bytesPerPixel % 4;
	int numberOfPixels = rows * cols;
	int rawImageBytes = rows * paddedColumnBytes;
	

	//Fill in the information header
	/*
	BITMAPV5HEADER infoHeader;
	infoHeader.bV5Size = sizeof(infoHeader);
	infoHeader.bV5Width = columns;
	infoHeader.bV5Height = rows;
	infoHeader.bV5Planes = 1;
	infoHeader.bV5BitCount = bytesPerPixel * 8;
	infoHeader.bV5Compression = BI_BITFIELDS;  // BI_RGB = No compression
	infoHeader.bV5SizeImage = rawImageBytes;
	infoHeader.bV5XPelsPerMeter = 0;
	infoHeader.bV5YPelsPerMeter = 0;
	infoHeader.bV5ClrUsed = 0;
	infoHeader.bV5ClrImportant = 0;
	infoHeader.bV5AlphaMask = 0;
	infoHeader.bV5RedMask = 0xFF000000;
	infoHeader.bV5GreenMask = 0x00FF0000;
	infoHeader.bV5BlueMask = 0x0000FF00;
	*/
	
	BITMAPINFOHEADER infoHeader;
	infoHeader.biSize = sizeof(infoHeader);
	infoHeader.biWidth = cols;
	infoHeader.biHeight = rows;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = bytesPerPixel * 8;
	infoHeader.biCompression = BI_RGB;  // BI_RGB = No compression
	infoHeader.biSizeImage = rawImageBytes;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;
	

	//Fill in the image header
	BITMAPFILEHEADER header;
	header.bfType = 'MB';
	header.bfSize = sizeof(header) + sizeof(infoHeader) + rawImageBytes;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(header) + sizeof(infoHeader);
	
	
	//Open file
	FILE* file = fopen(filename, "wb");
	if(file == NULL)
		throw errno;


	// Write bitmap to file //
	int ret;

	//Write bitmap header
	ret = fwrite(&header, sizeof(header), 1, file);
	if(ret != 1)
		throw errno;

	// Write information header
	ret = fwrite(&infoHeader, sizeof(infoHeader), 1, file);
	if(ret != 1)
		throw errno;

	// Write pixels
	char padding[] = { 0, 0, 0, 0 };
	int paddingBytes = paddedColumnBytes - cols*bytesPerPixel;
	for(int y=rows-1; y>=0; y--)
	{
		//Write row
		ret = fwrite(&bitmapPixels.data[y*cols], bytesPerPixel, cols, file);
		if(ret != cols)
			throw errno;

		//Write padding
		if(paddingBytes > 0)
		{
			ret = fwrite(padding, 1, paddingBytes, file);
			if(ret != paddingBytes)
				throw errno;
		}

	}
	
	//Close file
	(void)fclose(file);

	return;
}




void DataExporter::ExportDepthImageToCsv(const char* filename, const DepthImage& depthImage)
{
	
	//Open file
	FILE* file = fopen(filename, "w");
	if(file == NULL)
		throw errno;

	//Write data
	for(size_t y=0; y<depthImage.rows; y++)
	{
		fprintf(file, "%hu", depthImage.data[y][0]);

		for(size_t x=1; x<depthImage.cols; x++)
		{
			fprintf(file, ",%hu", depthImage.data[y][x]);
		}

		fprintf(file, "\n");
	}


	//Close file
	(void)fclose(file);

	return;
}


void DataExporter::ExportBinaryImageToCsv(const char* filename, const BinaryImage& binaryImage)
{
	static DepthImage depthImage;

	//Convert binary image to doubles
	for(size_t y=0; y<binaryImage.rows; y++)
	{
		for(size_t x=0; x<binaryImage.cols; x++)
		{
			depthImage.data[y][x] = binaryImage.data[y][x] ? 1 : 0;
		}
	}

	depthImage.rows = binaryImage.rows;
	depthImage.cols = binaryImage.cols;

	//Export like depth image
	ExportDepthImageToCsv(filename, depthImage);

	return;
}


void DataExporter::ExportDataToCsv(const char* filename, const std::vector<Point2Di>& data)
{
	//Open file
	FILE* file = fopen(filename, "w");
	if(file == NULL)
		throw errno;

	//Write data
	std::vector<Point2Di>::const_iterator iter;
	for(iter = data.begin(); iter < data.end(); iter++)
	{
		const Point2Di& point = *iter;
		assert(sizeof(point.x) == sizeof(unsigned int));
		assert(sizeof(point.y) == sizeof(unsigned int));
		fprintf(file, "%u, %u\n", point.x, point.y);
	}
	
	//Close file
	(void)fclose(file);

	return;
}


void DataExporter::ExportDataToCsv(const char* filename, const std::vector<float>& data)
{
	//Open file
	FILE* file = fopen(filename, "w");
	if(file == NULL)
		throw errno;

	//Write data
	std::vector<float>::const_iterator iter;
	for(iter = data.begin(); iter < data.end(); iter++)
	{
		fprintf(file, "%f\n", *iter);
	}
	
	//Close file
	(void)fclose(file);

	return;
}



void DataExporter::ImportCsvToDepthImage(const char* filename, DepthImage& depthImage)
{
	// Open file for reading
	ifstream file(filename);
	if(file.fail())
		throw -1;

	bool firstLine = true;
	int colNum = 0;
	int row = 0;
	int col = 0;
	string line;
	while(getline(file, line))
	{
		stringstream ss(line);
		string valueString;
		while(std::getline(ss, valueString, ','))
		{
			unsigned value;
			int ret = sscanf(valueString.c_str(), "%hu", &value);
			if(ret == 0)
				throw -1;

			depthImage.data[row][col] = value;
			col++;
		}

		// Set the number of columns in the depth image
		if(firstLine)
		{
			colNum = col;
			firstLine = false;
		}

		// Make sure each row has the same number of columns
		if(col != colNum)
			throw -1;

		row++;
		col = 0;
	}
	
	// Set the image size
	depthImage.rows = row;
	depthImage.cols = colNum;

	return;
}

}
