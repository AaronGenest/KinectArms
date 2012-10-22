#include <DataTypes/DataTypes.h>

#include "../Effect.h"


namespace KinectViz {

class PointerCircle : public Effect {
public:
	PointerCircle() { }
	~PointerCircle() { }

	void applyEffect(ColorImage& image, KinectData& kinectData, BinaryImage& handsMask);
};

}