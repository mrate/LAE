// LAE
#include "GameRoom.h"
#include "../utils/utils.h"
#include "../logger/Logger.h"
#include "../game/Global.h"
#include "../cache/AnimationCache.h"
#include "../cache/ImageCache.h"
#include "../math/MathUtils.h"
#include "../xml/Xml.h"
//#include "../../LAE_FileSystem/File.h"
// boost
#include <boost/bind.hpp>
// stl
#include <algorithm>
#include <fstream>
#include <limits>
#include <math.h>
// xml
#include <tinyxml.h>

namespace LAE {

GameRoom::GameRoom() 
	: width_( 0 )
	, height_( 0 )
	, minZPlaneIndex_( 0 )
	, maxZPlaneIndex_( 0 ) {
}

GameRoom::~GameRoom() {
	free();
}

/*
void GameRoom::loadBin( const std::string& fname ) {
	free();

	try {
		string anim;

		LOG_DEBUG( "Loading room '" << fname << "' ..." );
		CFile fr( fname.c_str() );

		width_ = fr.readShort();
		height_ = fr.readShort();
		LOG_DEBUG( "width = " << width_ << ", height = " << height_ );

		std::string bgImageFName;
		fr.readString( bgImageFName );
		bgImage_ = ImageManager::i()->createObject( bgImageFName );
		fr.readString( roomScript_ );
		fr.readString( textFile_ );

		LOG_DEBUG( "bgImage = " << bgImageFName );

		// zplanes
		int zplanesCount = fr.readUChar();;
		LOG_DEBUG( "zplanes: " << zplanesCount );
		//zplanes = new AnimatedSprite[zplanesCount];
		zplanes_.reserve( zplanesCount );
		for( int i = 0; i < zplanesCount; i++ ) {
			zplanes_.push_back( AnimatedSprite() );
			AnimatedSprite& zplane = zplanes_.back();

			zplane.setX( fr.readShort() );
			zplane.setY( fr.readShort() );
			zplane.setZ( fr.readShort() );
			fr.readString( anim );
			zplane.setAnimation( AnimationManager::i()->createObject( anim ), true );
			zplane.setEffect( ( Screen::EffectType )fr.readUChar() );
			zplane.setLightable( fr.readUChar() == 1 );

			zmap_[zplane.getZ()].push_back( &zplane );

			LOG_DEBUG( "   x: " << zplane.getX() );
			LOG_DEBUG( "   y: " << zplane.getY() );
			LOG_DEBUG( "   z: " << zplane.getZ() );
			//LOG_DEBUG(" img: " << zplane.image << Log::endl);
		}

		// scales
		int scalesCount = fr.readUChar();
		//scales = new SScale[scalesCount];
		scales_.reserve( scalesCount );
		LOG_DEBUG( "scales: " << scalesCount );
		int y1, y2, scale1, scale2;
		for( int i = 0; i < scalesCount; i++ ) {
			scales_.push_back( Scale() );
			Scale& scale = scales_.back();

			scale.p0.x = fr.readShort();
			y1 = fr.readShort();
			scale.p1.x = fr.readShort();
			y2 = fr.readShort();

			scale1 = fr.readUChar();
			scale2 = fr.readUChar();

			scale.p0.y = min( y1, y2 );
			scale.p1.y = max( y1, y2 );
			scale.scale1 = min( scale1, scale2 );
			scale.scale2 = max( scale1, scale2 );

			LOG_DEBUG( "  " << scale.p0.x << "," << scale.p0.y << ":" << ( int )scale.scale1 << " -> " <<
			          scale.p1.x << "," << scale.p1.y << ":" << ( int )scale.scale2 );
		}

		// boxes
		int boxesCount = fr.readUChar();
		boxes_.reserve( boxesCount );
		//boxes = new SBox[boxesCount];
		LOG_DEBUG( "boxes: " << boxesCount );
		boxMap_ = new IntVector[boxesCount];
		for( int i = 0; i < boxesCount; i++ ) {
			boxes_.push_back( WalkingBox() );
			WalkingBox& box = boxes_.back();

			box.center.x = 0;
			box.center.y = 0;

			for( int j = 0; j < 4; j++ ) {
				box.p[j].x = fr.readShort();
				box.p[j].y = fr.readShort();

				box.center.x += box.p[j].x;
				box.center.y += box.p[j].y;

				LOG_DEBUG( "   p[" << j << "]: " << box.p[j].x << ", " << box.p[j].y );
			}

			box.center.x /= 4;
			box.center.y /= 4;
			box.z = fr.readShort();
			box.scale = fr.readUChar();
			box.active = true;
			LOG_DEBUG( "   z:" << box.z );

			// neighbours
			unsigned char neighbours = fr.readUChar();
			for( int j = 0; j < neighbours; j++ ) {
				boxMap_[i].push_back( fr.readUChar() );
			}
		}

		// items
		int count = fr.readUChar();
		LOG_DEBUG( "items: " << count );
		for( int i = 0; i < count; i++ ) {
			std::string name;
			short x, y, z, w, h;
			bool active;

			x = fr.readShort();
			y = fr.readShort();
			z = fr.readShort();
			w = fr.readShort();
			h = fr.readShort();
			active = fr.readUChar() == 1;
			fr.readString( name );

			items_[name] = Item();
			Item& item = items_[name];

			item.setX( x );
			item.setY( y );
			item.setZ( z );
			item.setWidth( w );
			item.setHeight( h );
			item.setActive( active );
			item.setName( name );

			std::string descr;
			fr.readString( descr );
			item.setDescription( descr );

			std::string cursor;
			fr.readString( cursor );
			item.setCursorName( cursor );

			fr.readString( anim );
			item.setAnimation( AnimationManager::i()->createObject( anim ), true );
			
			std::string onExamine;
			fr.readString( onExamine );
			item.setOnExamine( onExamine );
			
			std::string onUse;
			fr.readString( onUse );
			item.setOnUse( onUse );
			
			item.setEffect( ( Screen::EffectType )fr.readUChar() );
			item.setLightable( fr.readUChar() == 1 );

			//if( item.getCursor().size() == 0 ) {
			//	item.setCursor( Global::i()->defaultCursor() );
			//}

			zmap_[z].push_back( &item );

			//
			LOG_DEBUG( "   x:" << item.getX() );
			LOG_DEBUG( "   y:" << item.getY() );
			LOG_DEBUG( "   w:" << item.getWidth() );
			LOG_DEBUG( "   h:" << item.getHeight() );
			LOG_DEBUG( "   n:" << item.getName() );
			LOG_DEBUG( "   d:" << item.getDescription() );
			LOG_DEBUG( " cur:" << item.getCursorName() );
			LOG_DEBUG( "  ex:" << item.getOnExamine() );
			LOG_DEBUG( " use:" << item.getOnUse() );
		}

		// lights
		int lightsCount = fr.readUChar();
		lights_.reserve( lightsCount );
		//lights = new SLightSource[lightsCount];
		for( int i = 0; i < lightsCount; ++i ) {
			lights_.push_back( LightSource() );
			LightSource& light = lights_.back();

			light.x = fr.readShort();
			light.y = fr.readShort();
			light.intensity = fr.readUChar();
			light.active = fr.readUChar() == 1;
		}

		LOG_DEBUG( "loading done" );
	} catch( std::exception& ex ) {
		LOG_ERROR( "Error loading room " << fname << ": " << ex.what() );
		throw ;
	}

	detectNeighbours();
	updateZMapIndecies();
}

void GameRoom::saveBin( const std::string& fname ) {
	try {
		CFile fr( fname.c_str(), CFile::WRITE );

		fr.writeShort( width_ );
		fr.writeShort( height_ );
		fr.writeString( bgImage_->getFileName() );
		fr.writeString( roomScript_ );
		fr.writeString( textFile_ );

		// zplanes
		fr.writeUChar( ( unsigned char )zplanes_.size() );
		for( AnimatedSpritesVector::const_iterator i = zplanes_.begin(); i != zplanes_.end(); ++i ) {
			const AnimatedSprite& zplane = *i;
			fr.writeShort( zplane.getX() );
			fr.writeShort( zplane.getY() );
			fr.writeShort( zplane.getZ() );
			fr.writeString( zplane.getAnimation()->getFileName() );
			fr.writeUChar( ( unsigned char )zplane.getEffect() );
			fr.writeUChar( zplane.isLightable() ? 1 : 0 );
		}

		// scales
		fr.writeUChar( ( unsigned char )scales_.size() );
		for( Scales::const_iterator i = scales_.begin(); i != scales_.end(); ++i ) {
			const Scale& scale = *i;
			fr.writeShort( scale.p0.x );
			fr.writeShort( scale.p0.y );
			fr.writeShort( scale.p1.x );
			fr.writeShort( scale.p1.y );
			fr.writeUChar( scale.scale1 );
			fr.writeUChar( scale.scale2 );
		}

		// boxes

		// detekce sousedu
		detectNeighbours();

		fr.writeUChar( ( unsigned char )boxes_.size() );
		//for(TBoxVector::const_iterator i=boxes.begin();i!=boxes.end();++i)
		for( size_t i = 0, size = boxes_.size(); i < size; ++i ) {
			//const SBox &box = *i;
			const WalkingBox& box = boxes_[i];
			for( int j = 0; j < 4; ++j ) {
				fr.writeShort( box.p[j].x );
				fr.writeShort( box.p[j].y );
			}
			fr.writeShort( box.z );
			fr.writeUChar( box.scale );

			//
			//fr.writeUChar(0);

			//fr.writeUChar(neightbours);
			//for(int j=0;j<neighbours;j++)
			//{
			//	fr.writeUChar(neighbour[j]);
			//}

			unsigned char count = 0;
			for( unsigned char j = 0; j < boxes_.size(); ++j )
				if( box.neighbour[j] ) {
					++count;
				}

			fr.writeUChar( count );
			for( unsigned char j = 0; j < boxes_.size(); ++j )
				if( box.neighbour[j] ) {
					fr.writeUChar( j );
				}
		}

		// items
		fr.writeUChar( ( unsigned char )items_.size() );
		for( ItemHashMap::const_iterator i = items_.begin(); i != items_.end(); ++i ) {
			const Item& item = i->second;
			fr.writeShort( item.getX() );
			fr.writeShort( item.getY() );
			fr.writeShort( item.getZ() );
			fr.writeShort( item.getWidth() );
			fr.writeShort( item.getHeight() );
			fr.writeUChar( item.isActive() ? 1 : 0 );
			fr.writeString( item.getName() );
			fr.writeString( item.getDescription() );
			fr.writeString( item.getCursorName() );
			fr.writeString( item.getAnimation()->getFileName() );
			fr.writeString( item.getOnExamine() );
			fr.writeString( item.getOnUse() );
			fr.writeUChar( ( unsigned char )item.getEffect() );
			fr.writeUChar( item.isLightable() ? 1 : 0 );
		}

		// lights
		fr.writeUChar( ( unsigned char )lights_.size() );
		for( Lights::const_iterator i = lights_.begin(); i != lights_.end(); ++i ) {
			const LightSource& light = *i;
			fr.writeShort( light.x );
			fr.writeShort( light.y );
			fr.writeUChar( light.intensity );
			fr.writeUChar( light.active );
		}
	} catch( std::exception& ex ) {
		LOG_ERROR( "Error saving room " << fname << ": " << ex.what() );
		throw ;
	}
}
*/

void GameRoom::free() {
	LOG_DEBUG( "freeing memory" );

	// erase all
	std::for_each( zplanes_.begin(), zplanes_.end(), []( AnimatedSprite* s ) { delete s; } );
	boxes_.clear();
	zplanes_.clear();
	scales_.clear();
	zmap_.clear();
	items_.clear();
}

void GameRoom::update( unsigned long timeMili ) {
	// aktualizace animaci z-planes a objektu
	std::for_each( zplanes_.begin(), zplanes_.end(), boost::bind( &AnimatedSprite::update, _1, timeMili ) );

	// aktualizace itemu v mistnosti
	for_each_value( items_.begin(), items_.end(), boost::bind( &Item::update, _1, timeMili ) );
}

void GameRoom::detectNeighbours() {
	size_t size = boxes_.size();

	//@@TODO: seznam sousedu bude vybirat uzivatel z prekryvajicich se boxu
	for( size_t i = 0; i < size; ++i ) {
		boxes_[i].neighbour.clear();
		boxes_[i].inter.clear();
	}

	for( size_t i = 0; i < size - 1; ++i ) {
		WalkingBox& box1 = boxes_[i];
		Polygon p1( box1.p[0], box1.p[1], box1.p[2], box1.p[3] );

		for( size_t j = i + 1; j < size; ++j ) {
			WalkingBox& box2 = boxes_[j];
			Polygon p2( box2.p[0], box2.p[1], box2.p[2], box2.p[3] );

			Segment inter;

			if( findInter( p1, p2, inter ) ) {
				box1.neighbour.insert(j);
				box2.neighbour.insert(i);

				box1.inter.insert( std::make_pair(j, inter) );
				box2.inter.insert( std::make_pair(i, inter) );
			}
		}
	}

	//@@TODO: find all shortest paths...
}

bool GameRoom::findBoxInter( const WalkingBox& box1, const WalkingBox& box2, Segment& inter ) const {
	Polygon p1( box1.p[0], box1.p[1], box1.p[2], box1.p[3] );
	Polygon p2( box2.p[0], box2.p[1], box2.p[2], box2.p[3] );
	return findInter( p1, p2, inter );
}

void GameRoom::load( const std::string& fileName ) {
	LOG_DEBUG( "Loading room '" << fileName << "' ..." );
	free();

	try {
		TiXmlDocument document;
		document.LoadFile(fileName.c_str());

		TiXmlElement* roomElement = document.RootElement();
		width_ = xml::loadAttribute<unsigned short>( roomElement, "width" );
		height_ = xml::loadAttribute<unsigned short>( roomElement, "height" );
		std::string bgImage = xml::loadAttribute<std::string>( roomElement, "bgImage" );
		bgImage_ = ImageManager::i()->createObject(bgImage);
		roomScript_ = xml::loadAttribute<std::string>( roomElement, "script" );
		textFile_ = xml::loadAttribute<std::string>( roomElement, "text" );

		LOG_DEBUG( "...width = " << width_ << ", height = " << height_ );
		LOG_DEBUG( "...bgImage = " << bgImage_ );

		// zplanes
		TiXmlElement* zPlanesE = roomElement->FirstChildElement("zplanes");
		if( zPlanesE ) {
			for( TiXmlElement* zPlaneE = zPlanesE->FirstChildElement("zplane"); zPlaneE; zPlaneE = zPlaneE->NextSiblingElement("zplane") ) {
				AnimatedSprite* zplane = new AnimatedSprite();

				zplane->setX( xml::loadAttribute<short>(zPlaneE, "x") );
				zplane->setY( xml::loadAttribute<short>(zPlaneE, "y") );
				zplane->setZ( xml::loadAttribute<short>(zPlaneE, "z") );
				std::string anim = xml::loadAttribute<std::string>(zPlaneE, "animation");
				zplane->setAnimation( AnimationManager::i()->createObject(anim), true );
				zplane->setEffect( static_cast<Screen::EffectType>( xml::loadAttribute<int>(zPlaneE, "effect") ) );
				zplane->setLightable( xml::loadAttribute<bool>(zPlaneE, "lightable") );

				zplanes_.push_back( zplane );
				zmap_[zplane->getZ()].push_back( zplane );

//				LOG_DEBUG( "...zplane(" << zplanes_.size() << "):" );
//				LOG_DEBUG( "......x: " << zplane.getX() );
//				LOG_DEBUG( "......y: " << zplane.getY() );
//				LOG_DEBUG( "......z: " << zplane.getZ() );
//				LOG_DEBUG( "......a: " << zplane.getAnimation()->getFileName() );
//				assert( zplane.getAnimation() );
			}
		}

		// scales
		TiXmlElement* scalesE = roomElement->FirstChildElement( "scales" );
		if( scalesE ) {
			int y1, y2, scale1, scale2;
			for( TiXmlElement* scaleE = scalesE->FirstChildElement("scale"); scaleE; scaleE = scaleE->NextSiblingElement("scale") ) {
				scales_.push_back( Scale() );
				Scale& scale = scales_.back();

				scale.p0.x = xml::loadAttribute<int>( scaleE, "x1" );
				y1 = xml::loadAttribute<int>( scaleE, "y1" );
				scale.p1.x = xml::loadAttribute<int>( scaleE, "x2" );
				y2 = xml::loadAttribute<int>( scaleE, "y2" );
				scale1 = xml::loadAttribute<int>( scaleE, "scale1" );
				scale2 = xml::loadAttribute<int>( scaleE, "scale2" );

				scale.p0.y = min( y1, y2 );
				scale.p1.y = max( y1, y2 );
				scale.scale1 = min( scale1, scale2 );
				scale.scale2 = max( scale1, scale2 );

				LOG_DEBUG( "...scale:" );
				LOG_DEBUG( "......" << scale.p0.x << "," << scale.p0.y << ":" << ( int )scale.scale1 << " -> " <<
					scale.p1.x << "," << scale.p1.y << ":" << ( int )scale.scale2 );
			}
		}

		// boxes
		TiXmlElement* boxesE = roomElement->FirstChildElement("boxes");
		if( boxesE ) {
			int boxesCount = 0;
			for( TiXmlElement* boxE = boxesE->FirstChildElement("box"); boxE; boxE = boxE->NextSiblingElement("box") ) {
				++boxesCount;
			}

			for( TiXmlElement* boxE = boxesE->FirstChildElement("box"); boxE; boxE = boxE->NextSiblingElement("box") ) {
				boxes_.push_back( WalkingBox() );
				WalkingBox& box = boxes_.back();

				box.center.x = 0;
				box.center.y = 0;
				box.z = boost::lexical_cast<short>( boxE->Attribute("z") );
				box.scale = boost::lexical_cast<int>( boxE->Attribute("scale") );

				int pointIndex = 0;
				for( TiXmlElement* pointE = boxE->FirstChildElement("point"); pointE && pointIndex < 4; pointE = pointE->NextSiblingElement("point") ) {			
					box.p[pointIndex].x = boost::lexical_cast<double>( pointE->Attribute( "x" ) );
					box.p[pointIndex].y = boost::lexical_cast<double>( pointE->Attribute( "y" ) );
					box.center.x += box.p[pointIndex].x;
					box.center.y += box.p[pointIndex].y;
					++pointIndex;

					//LOG_DEBUG( "......p[" << pointIndex << "]: " << box.p[j].x << ", " << box.p[j].y );
				}

				box.center.x /= 4;
				box.center.y /= 4;
				box.active = true;

				// neighbours
				TiXmlElement* neighboursE = boxE->FirstChildElement( "neighbours" );
				if( neighboursE ) {
					for( TiXmlElement* neighE = neighboursE->FirstChildElement("neighbour"); neighE; neighE = neighE->NextSiblingElement("neighbour") ) {
						const char* v = neighE->Attribute("box");
						unsigned char dst = boost::lexical_cast<int>(v);
						box.neighbour.insert( dst );
					}
				}
			}
		}

		for( size_t i = 0; i <boxes_.size(); ++i ) {
			WalkingBox& box1 = boxes_[i];
			for( auto it = box1.neighbour.begin(); it != box1.neighbour.end(); ++it ) {
				WalkingBox& box2 = boxes_[*it];

				Segment inter;
				findBoxInter( box1, box2, inter );
				box1.inter.insert( std::make_pair(*it, inter) );
				box2.inter.insert( std::make_pair(i, inter) );
			}
		}

		// items
		LOG_DEBUG( "...items" );
		TiXmlElement* itemsE = roomElement->FirstChildElement("items");
		if( itemsE ) {
			for( TiXmlElement* itemE = itemsE->FirstChildElement("item"); itemE; itemE = itemE->NextSiblingElement("item") ) {
				Item item;
				item.setX( xml::loadAttribute<short>( itemE, "x" ) );
				item.setY( xml::loadAttribute<short>( itemE, "y" ) );
				item.setZ( xml::loadAttribute<short>( itemE, "z" ) );

				TiXmlElement* activeRectE = itemE->FirstChildElement( "active_rect" );
				if( activeRectE != nullptr ) {
					Rectangle rect;
					rect.setX( xml::loadAttribute<int>( activeRectE, "x" ) );
					rect.setY( xml::loadAttribute<int>( activeRectE, "y" ) );
					rect.setWidth( xml::loadAttribute<int>( activeRectE, "w" ) );
					rect.setHeight( xml::loadAttribute<int>( activeRectE, "h" ) );
					item.setActiveRect(rect);
				}

				item.setActive( xml::loadAttribute<bool>( itemE, "active" ) );
				item.setName( xml::loadAttribute<std::string>( itemE, "name" ) );
				item.setDescription( xml::loadAttribute<std::string>( itemE, "description" ) );			
				item.setCursorName( xml::loadAttribute<std::string>( itemE, "cursor" ) );
				
				std::string anim = xml::loadAttribute<std::string>( itemE, "animation" );
				item.setAnimation( AnimationManager::i()->createObject(anim), true );
				item.setOnExamine( xml::loadAttribute<std::string>( itemE, "onExamine" ) );
				item.setOnUse( xml::loadAttribute<std::string>( itemE, "onUse" ) );
				item.setEffect( static_cast<Screen::EffectType>( xml::loadAttribute<int>( itemE, "effect" ) ) );
				item.setLightable( xml::loadAttribute<bool>( itemE, "lightable" ) );

				assert( item.getAnimation() );
				items_.insert( std::make_pair( item.getName(), item ) );
				zmap_[item.getZ()].push_back( &items_[item.getName()] );

				LOG_DEBUG( "...item:" << (std::dec) );
				LOG_DEBUG( "......x:" << item.getX() );
				LOG_DEBUG( "......y:" << item.getY() );
				LOG_DEBUG( "......z:" << item.getZ() )
				LOG_DEBUG( "......w:" << item.getActiveRect().width() );
				LOG_DEBUG( "......h:" << item.getActiveRect().height() );
				LOG_DEBUG( "......n:" << item.getName() );
				LOG_DEBUG( "......d:" << item.getDescription() );
				LOG_DEBUG( "......cur:" << item.getCursorName() );
				LOG_DEBUG( "......ex:" << item.getOnExamine() );
				LOG_DEBUG( "......use:" << item.getOnUse() );
				LOG_DEBUG( "......anim: " << item.getAnimation()->getFileName() );
			}
		}

		// lights
		LOG_DEBUG( "...lights" );
		TiXmlElement* lightsE = roomElement->FirstChildElement("lights");
		if( lightsE ) {
			for( TiXmlElement* lightE = lightsE->FirstChildElement("light"); lightE; lightE = lightE->NextSiblingElement("light") ) {
				LightSource light;
				light.x = xml::loadAttribute<short>( lightE, "x" );
				light.y = xml::loadAttribute<short>( lightE, "y" );
				light.intensity = xml::loadAttribute<int>( lightE, "intensity" );
				light.active = xml::loadAttribute<bool>( lightE, "active" );

				lights_.push_back( light );
			}
		}

		LOG_DEBUG( "Room loaded" );
	} catch( std::exception& ex ) {
		LOG_ERROR( "Error loading room " << fileName << ": " << ex.what() );
		free();
		throw ;
	}

	detectNeighbours();
	updateZMapIndecies();
}

void GameRoom::save( const std::string& fileName ) const {
	TiXmlDocument document;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );

	TiXmlElement* roomElement = new TiXmlElement( "room" );
	document.LinkEndChild(decl);
	document.LinkEndChild(roomElement);

	try {
		xml::saveAttribute( roomElement, "width", width_ );
		xml::saveAttribute( roomElement, "height", height_ );
		xml::saveAttribute( roomElement, "bgImage", bgImage_->getFileName() );
		xml::saveAttribute( roomElement, "script", roomScript_ );
		xml::saveAttribute( roomElement, "text", textFile_ );

		// zplanes
		TiXmlElement* zplanesE = new TiXmlElement("zplanes");
		roomElement->LinkEndChild(zplanesE);

		for( auto i = zplanes_.begin(); i != zplanes_.end(); ++i ) {
			const AnimatedSprite* zplane = *i;

			TiXmlElement* zplaneE = new TiXmlElement("zplane");
			zplanesE->LinkEndChild(zplaneE);

			xml::saveAttribute( zplaneE, "x", zplane->getX() );
			xml::saveAttribute( zplaneE, "y", zplane->getY() );
			xml::saveAttribute( zplaneE, "z", zplane->getZ() );
			xml::saveAttribute( zplaneE, "animation", zplane->getAnimation()->getFileName() );
			xml::saveAttribute( zplaneE, "effect", zplane->getEffect() );
			xml::saveAttribute( zplaneE, "lightable", zplane->isLightable() ? 1 : 0 );
		}

		// scales
		TiXmlElement* scalesE = new TiXmlElement("scales");
		roomElement->LinkEndChild(scalesE);

		for( Scales::const_iterator i = scales_.begin(); i != scales_.end(); ++i ) {
			const Scale& scale = *i;

			TiXmlElement* scaleE = new TiXmlElement( "scale" );
			scalesE->LinkEndChild(scaleE);

			xml::saveAttribute( scaleE, "x1", scale.p0.x );
			xml::saveAttribute( scaleE, "y1", scale.p0.y );
			xml::saveAttribute( scaleE, "x2", scale.p1.x );
			xml::saveAttribute( scaleE, "y2", scale.p1.y );
			xml::saveAttribute( scaleE, "scale1", scale.scale1 );
			xml::saveAttribute( scaleE, "scale2", scale.scale2 );
		}

		// boxes

		// detekce sousedu
		//detectNeighbours();

		TiXmlElement* boxesE = new TiXmlElement("boxes");
		roomElement->LinkEndChild(boxesE);

		for( size_t i = 0, size = boxes_.size(); i < size; ++i ) {
			const WalkingBox& box = boxes_[i];

			TiXmlElement* boxE = new TiXmlElement( "box" );
			boxesE->LinkEndChild(boxE);

			boxE->SetAttribute("z", box.z);
			boxE->SetAttribute("scale", box.scale);

			for( int j = 0; j < 4; ++j ) {
				TiXmlElement* pE = new TiXmlElement("point");
				pE->SetAttribute( "x", boost::lexical_cast<std::string>( box.p[j].x ).c_str() );
				pE->SetAttribute( "y", boost::lexical_cast<std::string>( box.p[j].y ).c_str() );
				boxE->LinkEndChild(pE);
			}

			TiXmlElement* neighboursE = new TiXmlElement("neighbours");
			boxE->LinkEndChild(neighboursE);

			for( unsigned char j = 0; j < boxes_.size(); ++j ) {
				if( box.neighbour.find(j) != box.neighbour.end() ) {
					TiXmlElement* neighbourE = new TiXmlElement("neighbour");
					neighbourE->SetAttribute("box", j);
					neighboursE->LinkEndChild(neighbourE);
				}
			}
		}

		// items
		TiXmlElement* itemsE = new TiXmlElement("items");
		roomElement->LinkEndChild(itemsE);

		for( auto i = items_.begin(); i != items_.end(); ++i ) {
			const Item& item = i->second;

			TiXmlElement* itemE = new TiXmlElement("item");
			itemsE->LinkEndChild(itemE);

			xml::saveAttribute(itemE, "x", item.getX() );
			xml::saveAttribute(itemE, "y", item.getY() );
			xml::saveAttribute(itemE, "z", item.getZ() );

			TiXmlElement* activeRectE = new TiXmlElement("active_rect");
			itemE->LinkEndChild(activeRectE);

			xml::saveAttribute(activeRectE, "x", item.getActiveRect().x());
			xml::saveAttribute(activeRectE, "y", item.getActiveRect().y());
			xml::saveAttribute(activeRectE, "w", item.getActiveRect().width());
			xml::saveAttribute(activeRectE, "h", item.getActiveRect().height());

			xml::saveAttribute(itemE, "active", item.isActive() );
			xml::saveAttribute(itemE, "name", item.getName() );
			xml::saveAttribute(itemE, "description", item.getDescription() );
			xml::saveAttribute(itemE, "cursor", item.getCursorName() );
			xml::saveAttribute(itemE, "animation", item.getAnimation()->getFileName() );
			xml::saveAttribute(itemE, "onExamine", item.getOnExamine() );
			xml::saveAttribute(itemE, "onUse", item.getOnUse() );
			xml::saveAttribute(itemE, "effect", ( unsigned char )item.getEffect() );
			xml::saveAttribute(itemE, "lightable", item.isLightable() );
		}

		// lights
		TiXmlElement* lightsE = new TiXmlElement("lights");
		roomElement->LinkEndChild(lightsE);

		for( Lights::const_iterator i = lights_.begin(); i != lights_.end(); ++i ) {
			const LightSource& light = *i;

			TiXmlElement* lightE = new TiXmlElement("light");
			lightsE->LinkEndChild(lightE);

			xml::saveAttribute(lightE, "x", light.x );
			xml::saveAttribute(lightE, "y", light.y );
			xml::saveAttribute(lightE, "intensity", (int)light.intensity );
			xml::saveAttribute(lightE, "active", light.active );
		}
	} catch( std::exception& ex ) {
		LOG_ERROR( "Error saving room " << fileName << ": " << ex.what() );
		throw ;
	}

	document.SaveFile(fileName.c_str());
}

void GameRoom::setBoxActive( unsigned char boxIndex, bool active ) {
	if( /*boxIndex >= 0 &&*/ boxIndex < boxes_.size() ) {
		boxes_[boxIndex].active = active;
	}
}

void GameRoom::updateZMapIndecies() {
	minZPlaneIndex_ = std::numeric_limits<int>::max();
	maxZPlaneIndex_ = std::numeric_limits<int>::min();

	for( auto it = zmap_.begin(); it != zmap_.end(); ++it ) {
		minZPlaneIndex_ = std::min( minZPlaneIndex_, it->first );
		maxZPlaneIndex_ = std::max( minZPlaneIndex_, it->first );
	}
}

const AnimatedSprites* GameRoom::getLayer(int z) const {
	auto it = zmap_.find(z);
	return it == zmap_.end() ? nullptr : &it->second;
}

}
