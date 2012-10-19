//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_IMAGE_PROCESSING

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "HandLabeler.h"

// Module


// Project


// Standard C++


// Standard C



//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------




namespace ImageProcessing
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------

void HandLabeler::LabelHands(long milliseconds, std::vector<Hand>& hands)
{
	
	const long oldestTime = 5000;
	static std::vector<HandFeatures> unlabeledHands;

	// Clear any old features
	for(size_t i=0; i<labeledHands.size(); i++)
	{
		if(milliseconds - labeledHands[i].time > oldestTime)
			labeledHands.erase(labeledHands.begin() + i);
	}

	// Get hand features
	unlabeledHands.clear();
	for(size_t i=0; i<hands.size(); i++)
	{
		// Can't label a hand unless we have the arm base
		if(hands[i].armBase.x == -1)
		{
			hands[i].id = -1;
			continue;
		}

		static HandFeatures features;
		features.label = i; // label it as its index for now
		features.time = milliseconds;

		features.position = hands[i].armBase;
		unlabeledHands.push_back(features);
	}

	// Create graph
	static std::vector<std::vector<float>> graphEdges;
	graphEdges.clear();

	for(size_t i=0; i<unlabeledHands.size(); i++)
	{
		graphEdges.push_back(std::vector<float>());

		for(size_t j=0; j<labeledHands.size(); j++)
		{
			const HandFeatures& leftNode = unlabeledHands[i];
			const HandFeatures& rightNode = labeledHands[j];
			graphEdges[i].push_back(ComputeEdgeWeight(leftNode, rightNode));
		}
	}
	
	// Match the hands
	static std::vector<int> matches;
	MatchLabels(graphEdges, matches);
	
	// Update the labeled hands
	for(size_t i=0; i<matches.size(); i++)
	{
		int leftNode = i;
		int rightNode = matches[i];

		// Get hand index
		int handIndex = unlabeledHands[leftNode].label;

		if(rightNode == -1)  // We have a new hand
		{
			
			// Assign label to unlabeled hand
			unlabeledHands[leftNode].label = FindUnusedId();

			// Set hand id
			hands[handIndex].id = unlabeledHands[leftNode].label;

			// Add new hand to labeled hands
			labeledHands.push_back(unlabeledHands[leftNode]);
			
		}
		else  // Matched hand
		{
			// Assign label to unlabeled hand
			unlabeledHands[leftNode].label = labeledHands[rightNode].label;

			// Set hand id
			hands[handIndex].id = labeledHands[rightNode].label;

			// Update hand features
			labeledHands[rightNode] = unlabeledHands[leftNode];
		}
		 
	}
	
	
	return;
}


//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------


//!! Should return -1 if velocity between two points is too high
float HandLabeler::ComputeEdgeWeight(const HandFeatures& a, const HandFeatures& b)
{
	Point3Df displacementVector(b.position - a.position, b.time - a.time);
	return displacementVector.Magnitude();
}


void HandLabeler::MatchLabels(std::vector<std::vector<float>>& graphEdges, std::vector<int>& matches)
{

	matches.clear();
	matches.resize(graphEdges.size(), -1);

	bool noEdges = false;
	while(!noEdges)
	{
		// Find min edge and match them
		int leftNode = -1;
		int rightNode = -1;
		float minWeight = 99999;
		noEdges = true;
		for(size_t i=0; i<graphEdges.size(); i++)
		{
			const std::vector<float>& nodeEdges = graphEdges[i];

			for(size_t j=0; j<nodeEdges.size(); j++)
			{
				if(nodeEdges[j] != -1 && nodeEdges[j] < minWeight)
				{
					minWeight = nodeEdges[j];
					leftNode = i;
					rightNode = j;
					noEdges = false;
				}
			}
		}

		if(noEdges)
			continue;

		// Create match
		matches[leftNode] = rightNode;

		// Remove edges to the nodes
		std::vector<float>& leftNodeEdges = graphEdges[leftNode];
		for(size_t i=0; i<leftNodeEdges.size(); i++)
			leftNodeEdges[i] = -1;
	
		for(size_t i=0; i<graphEdges.size(); i++)
			graphEdges[i][rightNode] = -1;

	}

	return;
}




int HandLabeler::FindUnusedId() const
{
	bool isUsed = true;
	int id = -1;
	while(isUsed)
	{
		id++;

		isUsed = false;
		for(size_t i=0; i<labeledHands.size() && !isUsed; i++)
			isUsed = labeledHands[i].label == id;
	}

	return id;
}



}
