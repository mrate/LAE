#pragma once
#ifndef _OPEN_GL_SCREEN_H_
#define _OPEN_GL_SCREEN_H_

// LAE
#include "Image.h"
#include "Font.h"
#include "graphics.h"
#include "Color.h"
#include "../types/types.h"
// stl
#include <string>

#define MAX_LIGHTS		16

namespace LAE {

class Screen {
public:
	enum EffectType {
	    EFFECT_NONE = 0,
	    EFFECT_LIGHT_RAY = 1
	};

	Screen( unsigned short width, unsigned short height, unsigned char bpp, bool fulscreen );
	virtual ~Screen();

	void clear() {
		glClear( GL_COLOR_BUFFER_BIT );
	}

	void setEffect( EffectType effect );

	void drawSprite( const Sprite& sprite, unsigned char alpha = 255, short angle = 0 );
	void drawSprite( const Sprite& sprite, int x, int y, unsigned char alpha = 255, short angle = 0 );

	void drawImage( const Image* img, int x, int y, unsigned char alpha = 255, short angle = 0 ) {
		drawImage( img, x, y, img->width(), img->height(), alpha, angle );
	}

	void drawImage( const Image* img, int x, int y, int width, int height, unsigned char alpha = 255, short angle = 0 );
	void drawScaleImage( const Image* img, int x, int y, int width, int height, unsigned char alpha = 255 );

	void drawText( const std::string& text, const Font& font, double x, double y, const Color& color = Color() );

	void drawLine( const Point& p0, const Point& p1, const Color& color = Color() ) {
		drawLine( p0.x, p0.y, p1.x, p1.y, color );
	}
	void drawLine( const Segment& segment, const Color& color = Color() ) {
		drawLine( segment.p0.x, segment.p0.y, segment.p1.x, segment.p1.y, color );
	}
	void drawLine( int x1, int y1, int x2, int y2, const Color& color = Color() );
	void drawLine( double x1, double y1, double x2, double y2, const Color& color = Color() );
	void drawRectangle( int x1, int y1, int x2, int y2, const Color& color = Color() );
	void fillRectangle( int x1, int y1, int x2, int y2, const Color& color = Color() );

	void drawFillPoly( const Point* p, int count, const Color& color = Color() );
	void drawPoly( const Point* p, int count, const Color& color = Color() );

	void enableLight( unsigned char index, LightSource* ls );
	void disableLight( unsigned char index );

	void flip();

private:
	unsigned short screenWidth_;
	unsigned short screenHeight_;
	unsigned char screenBpp_;
	unsigned char lightsCount_;

	GLuint textBase_;

	LightSource*	lights_[MAX_LIGHTS];
	
	void calcLights( unsigned char* l, int x1, int y1, int x2, int y2 );
	//	void initFonts();
};
}

#endif // _OPEN_GL_SCREEN_H_
