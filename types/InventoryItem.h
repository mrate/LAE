#pragma once
#ifndef _INVENTORYITEM_H_
#define _INVENTORYITEM_H_

// LAE
#include "../graphics/PAnimation.h"
// stl
#include <map>
#include <string>

namespace LAE {

struct SInventoryItem {
	std::string name;
	std::string description;
	PAnimation animation;
	std::string cursorName;
	std::string onExamine;
	std::string onUse;
};

typedef std::map<std::string, SInventoryItem> TInventoryItemsMap;
//typedef std::vector<std::string> TInventoryItemVector;

}

#endif // _INVENTORYITEM_H_
