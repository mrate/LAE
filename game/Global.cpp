// LAE
#include "Global.h"
#include "../logger/Logger.h"
#include "../utils/utils.h"
#include "../xml/Xml.h"
#include "../cache/AnimationCache.h"
#include "../cache/ImageCache.h"
#include "../game/GameEngine.h"
#include "../audio/MediaPlayer.h"
#include "../exception/Exception.h"
// boost
#include <boost/lexical_cast.hpp>
// tinyxml
#include <tinyxml.h>

namespace LAE {

Global::Global() 
	: musicVolume_( 100 )
	, soundVolume_( 200 )
	, speechVolume_( 200 )
	, consoleMaxLines_( 20 ) {
}

Global::~Global() {
}

PAnimation Global::cursorAnim( const std::string& name ) const {
	auto i = cursors_.find( name );
	if( i == cursors_.end() ) {
		assert( false && "Invalid cursor" );
		throw LAEException( "Invalid cursor " ) << name;
		//assert( false && "Invalid cursor" );
		//LOG_ERROR( "Cursor '" << name << "' doesn't exist (using default)!" );
		//return defaultCursor();
	} else {
		return i->second;
	}
}

bool Global::load() {
	return load( "settings.xml" );
}

bool Global::load( const std::string& fname ) {
	LOG_DEBUG( "Global load" );

	TiXmlDocument doc( fname.c_str() );
	if( !doc.LoadFile() ) {
		throw LAEException( "Error loading " ) << fname;
	}

	TiXmlHandle hDoc( &doc );
	TiXmlElement* pRoot = hDoc.FirstChildElement( "LAE" ).Element();
	if( !pRoot ) {
		throw LAEException( "Invalid settings.xml: missing LAE element" );
	}

	// <cursors>
	LOG_DEBUG( "...loading cursors" );	
	if( const TiXmlElement* cursorsE = pRoot->FirstChildElement( "cursors" ) ) {
		for( const TiXmlElement* cursorE = cursorsE->FirstChildElement( "cursor" ); cursorE; cursorE = cursorE->NextSiblingElement( "cursor" ) ) {
			std::string name = xml::loadAttribute<std::string>(cursorE, "name");
			std::string anim = xml::loadAttribute<std::string>(cursorE, "animation");
			PAnimation pAnim = AnimationManager::i()->createObject( anim );

			const char* def = cursorE->Attribute( "default" );
			if( def && strcmp( def, "true" ) == 0 ) {
				defaultCursor_ = pAnim;
			}

			cursors_[name] = pAnim;
		}
	}

	// <fonts>
	LOG_DEBUG( "...loading fonts" );	
	if( const TiXmlElement* fontsE = pRoot->FirstChildElement( "fonts" ) ) {
		for( const TiXmlElement* fontE = fontsE->FirstChildElement( "font" ); fontE; fontE = fontE->NextSiblingElement( "font" ) ) {
			std::string name = xml::loadAttribute<std::string>(fontE, "name");
			std::string file = xml::loadAttribute<std::string>(fontE, "file");
			std::string text = xml::loadAttribute<std::string>(fontE, "text");
			std::string img = xml::loadAttribute<std::string>(fontE, "img");

			LOG_DEBUG( "Loading font '" << file << "'..." );
			fonts_[name].load( file, text );
			fonts_[name].setImage( ImageManager::i()->createObject(img) );

			const char* def = fontE->Attribute( "default" );
			if( def && strcmp( def, "true" ) == 0 ) {
				defaultFontName_ = name;
				defaultFont_ = &fonts_[name];
			}
		}
	}

	// <inventory>
	LOG_DEBUG( "...loading inventory" );
	if( const TiXmlElement* inventoryE = pRoot->FirstChildElement( "inventory" ) ) {
		inventory_.script = xml::loadAttribute<std::string>( inventoryE, "script" );
		inventory_.itemWidth = xml::loadAttribute<int>( inventoryE, "item_width", 50 );
		inventory_.itemHeight = xml::loadAttribute<int>( inventoryE, "item_height", 50 );
		inventory_.posX = xml::loadAttribute<int>( inventoryE, "x_position", 50 );
		inventory_.posY = xml::loadAttribute<int>( inventoryE, "y_position", 50 );

		// inventory items
		for( const TiXmlElement* itemE = inventoryE->FirstChildElement( "item" ); itemE; itemE = itemE->NextSiblingElement() ) {
			SInventoryItem invItem;
			invItem.name = xml::loadAttribute<std::string>( itemE, "name" );
			invItem.description = xml::loadAttribute<std::string>( itemE, "description" );
			std::string anim = xml::loadAttribute<std::string>( itemE, "animation" );
			invItem.animation = AnimationManager::i()->createObject(anim);
			invItem.cursorName = xml::loadAttribute<std::string>( itemE, "cursor" );
			invItem.onExamine = xml::loadAttribute<std::string>( itemE, "onExamine" );
			invItem.onUse = xml::loadAttribute<std::string>( itemE, "onUse" );

			invMap_[invItem.name] = invItem;
		}
	}

	// <global>
	LOG_DEBUG( "...loading global" );
	if( const TiXmlElement* globalE = pRoot->FirstChildElement( "global" ) ) {
		// <resolution>
		const TiXmlElement* resE = globalE->FirstChildElement( "resolution" );
		screenWidth_ = xml::loadAttribute<int>( resE, "width" );
		screenHeight_ = xml::loadAttribute<int>( resE, "height" );
		screenBpp_ = xml::loadAttribute<int>( resE, "depth" );

		// <script>
		const TiXmlElement* scriptE = globalE->FirstChildElement( "script" );
		globalScript_ = xml::loadAttribute<std::string>( scriptE, "file" );
		initFunction_ = xml::loadAttribute<std::string>( scriptE, "init_function" );

		// <text>
		globalText_ = xml::loadAttribute<std::string>( globalE, "text" );
		texts_.load( globalText_ );

		// <item_description>
		const TiXmlElement* descrE = globalE->FirstChildElement( "item_description_box" );
		descriptionBox_.setX( xml::loadAttribute<int>(descrE, "x") );
		descriptionBox_.setY( xml::loadAttribute<int>(descrE, "y") );
		descriptionBox_.setWidth( xml::loadAttribute<int>(descrE, "width") );
		descriptionBox_.setHeight( xml::loadAttribute<int>(descrE, "height") );
		
		if(const TiXmlElement* alignE = descrE->FirstChildElement( "text_align" ) ) {
			std::string align = alignE->FirstChild()->Value();
			if( align == "center" ) {
				descriptionAlign_ = 0;
			}
		}

		// <console>
		const TiXmlElement* consoleE = globalE->FirstChildElement( "console" );
		consoleMaxLines_ = xml::loadAttribute<int>( consoleE, "max_lines");
		consoleFontName_ = xml::loadAttribute<std::string>( consoleE, "font");

		// <init>
		const TiXmlElement* initE = globalE->FirstChildElement( "init" );
		initScript_ = xml::loadAttribute<std::string>(initE, "script");
	}

	LOG_DEBUG( "Global configuration loaded" );
	return true;
}

bool Global::init() {
	LOG_DEBUG( "Global init" );

	// nacteni kurzoru
	LOG_DEBUG( "...caching cursor animations" );
	//for_each_value( cursors_.begin(), cursors_.end(), boost::bind( &AnimationCache::getObject, _1, true ) );

	// objekty inventare
	LOG_DEBUG( "...caching inventory items animations..." );
	//for( TInventoryItemsMap::iterator i = invMap_.begin(); i != invMap_.end(); ++i ) {
	//	AnimationCache::getObject( i->second.animation, true );
	//}

	LOG_DEBUG( "Global init OK" );
	return true;
}

Global* Global::i() {
	static Global instance;
	return &instance;
}

void Global::setCursor( PAnimation anim ) {
	if( cursorAnimation_ != anim ) {
		cursor_.start( anim, true );
		cursorAnimation_ = anim;
	}
}

void Global::setDefaultCursor() {
	setCursor( defaultCursor_ );
}

void Global::update( unsigned long timeMilli ) {
	cursor_.update(timeMilli);
}

int Global::maxZPlaneIndex() const {
	//@@TODO:
	return 5;
}

int Global::minZPlaneIndex() const {
	//@@TODO:
	return 0;
}

void Global::setMusicVolume( unsigned char musicVolume ) {
	musicVolume_ = musicVolume;
	MediaPlayer::i()->setStreamVolume(musicVolume);
}

void Global::setSoundVolume( unsigned char soundVolume ) {
	soundVolume_ = soundVolume;
}

void Global::setSpeechVolume( unsigned char speechVolume ) {
	speechVolume_ = speechVolume;
	//@@TODO:
	//MediaPlayer::i()->setChannelVolume( GameEngine::SPEECH_AUDIO_CHANNEL, speechVolume );
}

const SInventoryItem& Global::invItem( const std::string& name ) const {
	 auto it = invMap_.find(name);
	 if( it == invMap_.end() ) {
		 assert( false && "Invalid inventory item" );
		 throw LAEException( "Invalid inventory item " ) << name;
	 }
	 return it->second;
}

}
