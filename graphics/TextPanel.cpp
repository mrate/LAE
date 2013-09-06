// LAE
#include "TextPanel.h"
#include "Screen.h"
#include "Font.h"
#include "../game/Global.h"
#include "../logger/Logger.h"
// stl
#include <cmath>

namespace LAE {

TextPanel::TextPanel()
	: font_( Global::i()->defaultFont() )
	, x_( 0 )
	, y_( 0 )
	, width_( 0 )
	, height_( 0 ) {
}

TextPanel::~TextPanel() {
}

void TextPanel::update( unsigned long millis ) {

}

void TextPanel::render( Screen& screen ) const {
	if( width_ <= 0 || height_ <= 0 ) {
		return ;
	}

	Color bgColor2;
	bgColor2.r = std::max( bgColor_.r - 100, 0 );
	bgColor2.g = std::max( bgColor_.g - 100, 0 );
	bgColor2.b = std::max( bgColor_.b - 100, 0 );
	bgColor2.a = std::max( bgColor_.a - 100, 0 );

	Point pp[4];
	pp[0] = Point( x_, y_ );
	pp[1] = Point( x_ + width_, y_ );
	pp[2] = Point( x_ + width_, y_ + height_ );
	pp[3] = Point( x_, y_ + height_ );

	screen.drawFillPoly( pp, 4, bgColor2 );
	screen.drawPoly( pp, 4, bgColor_ );

	std::string r = stream_.str();
	std::size_t startIndex = 0;
	std::size_t p = 0;
	int lineNr = 0;
	do {
		p = r.find('\n', startIndex);
		if( p != std::string::npos ) {
			std::string line = r.substr( startIndex, p - startIndex );
			screen.drawText( line, *font_, x_, y_ + lineNr * font_->getCharHeight(), fontColor_);
			startIndex = p+1;
			++lineNr;
		}
	} while(p != std::string::npos);
}

void TextPanel::autoResize() {
	width_ = 0;
	height_ = 0;

	std::string r = stream_.str();
	std::size_t startIndex = 0;
	std::size_t p = 0;

	do {
		height_ += font_->getCharHeight();
		p = r.find('\n', startIndex);
		if( p != std::string::npos ) {
			std::string line = r.substr( startIndex, p - startIndex );
			width_ = std::max( width_, font_->getWidth(line) );
			startIndex = p+1;
		}
	} while(p != std::string::npos);
}

void TextPanel::reset() {
	stream_.str( std::string() );
}

}
