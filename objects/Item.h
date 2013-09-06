#pragma once
#ifndef _ITEM_H_
#define _ITEM_H_

// LAE
#include "../graphics/AnimatedSprite.h"
#include "../graphics/PAnimation.h"
#include "../math/Rectangle.h"
// stl
#include <string>
// boost
#include <boost/unordered_map.hpp>

namespace LAE {

/// Trida herniho predmetu
class Item : public AnimatedSprite {
public:
	/// Constructor
	Item();
	virtual ~Item();

	/// Returns item active area
	Rectangle& getActiveRect() { return activeRect_; }
	/// Returns item active area
	const Rectangle& getActiveRect() const { return activeRect_; }
	/// Sets new active area for item
	void setActiveRect(const Rectangle& rect) { activeRect_ = rect; }
	
	/// Returns name of item
	const std::string& getName() const { return name_; }
	/// Sets new name of item
	void setName( const std::string& name ) { name_ = name; }

	/// Returns description text
	const std::string& getDescription() const { return descr_; }
	/// Sets new description text
	void setDescription( const std::string& descr ) { descr_ = descr; }

	/// Returs name of a cursor that will be displayed when mouse pointer is over item
	const std::string& getCursorName() const { return cursor_; }
	/// Sets new name of a cursor that will be displayed when mouse pointer is over item
	void setCursorName( const std::string&  cursor ) { cursor_ = cursor; }

	/// Enables / disables item
	void setActive( bool active ) { active_ = active; }
	/// Returns true if items is active
	bool isActive() const { return active_; }

	/// Returns function name that will be called when player examines item
	const std::string& getOnExamine() const { return onExamine_; }
	/// Sets new function name that will be called when player examines item
	void setOnExamine( const std::string& onExamine ) { onExamine_ = onExamine; }

	/// Returns function name that will be called when player uses item
	const std::string& getOnUse() const { return onUse_; }
	/// Sets new function name that will be called when player examines item
	void setOnUse( const std::string& onUse ) { onUse_ = onUse; }

private:
	Rectangle activeRect_;
	std::string name_;	///< item name
	std::string descr_;	///< item description
	std::string cursor_;	///< cursor name
	bool active_;		///< item is active
	bool visible_;		///< item is visible

	std::string onExamine_;	///< funkce skriptu pro prozkoumani predmetu
	std::string onUse_;		///< funkce skriptu pro pouziti predmetu
};

/// hash mapa: nazev predmetu -> predmet
typedef boost::unordered_map<std::string, Item> ItemHashMap;

}

#endif // _ITEM_H_
