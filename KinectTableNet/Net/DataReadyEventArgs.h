#pragma once

// Unmanaged //
#include "DataTypes\KinectData.h"

// Managed
#include "SessionParameters.h"
#include "Client.h"


using namespace System;


namespace KinectTableNet
{

	
	public ref class DataReadyEventArgs sealed
	{

	  public:

		/// <summary>
		/// Gets the data.  Does not reallocate any memory that the KinectData structure has already allocated.
		/// </summary>
		void GetData([Out] KinectData^% data)
		{
			if(!validData)
				throw "Data is not valid outside event handler.";

			if(data == nullptr)
				data = GetData();
			else
				data->CopyFrom(this->data);
			return;
		}
		
		/// <summary>
		/// Gets the data.  All memory is newly allocated.
		/// </summary>
		KinectData^ GetData()
		{
			if(!validData)
				throw "Data is not valid outside event handler.";

			return gcnew KinectData(data);
		}
	
	  internal:

		DataReadyEventArgs(DataTypes::KinectData& data) : data(data), validData(true)
		{ return; }

		void SetDataInvalid() { validData = false; }

	  private:

		DataTypes::KinectData& data;
		bool validData;

	};

}
