#pragma once

// Unmanaged //
#include "DataTypes\Table.h"

// Managed //
#include "ColorImage.h"
#include "DepthImage.h"
#include "BinaryImage.h"

// Namespaces //
using namespace System;
using namespace System::Drawing;

namespace KinectTableNet
{
	
	/// <summary>
	/// Holds information about the table in the image.
	/// </summary>
	public ref class Table sealed
	{
	  
	  public:

		/// <summary>
		/// The corners of the table.
		/// </summary>
		property array<Point>^ Corners
		{
			array<Point>^ get() { return corners; }
		}

		/// <summary>
		/// The approximate height of the table.
		/// </summary>
		property unsigned short Depth
		{
			unsigned short get() { return depth; }
		}

		/// <summary>
		/// Creates a binary image of the area of the table.  Edges are shown in white.
		/// </summary>
		ImageFrame^ CreateTableBlob()
		{
			return CreateTableBlob(true);
		}

		/// <summary>
		/// Creates a binary image of the area of the table.  The area is shown in white.
		/// </summary>
		/// <param name="fillBlob">Whether or not to fill the region.</param>
		ImageFrame^ CreateTableBlob(bool fillBlob)
		{
			DataTypes::BinaryImage& tableBlob = *binaryImageU;

			tableU->CreateTableBlob(tableBlob, fillBlob);
			return gcnew BinaryImage(tableBlob);
		}


		void CopyTo(Table^ table)
		{
			corners.CopyTo(table->corners, 0);
			table->depth = depth;

			table->tableU = tableU;
			binaryImageU->CopyTo(*(table->binaryImageU));

			return;
		}

	  internal:

		Table(const DataTypes::Table& table) : tableU(new DataTypes::Table()), binaryImageU(new DataTypes::BinaryImage())
		{

			// Save table structure
			DataTypes::Table& thisTable = *(this->tableU);
			//thisTable = table;
			thisTable.corners.clear();
			thisTable.corners = table.corners;
			thisTable.depth = table.depth;

			// Get corners
			corners = gcnew array<Point>(table.corners.size());
			for(int i=0; i<table.corners.size(); i++)
			{
				const DataTypes::Point2Di& point = table.corners[i];
				corners[i] = Point(point.x, point.y);
			}

			// Get depth
			depth = table.depth;

			return;
		}

		~Table()
		{
			if(tableU != NULL)
			{
				delete tableU;
				tableU = NULL;
			}

			if(binaryImageU != NULL)
			{
				delete binaryImageU;
				binaryImageU = NULL;
			}

			return;
		}

	  private:

		array<Point>^ corners;
		unsigned short depth;
		DataTypes::Table* tableU;
		DataTypes::BinaryImage* binaryImageU;

	};

}
