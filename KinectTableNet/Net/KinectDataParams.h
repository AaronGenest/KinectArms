#pragma once

#include "DataTypes\KinectDataParams.h"

using namespace System;

namespace KinectTableNet
{
	
	/// <summary>
	/// Specifies which data the sensor should make available.
	/// </summary>
	public ref struct KinectDataParams
	{

	  public:

		enum class EnableType
		{
			All,
			None,
			AllButImages,
			ImagesOnly
		};

		
		/// <summary>
		/// Creates a new KinectDataParams structure with everything false.
		/// </summary>
		KinectDataParams()
		{
			InitFromUnmanagedDataParams(DataTypes::KinectDataParams());
			return;
		}

		KinectDataParams(EnableType enableType)
		{
			DataTypes::KinectDataParams::EnableType enableTypeU = (DataTypes::KinectDataParams::EnableType)(int)enableType;
			InitFromUnmanagedDataParams(DataTypes::KinectDataParams(enableTypeU));
			return;
		}

		KinectDataParams(KinectDataParams^ dataParams)
		{
			colorImageEnable = dataParams->colorImageEnable;
			depthImageEnable = dataParams->depthImageEnable;
			validityImageEnable = dataParams->validityImageEnable;
			testImageEnable = dataParams->testImageEnable;

			tableEnable = dataParams->tableEnable;
			handsEnable = dataParams->handsEnable;
			return;
		}

		

		
		// Images //

		/// <summary>
		/// Specifies if color data is enabled.
		/// </summary>
		bool colorImageEnable;

		/// <summary>
		/// Specifies if depth data is enabled.
		/// </summary>
		bool depthImageEnable;

		/// <summary>
		/// Specifies if the validity information from the depth data is enabled.
		/// </summary>
		bool validityImageEnable;

		/// <summary>
		/// Specifies if the test image is enabled.
		/// </summary>
		bool testImageEnable;


		// Data //

		/// <summary>
		/// Specifies if the table information is enabled.
		/// </summary>
		bool tableEnable;

		/// <summary>
		/// Specifies if the hand information is enabled.
		/// </summary>
		bool handsEnable;

	  internal:

		KinectDataParams(const DataTypes::KinectDataParams& params)
		{
			InitFromUnmanagedDataParams(params);
			return;
		}

		void CopyTo(DataTypes::KinectDataParams& params)
		{
			params.colorImageEnable = colorImageEnable;
			params.depthImageEnable = depthImageEnable;
			params.validityImageEnable = validityImageEnable;
			params.testImageEnable = testImageEnable;

			params.tableEnable = tableEnable;
			params.handsEnable = handsEnable;

			return;
		}


	  private:

		void InitFromUnmanagedDataParams(const DataTypes::KinectDataParams& params)
		{
			colorImageEnable = params.colorImageEnable;
			depthImageEnable = params.depthImageEnable;
			validityImageEnable = params.validityImageEnable;
			testImageEnable = params.testImageEnable;

			tableEnable = params.tableEnable;
			handsEnable = params.handsEnable;

			return;
		}


	};

}
