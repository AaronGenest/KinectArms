#pragma once

#include "DataTypes\Other.h"

#include "KinectDataParams.h"

using namespace System;

namespace KinectTableNet
{
	/// <summary>
	/// Specifies parameters for the type of connection to make.
	/// </summary>
	public ref struct SessionParameters
	{

		public:

			SessionParameters()
			{
				this->dataParams = gcnew KinectDataParams();
				return;
			}

			SessionParameters(KinectDataParams::EnableType enableType)
			{
				this->dataParams = gcnew KinectDataParams(enableType);
				return;
			}

			SessionParameters(KinectDataParams^ dataParams)
			{
				this->dataParams = gcnew KinectDataParams(dataParams);
				return;
			}

			/// <summary>
			/// Specifies which data the sensor should make available.
			/// </summary>
			property KinectDataParams^ DataParams
			{
				KinectDataParams^ get() { return dataParams; }
			}

		internal:

			void CopyTo(DataTypes::SessionParameters& sessionParams)
			{
				this->dataParams->CopyTo(sessionParams.dataParams);
				return;
			}

		private:

			KinectDataParams^ dataParams;
	};
}
