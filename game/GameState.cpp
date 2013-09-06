// LAE
#include "GameState.h"
#include "../game/Global.h"
// stl
#include <algorithm>

namespace LAE {

GameState::GameState() {
}

GameState::~GameState() {
}

GameState* GameState::i() {
	static GameState instance;
	return &instance;
}

bool GameState::getState( const std::string& state ) const {
	auto it = gameState_.find(state);
	return it == gameState_.end() ? false : it->second;
}

void GameState::setState( const std::string& state, bool value ) {
	gameState_[state] = value;
}

void GameState::addToInventory( const std::string& item ) {
	inventory_.push_back( InventoryItem() );
	InventoryItem& newItem = inventory_.back();
	newItem.name = item;
	newItem.setX( 0 );
	newItem.setY( 0 );
	newItem.setAnimation( Global::i()->invItem( item ).animation, true );
}

void GameState::removeFromInventory( const std::string& itemName ) {
	inventory_.erase( std::remove_if( inventory_.begin(), inventory_.end(), [&]( const InventoryItem& item ) { return item.name == itemName; } ), inventory_.end() );
}

void GameState::update( unsigned long timeMilli ) {
	for( auto i = inventory_.begin(); i != inventory_.end(); i++ ) {
		i->update( timeMilli );
	}
}

}
