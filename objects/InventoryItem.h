#pragma once
#include "../graphics/AnimatedSprite.h"

#include <string>

namespace LAE {

class InventoryItem : public AnimatedSprite {
public:
	InventoryItem();
	virtual ~InventoryItem();

	std::string name;
};

typedef std::vector<InventoryItem> InventoryItems;

}
