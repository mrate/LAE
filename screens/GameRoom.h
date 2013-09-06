#pragma once
#ifndef _GAMEROOM_H_
#define _GAMEROOM_H_

// LAE
#include "../types/types.h"
#include "../graphics/AnimatedSprite.h"
#include "../graphics/PImage.h"
#include "../objects/Item.h"

namespace LAE {

/// Game room class
class GameRoom {
public:
	/// Map of sprites ordered by Z-axis
	typedef std::map<int, AnimatedSprites>	ZIndexSpriteMap;

	GameRoom();
	virtual ~GameRoom();

	// nacitani / ukladani
	//void loadBin( const std::string& fileName );
	//void saveBin( const std::string& fileName );

	/// Loads room from given file name
	void load( const std::string& fileName );
	/// Saves room to given file name
	void save( const std::string& fileName ) const;

	/// Release all resources
	void free();
	/// Update all animations in room
	void update( unsigned long timeMili );

	/// Detect neighbours of all walking boxes in room
	void detectNeighbours();

	/// Returns width
	unsigned short getWidth() const { return width_; }
	/// Sets new width
	void setWidth(unsigned short width) { width_ = width; }

	/// Returns height
	unsigned short getHeight() const { return height_; }
	/// Sets new height
	void setHeight(unsigned short height) { height_ = height; }

	/// Retuns room script file name
	const std::string& getRoomScript() const { return roomScript_; }
	/// Sets new room script file name
	void setRoomScript( const std::string& roomScript ) { roomScript_ = roomScript; }
	
	/// Returns background image
	PImage getBgImage() const { return bgImage_; }
	/// Sets new background image
	void setBgImage( PImage bgImage ) { bgImage_ = bgImage; }

	/// Returns room text file
	const std::string& getTextFile() const { return textFile_; }
	/// Sets new room text file
	void setTextFile(const std::string& textFile) { textFile_ = textFile; }

	/// Sets walking box with given index (non-)active
	void setBoxActive( unsigned char boxIndex, bool active );
	/// Returns all walking boxes (const)
	const WalkingBoxes& getBoxes() const { return boxes_; }
	/// Returns all walking boxes
	WalkingBoxes& getBoxes() { return boxes_; }

	/// Returns all scales (const)
	const Scales& getScales() const { return scales_; }
	/// Returns all scales
	Scales& getScales() { return scales_; }
	/// Returns all lights (const)
	const Lights& getLights() const { return lights_; }
	/// Returns all lights
	Lights& getLights() { return lights_; }
	/// Returns all items (const)
	const ItemHashMap& getItems() const { return items_; }
	/// Returns all items
	ItemHashMap& getItems() { return items_; }
	/// Returns all z-planes
	AnimatedSprites& getZPlanes() { return zplanes_; }
	/// Returns all sprites of given z-order
	const AnimatedSprites* getLayer(int z) const;

	/// Returns minimum z-order
	int getMinZPlaneIndex() const { return minZPlaneIndex_; }
	/// Returns maximum z-order
	int getMaxZPlaneIndex() const { return maxZPlaneIndex_; }

private:
	void updateZMapIndecies();
	bool findBoxInter( const WalkingBox& w1, const WalkingBox& w2, Segment& inter ) const;

	std::string		roomScript_;	///< jmeno skriptu mistnosti
	PImage			bgImage_;		///< jmeno obrazku s pozadim mistnosti
	std::string		textFile_;		///< jmeno souboru s textem

	unsigned short	width_;		///< sirka mistnosti
	unsigned short	height_;	///< vyska mistnosti

	//StringMap			stringMap_;	///<
	ItemHashMap			items_;		///< hash mapa predmetu
	AnimatedSprites		zplanes_;	///< vektor Z-Planes
	WalkingBoxes				boxes_;		///< boxy chozeni
	Scales			scales_;		///< zmeny meritka postavicek
	Lights			lights_;		///< svetelne zdroje

	ZIndexSpriteMap		zmap_;		///<
	int minZPlaneIndex_;
	int maxZPlaneIndex_;
};
}

#endif // _GAMEROOM_H_
