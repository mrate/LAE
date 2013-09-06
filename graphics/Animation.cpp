// LAE
#include "Animation.h"
#include "../logger/Logger.h"
#include "../types/types.h"
#include "../utils/utils.h"
#include "../cache/ImageCache.h"
#include "../cache/SoundCache.h"
#include "../xml/Xml.h"
// stl
#include <fstream>
// xml
#include <tinyxml.h>

namespace LAE {

Animation::Animation()
	: length_( 0 ) {
}

Animation::~Animation() {
	free();
}

/*
void Animation::loadBin( const std::string& fname ) {
	free();

	CFile fr( fname.c_str(), CFile::READ );
	unsigned char nframes = fr.readUChar();
	for( int i = 0; i < nframes; ++i ) {
		frames.push_back( AnimationFrame() );
		AnimationFrame& frame = frames.back();

		frame.delay = fr.readShort();
		length_ += frame.delay;

		std::string sampleName;
		fr.readString( sampleName );
		if( !sampleName.empty() ) {
			LOG_DEBUG( "  sound: " << sampleName );
			frame.sample = SoundManager::i()->createObject( sampleName );
		} else {
			//frame.sample.reset();
		}

		unsigned char sprites = fr.readUChar();
		//debug_cout("    " << i << ") delay: " << frame.delay << ", sprites: " << sprites << endl);
		for( int j = 0; j < sprites; ++j ) {
			frame.sprites.push_back( Sprite() );
			Sprite& sprite = frame.sprites.back();

			std::string imageName;
			fr.readString( imageName );
			sprite.setImage( ImageManager::i()->createObject( imageName ) );
			sprite.setX( fr.readShort() );
			sprite.setY( fr.readShort() );

			//debug_cout("      " << j << ") " << sprite.image << ", x=" << sprite.x << ", y=" << sprite.y << endl);
		}
	}
	fileName_ = fname;
}

void Animation::saveBin( const std::string& fname ) const {
	CFile fr( fname.c_str(), CFile::WRITE );
	fr.writeUChar( ( unsigned char )frames.size() );
	for( AnimationFrames::const_iterator it = frames.begin(); it != frames.end(); ++it ) {
		const AnimationFrame& frame = *it;

		fr.writeShort( frame.delay );
		if( frame.sample != 0 ) {
			fr.writeString( frame.sample->getFileName() );
		} else {
			fr.writeUChar( 0 );
		}

		fr.writeUChar( ( unsigned char )frame.sprites.size() );
		for( Sprites::const_iterator j = frame.sprites.begin(); j != frame.sprites.end(); ++j ) {
			const Sprite& sprite = *j;

			fr.writeString( sprite.getImage()->getFileName() );
			fr.writeShort( (short)sprite.getX() );
			fr.writeShort( (short)sprite.getY() );
		}
	}
}
*/

void Animation::free() {
	length_ = 0;
	fileName_.clear();
	frames.clear();
}

int Animation::getFrameIndex( unsigned long time, bool loop ) const {
	if( loop ) {
		if( length_ == 0 ) {
			time = 0;
		} else
			time %= length_;

			//while( time > length_ ) {
			//	time -= length_;
			//}
	}

	int frameIndex = 0;
	if( time < length_ ) {
		unsigned long frameTime = 0;
		for( auto i = frames.begin(); i != frames.end(); i++ ) {
			const AnimationFrame& frame = *i;
			frameTime += frame.delay;
			if( frameTime > time ) {
				break;
			}
			++frameIndex;
		}
		return frameIndex;
	} else {
		return frames.size() - 1;
	}
}

void Animation::renderFrame( Screen& scr, int x, int y, unsigned long time, bool loop, unsigned char alpha ) const {
	if( frames.empty() ) {
		return ;
	}

	int frameIndex = getFrameIndex( time, loop );
	const Sprites& sprites = frames[frameIndex].sprites;
	for( auto i = sprites.begin(); i != sprites.end(); i++ ) {
		scr.drawSprite( *i, x, y, alpha );
	}
}

void Animation::renderFrame( Screen& scr, int x, int y, unsigned long index, unsigned char alpha ) const {
	if( frames.size() <= index /*|| index < 0*/ ) {
		return ;
	}

	const Sprites& sprites = frames[index].sprites;
	for( auto i = sprites.begin(); i != sprites.end(); i++ ) {
		scr.drawSprite( *i, x, y, alpha );
	}
}

void Animation::renderFrameScaled( Screen& scr, int x, int y, unsigned long time, bool loop, unsigned char scale, unsigned char alpha ) const {
	if( frames.empty() ) {
		return ;
	}

	int frameIndex = getFrameIndex( time, loop );
	const Sprites& sprites = frames[frameIndex].sprites;
	float dstX, dstY, dstWidth, dstHeight;
	float scl = ( float )scale / 255.0f;
	for( auto i = sprites.begin(); i != sprites.end(); i++ ) {
		const Sprite& sprite = *i;
		dstX = x + ( float )sprite.getX() * scl;
		dstY = y + ( float )sprite.getY() * scl;
		dstWidth = ( float )sprite.getImage()->width() * scl;
		dstHeight = ( float )sprite.getImage()->height() * scl;
		scr.drawScaleImage( sprite.getImage().get(), ( int )dstX, ( int )dstY, ( int )dstWidth, ( int )dstHeight, alpha );
	}
}

void Animation::init() {
	frames.clear();
}

void Animation::addFrame( const AnimationFrame& frame, int index ) {
	if( index >= static_cast<int>( frames.size() ) ) {
		index = -1;
	}
	if( index < 0 ) {
		frames.push_back( frame );
	} else {
		int i = 0;
		for( AnimationFrames::iterator it = frames.begin(); it != frames.end(); ++it ) {
			if( i++ == index ) {
				frames.insert( it, frame );
				return ;
			}
		}
	}
}

void Animation::removeFrame( int index ) {
	auto it = frames.begin();
	std::advance( it, index );
	if( it != frames.end() ) {
		frames.erase( it );
	}
}

void Animation::load( const std::string& fname ) {
	LOG_DEBUG( "..loading animation " << fname );
	free();

	if( fname.rfind(".png") == fname.size() - 4 ) {
		loadFromImage( fname );
	} else {
		loadFromXml( fname );
	}

	fileName_ = fname;
}

void Animation::save( const std::string& fname ) const {
	TiXmlDocument document;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );

	TiXmlElement* animElement = new TiXmlElement( "animation" );
	document.LinkEndChild(decl);
	document.LinkEndChild(animElement);

	for( AnimationFrames::const_iterator it = frames.begin(); it != frames.end(); ++it ) {
		const AnimationFrame& frame = *it;

		TiXmlElement* frameElement = new TiXmlElement("frame");
		frameElement->SetAttribute( "delay", frame.delay );
		animElement->LinkEndChild( frameElement );

		if( frame.sample ) {
			frameElement->SetAttribute( "sample", frame.sample->getFileName().c_str() );
		}

		TiXmlElement* spritesE = new TiXmlElement("sprites");
		frameElement->LinkEndChild(spritesE);
		for( auto j = frame.sprites.begin(); j != frame.sprites.end(); ++j ) {
			const Sprite& sprite = *j;

			TiXmlElement* spriteElement = new TiXmlElement("sprite");
			spriteElement->SetAttribute( "image", sprite.getImage()->getFileName().c_str() );
			spritesE->LinkEndChild( spriteElement );

			xml::saveAttribute(spriteElement, "x", sprite.getX() );
			xml::saveAttribute(spriteElement, "y", sprite.getY() );
		}
	}

	document.SaveFile(fname.c_str());
}

void Animation::loadFromXml( const std::string& fname ) {
	free();

	TiXmlDocument document;
	if( !document.LoadFile( fname.c_str() ) ) {
		throw LAEException("Error loading animation ") << fname;
	}

	TiXmlElement* animElement = document.RootElement();
	for( TiXmlElement* frameElement = animElement->FirstChildElement("frame"); frameElement; frameElement = frameElement->NextSiblingElement("frame") ) {
		frames.push_back( AnimationFrame() );
		AnimationFrame& frame = frames.back();

		frame.delay = boost::lexical_cast<unsigned short>( frameElement->Attribute("delay") );
		length_ += frame.delay;
		if( frameElement->Attribute("sample") ) {
			std::string sampleName = frameElement->Attribute("sample");
			frame.sample = SoundManager::i()->createObject( sampleName );
		} else {
			//frame.sample.reset();
		}

		TiXmlElement* spritesE = frameElement->FirstChildElement("sprites");
		if( spritesE ) {
			for( TiXmlElement* spriteE = spritesE->FirstChildElement("sprite"); spriteE; spriteE = spriteE->NextSiblingElement("sprite") ) {
				frame.sprites.push_back( Sprite() );
				Sprite& sprite = frame.sprites.back();

				std::string fileName = spriteE->Attribute("image");
				sprite.setImage( ImageManager::i()->createObject( fileName ) );
				sprite.setX( xml::loadAttribute<double>( spriteE, "x" ) );
				sprite.setY( xml::loadAttribute<double>( spriteE, "y" ) );
			}
		}
	}

	fileName_ = fname;
}

void Animation::loadFromImage( const std::string& fname ) {
	free();

	Sprite singleSprite;
	singleSprite.setImage( ImageManager::i()->createObject(fname) );

	AnimationFrame singleFrame;
	singleFrame.delay = 0;
	singleFrame.sprites.push_back(singleSprite);

	length_ = 0;
	frames.push_back(singleFrame);

	fileName_ = fname;
}

bool Animation::hasFrameSample( int frame ) const {
	return frames[frame].sample.get() != nullptr;
}

}
