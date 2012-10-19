#include <DataTypes/DataTypes.h>

#include "../Effect.h"


class PointerCircle : public Effect {
public:
	PointerCircle() { }
	~PointerCircle() { }

	void applyEffect(ColorImage& image);
};