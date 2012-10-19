#ifndef DATA_TYPES__POINTS_H
#define DATA_TYPES__POINTS_H


// Module Check //
//#ifndef MOD_DATA_TYPES
//#error "Files outside module cannot access file directly."
//#endif


//
// This holds all the image pixel types.
//


// Includes //


// Project
#include "Serialization\Serialization.h"
#include "Randomization\Randomization.h"

// Standard C++
#include <cmath>

// Standard C
#include <assert.h>



// Namespaces //




namespace DataTypes
{
	
	//!!Change int to size_t?
	typedef struct tagPoint2Di : public Serialization::ISerializable, public Randomization::IRandomizable
	{
		static int Dot(const tagPoint2Di a, const tagPoint2Di b) { return a.x * b.x + a.y * b.y; }


		tagPoint2Di()
			: x(0), y(0)
		{ return; }

		tagPoint2Di(int x, int y)
			: x(x), y(y)
		{ return; }

		tagPoint2Di(const tagPoint2Di& point)
			: x(point.x), y(point.y)
		{ return; }

		float Magnitude() const { return std::sqrt((float)(x*x + y*y)); }

		tagPoint2Di& operator+=(const tagPoint2Di& a)
		{
			x += a.x;
			y += a.y;
			return *this;
		}

		tagPoint2Di& operator-=(const tagPoint2Di& a)
		{
			x -= a.x;
			y -= a.y;
			return *this;
		}

		const tagPoint2Di operator+(const tagPoint2Di& a) const
		{
			return tagPoint2Di(*this) += a;
		}

		const tagPoint2Di operator-(const tagPoint2Di& a) const
		{
			return tagPoint2Di(*this) -= a;
		}

		tagPoint2Di operator-() const
		{
			return tagPoint2Di(-x, -y);
		}
		

		bool operator==(const tagPoint2Di& point) const
		{
			return (x == point.x) && (y == point.y);
		}

		bool operator!=(const tagPoint2Di& point) const
		{
			return !(*this == point);
		}

		void Serialize(Serialization::Serializer& serializer) const
		{
			serializer.Write(x);
			serializer.Write(y);
			return;
		}

		void Deserialize(Serialization::Deserializer& deserializer)
		{
			deserializer.Read(x);
			deserializer.Read(y);
			return;
		}


		void Randomize(Randomization::Randomizer& randomizer)
		{
			randomizer.Randomize(x);
			randomizer.Randomize(y);
			return;
		}


		int x;
		int y;

	} Point2Di;

	typedef struct tagPoint2Df
	{
		static float Dot(tagPoint2Df a, tagPoint2Df b) { return a.x * b.x + a.y * b.y; }

		tagPoint2Df()
			: x(0), y(0)
		{ return; }

		tagPoint2Df(float x, float y)
			: x(x), y(y)
		{ return; }

		tagPoint2Df(const tagPoint2Df& point)
			: x(point.x), y(point.y)
		{ return; }

		tagPoint2Df(const tagPoint2Di& point)
			: x((float)point.x), y((float)point.y)
		{ return; }
		
		float Magnitude() { return std::sqrt(x*x + y*y); }
		
		float x;
		float y;

	} Point2Df;

	typedef struct tagPoint3Df
	{
		static float Dot(tagPoint3Df a, tagPoint3Df b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
		static tagPoint3Df Cross(tagPoint3Df a, tagPoint3Df b) { return tagPoint3Df(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

		tagPoint3Df()
			: x(0), y(0), z(0)
		{ return; }

		tagPoint3Df(float x, float y, float z)
			: x(x), y(y), z(z)
		{ return; }

		tagPoint3Df(const tagPoint2Df& point, float z)
			: x(point.x), y(point.y), z(z)
		{ return; }

		tagPoint3Df(const tagPoint3Df& point)
			: x(point.x), y(point.y), z(point.z)
		{ return; }


		float Magnitude() { return std::sqrt(x*x + y*y + z*z); }

		float x;
		float y;
		float z;

	} Point3Df;

	typedef struct tagImagePoint
	{

		tagImagePoint()
			: x(0), y(0), z(0)
		{ return; }

		tagImagePoint(int x, int y, float z)
			: x(x), y(y), z(z)
		{ return; }
		
		tagImagePoint(const tagPoint2Di& point, float z)
			: x(point.x), y(point.y), z(z)
		{ return; }

		tagImagePoint(const tagImagePoint& point)
			: x(point.x), y(point.y), z(point.z)
		{ return; }

		int x;
		int y;
		float z;

	} ImagePoint;

}


#endif