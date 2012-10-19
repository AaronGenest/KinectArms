#ifndef DATA_EXPORTER_H
#define DATA_EXPORTER_H

//
// These functions export data to the file system.
//


// Includes //

//Project
#include "DataTypes/DataTypes.h"

//Standard
#include <vector>

// Namespaces //



namespace Util
{

	class __declspec(dllexport) DataExporter
	{

	  public:

		// PUBLIC //

		// Exporting //

		// Exports a color image to a bitmap file on the file system.
		static void ExportColorImageToBitmap(const char* filename, const DataTypes::ColorImage& image);

		// Exports a depth image to a comma-separated value file.
		static void ExportDepthImageToCsv(const char* filename, const DataTypes::DepthImage& depthImage);

		// Exports a binary image to a comma-separated value file.
		static void ExportBinaryImageToCsv(const char* filename, const DataTypes::BinaryImage& binaryImage);

		// Exports a 1D array of points to a comma-separated value file.
		static void ExportDataToCsv(const char* filename, const std::vector<DataTypes::Point2Di>& data);
		static void ExportDataToCsv(const char* filename, const std::vector<float>& data);


		// Importing //

		// Imports a depth image from a comma-separated value file.
		static void ImportCsvToDepthImage(const char* filename, DataTypes::DepthImage& depthImage);

	};
}

#endif