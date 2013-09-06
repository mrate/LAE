#pragma once
#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "../objects/InventoryItem.h"

// stl
#include <string>
#include <map>

namespace LAE {

class GameState {
public:
	static GameState* i();

	bool getState( const std::string& state ) const;
	void setState( const std::string& state, bool value );

	void addToInventory( const std::string& item );
	void removeFromInventory( const std::string& item );

	const InventoryItems& getInventory() const { return inventory_; }

	void update( unsigned long timeMilli );

private:
	GameState();
	~GameState();

	typedef std::map<std::string, bool>	GameStateMap;

	// game states
	GameStateMap	gameState_;

	// inventory
	InventoryItems	inventory_;		///< vektor predmetu v inventari
};

}

#endif // _GAMESTATE_H_
