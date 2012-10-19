#ifndef IMAGE_PROCESSING__HAND_LABELER_H
#define IMAGE_PROCESSING__HAND_LABELER_H


// Module Check //
#ifndef MOD_IMAGE_PROCESSING
#error "Files outside module cannot access file directly."
#endif



//
// This is a template.
//


// Includes //

// Module


// Project
#include "DataTypes/DataTypes.h"

// Standard C++
#include <vector>

// Standard C




// Namespaces //




namespace ImageProcessing
{
	
	class HandLabeler
	{

	  // PUBLIC //
	public:

		void ClearHistory() { labeledHands.clear(); }

		void LabelHands(long time, std::vector<Hand>& hands);

	  // PRIVATE //
	  private:

		
		struct HandFeatures
		{
			int label;
			long time;
			Point2Di position;
		};
		

		static float ComputeEdgeWeight(const HandFeatures& a, const HandFeatures& b);
		static void MatchLabels(std::vector<std::vector<float>>& graphEdges, std::vector<int>& matches);

		int FindUnusedId() const;
		


		std::vector<HandFeatures> labeledHands;

	};

}


#endif