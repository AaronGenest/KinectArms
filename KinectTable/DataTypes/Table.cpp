//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_DATA_TYPES

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Table.h"

// Module


// Project
#include "Util/Helpers.h"
#include "Util/Drawing.h"


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace DataTypes
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


Table& Table::operator=(const Table& table)
{
	if(this == &table)
		return *this;
			
	corners = table.corners;
	depth = table.depth;

	return *this;
}


void Table::CreateTableBlob(BinaryImage& tableBlob, bool fillBlob) const
{
	if(fillBlob)
	{
		Util::Helpers::GetBlobFromCorners(corners, tableBlob);
	}
	else
	{
		memset(tableBlob.data, 0, sizeof(tableBlob.data));
		for(size_t i=0; i<corners.size(); i++)
			Util::Drawing::DrawLine(tableBlob, corners[i], corners[(i + 1) % corners.size()], 1);

		tableBlob.rows = tableBlob.maxRows;
		tableBlob.cols = tableBlob.maxCols;
	}

	return;
}


bool Table::operator==(const Table& table) const
{
	bool isEqual = true;

	isEqual &= (corners == table.corners);
	isEqual &= (depth == table.depth);

	return isEqual;
}

bool Table::operator!=(const Table& table) const
{
	return !(*this == table);
}


void Table::Serialize(Serialization::Serializer& serializer) const
{
			
	// Get corners
	serializer.WriteVector(corners);
			
	// Get depth
	serializer.Write(depth);
			
	return;
}
		
void Table::Deserialize(Serialization::Deserializer& deserializer)
{
	// Set corners
	deserializer.ReadVector(corners);
			
	// Set depth
	deserializer.Read(depth);
			
	return;
}

//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------





}
