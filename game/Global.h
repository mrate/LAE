#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// LAE
#include "../types/types.h"
#include "../math/Rectangle.h"
#include "../graphics/Font.h"
#include "../graphics/AnimationPlayer.h"
#include "../graphics/PAnimation.h"
#include "../cache/TextStorage.h"
// stl
#include <string>
#include <fstream>

namespace LAE {
class Global {
public:
	typedef std::map< std::string, PAnimation > PAnimationMap;

	bool init();
	bool load();
	bool load( const std::string& fname );

	void update( unsigned long timeMilli );

	// cursor
	void setDefaultCursor();
	void setCursor( PAnimation anim );
	const AnimationPlayer& getCursor() const { return cursor_; }

	// defaults
	PAnimation cursorAnim( const std::string& name ) const;
	PAnimationMap cursorMap() const { return cursors_; }
	PAnimation defaultCursor() const { return defaultCursor_; }

	// fonts
	const std::string& defaultFontName() const { return defaultFontName_; }
	const std::string& consoleFontName() const { return consoleFontName_; }
	const Font* defaultFont() const { return defaultFont_; }
	const FontMap& fontMap() const { return fonts_; }

	// inventory
	const TInventoryItemsMap& itemsMap() const { return invMap_; }
	const SInventoryItem& invItem( const std::string& name ) const;
	int inventoryPosX() const { return inventory_.posX; }
	int inventoryPosY() const { return inventory_.posY; }
	int invItemWidth() const { return inventory_.itemWidth; }
	int invItemHeight() const { return inventory_.itemHeight; }
	const std::string& inventoryScript() const { return inventory_.script; }

	// description
	const Rectangle& getDescriptionBox() const { return descriptionBox_; }

	// resolution
	int screenWidth() const { return screenWidth_; }
	int screenHeight() const { return screenHeight_; }
	int screenDepth() const { return screenBpp_; }

	// console
	int consoleMaxLines() const { return consoleMaxLines_; }

	// global script
	const std::string& globalScript() const { return globalScript_; }
	const std::string& globalInitFunction() const { return initFunction_; }

	const std::string& globalText() const { return globalText_; }

	// volume
	unsigned char getMusicVolume() const { return musicVolume_; }
	void setMusicVolume( unsigned char musicVolume );
	unsigned char getSoundVolume() const { return soundVolume_; }
	void setSoundVolume( unsigned char soundVolume );
	unsigned char getSpeechVolume() const { return speechVolume_; }
	void setSpeechVolume( unsigned char speechVolume );

	// min / max z-plane index
	int maxZPlaneIndex() const;
	int minZPlaneIndex() const;

	const TextStorage& texts() const { return texts_; }

	// singleton instance
	static Global* i();

private:
	Global();
	virtual ~Global();

	// volume
	unsigned char	musicVolume_;
	unsigned char	soundVolume_;
	unsigned char	speechVolume_;

	// cursors
	PAnimationMap		cursors_;
	AnimationPlayer		cursor_;
	PAnimation			cursorAnimation_;
	PAnimation			defaultCursor_;

	// global
	std::string			globalScript_;
	std::string			initFunction_;
	TInventoryItemsMap	invMap_;

	// script
	std::string			initScript_;

	// text
	std::string			globalText_;

	// fonts
	FontMap		fonts_;
	Font*		defaultFont_;
	std::string			defaultFontName_;

	// resolution
	int screenWidth_;
	int screenHeight_;
	int screenBpp_;

	// inventory
	struct Inventory {
		int posX;
		int posY;
		int itemWidth;
		int itemHeight;
		std::string script;

		Inventory() : posX(0), posY(0), itemWidth(0), itemHeight(0) {}
	} inventory_;

	// description box
	Rectangle descriptionBox_;
	int descriptionAlign_;

	// console
	int consoleMaxLines_;
	std::string consoleFontName_;

	TextStorage texts_;
};
}

#endif // _GLOBAL_H_
