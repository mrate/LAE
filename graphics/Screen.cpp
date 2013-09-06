#include "Screen.h"

// LAE
#include "../cache/ImageCache.h"
#include "../game/Global.h"
#include "opengl.h"
// stl
#include <cmath>

namespace LAE {

Screen::Screen( unsigned short width, unsigned short height, unsigned char bpp, bool fulscreen )
	: screenWidth_( width )
	, screenHeight_( height )
	, screenBpp_( bpp )
	, lightsCount_( 0 )
	, textBase_( 0 ) {
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0 );
	glDisable( GL_DEPTH_TEST );
	//glDepthFunc(GL_LEQUAL);
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_TEXTURE_2D );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1, 1 );

	glMatrixMode( GL_MODELVIEW );

	// inicializace svetel
	memset( lights_, 0, sizeof( lights_ ) );

	// inicializace pisma
	//initFonts();
}

Screen::~Screen() {
	glDeleteLists( textBase_, 100 );
}

void Screen::setEffect( EffectType effect ) {
	switch( effect ) {
	case EFFECT_NONE:
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		break;
	case EFFECT_LIGHT_RAY:
		glBlendFunc( GL_DST_COLOR, GL_SRC_ALPHA );
		break;
	}
}

/*
void Screen::initFonts()
{
	const Font &font = Global::i()->getFont();
	const Image *fontImage = ImageCache::getObject(font.imageName);
	textBase = glGenLists(100);
	int numOfChars = 100;
	float cx, cy, cw, ch;
	float w = 10 * font.charWidth;
	float h = 10 * font.charHeight;

	glBindTexture(GL_TEXTURE_2D, fontImage->texture);
	for(int i=0;i<numOfChars;++i)
	{
		cy = 0.1f * (int)(i/10);
		ch = cy + font.charHeight;
		cx = font.left[i] / w;
		cw = font.right[i] / w;
		glNewList(textBase+i,GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(cx, cy); glVertex2i(0, 0);
				glTexCoord2f(cx, cy); glVertex2i(font.right[i] - font.left[i], 0);
				glTexCoord2f(cx, cy); glVertex2i(font.right[i] - font.left[i], font.charHeight);
				glTexCoord2f(cx, cy); glVertex2i(0, font.charHeight);
			glEnd();
			glTranslated(font.right[i] - font.left[i], 0, 0);
		glEndList();
	}
}
*/

void Screen::drawImage( const Image* img, int x, int y, int width, int height, unsigned char alpha, short angle ) {
	int dx = x, dy = y, dw = width, dh = height;
	if( dx + dw < 0 || dy + dh < 0 || dx > screenWidth_ || dy > screenHeight_ ) {
		return;
	}

	unsigned char l[4];
	calcLights( l, dx, dy, dx + dw, dy + dh );

	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, img->texture_ );
	glBegin( GL_QUADS );
		glColor4ub( l[0], l[0], l[0], alpha );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3d( dx, dy, 1 );
		glColor4ub( l[1], l[1], l[1], alpha );
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3d( dx + dw, dy, 1 );
		glColor4ub( l[2], l[2], l[2], alpha );
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3d( dx + dw, dy + dh, 1 );
		glColor4ub( l[3], l[3], l[3], alpha );
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3d( dx, dy + dh, 1 );
	glEnd();
}

void Screen::drawScaleImage( const Image* img, int x, int y, int width, int height, unsigned char alpha ) {
	int dx = x, dy = y, dw = img->width(), dh = img->height();
	if( dx + dw < 0 || dy + dh < 0 || dx > screenWidth_ || dy > screenHeight_ ) {
		return;
	}

	unsigned char l[4];
	calcLights( l, x, y, x + width, y + height );

	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, img->texture_ );
	glBegin( GL_QUADS );
		glColor4ub( l[0], l[0], l[0], alpha );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3d( x, y, 1 );
		glColor4ub( l[1], l[1], l[1], alpha );
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3d( x + width, y, 1 );
		glColor4ub( l[2], l[2], l[2], alpha );
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3d( x + width, y + height, 1 );
		glColor4ub( l[3], l[3], l[3], alpha );
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3d( x, y + height, 1 );
	glEnd();
}

void Screen::drawText( const std::string& text, const Font& font, double x, double y, const Color& color /*= Color()*/ ) {
	int textWidth = font.getWidth( text );
	float cx, cy, cw, ch;
	float w = static_cast<float>( font.charCols_ * font.charWidth_ );
	float h = static_cast<float>( font.charRows_ * font.charHeight_ );

	if( x < 0 ) { x = 0; }
	if( x + textWidth > screenWidth_ ) {
		x = screenWidth_ - textWidth;
	}
	if( y < 0 ) { y = 0; }
	if( y + font.charHeight_ > screenHeight_ ) {
		y = screenHeight_ - font.charHeight_;
	}

	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, font.getImage()->texture_ );
	glTranslated( x, y, 0 );

	for( size_t pos = 0; pos < text.size(); ++pos ) {
		if( text[pos] == ' ' ) {
			glTranslated( font.charWidth_ / 2, 0, 0 );
			continue;
		}

		unsigned char index = static_cast<unsigned char>(text[pos]);
		int i = font.charMap_[index];

		int charX = i % font.charCols_;
		int charY = i / font.charCols_;

		int chL = font.left_[i];
		int chR = font.right_[i];
		int charW = chR - chL;

		cx = (float)((charX * font.charWidth_) + chL) / w;
		cw = (float)((charX * font.charWidth_) + chR) / w;
		cy = (float)(charY * font.charHeight_) / h;
		ch = (float)((charY+1)*font.charHeight_) / h;

		glBegin( GL_QUADS );
			glColor4ub( color.r, color.g, color.b, color.a );
			glTexCoord2f( cx, cy );
			glVertex2i( 0, 0 );
			glTexCoord2f( cw, cy );
			glVertex2i( charW, 0 );
			glTexCoord2f( cw, ch );
			glVertex2i( charW, font.charHeight_ );
			glTexCoord2f( cx, ch );
			glVertex2i( 0, font.charHeight_ );
		glEnd();
		glTranslated( charW, 0, 0 );
	}

	/*
	glTranslated(x, y, 0);
	glListBase(textBase-33);
	glCallLists(text.size(),GL_BYTE,text.c_str());
	*/

	//textout_ex(bbuffer, font, text.c_str(), x, y, makecol(255, 255, 255), -1);
	/*
	BITMAP *bmp = ImageCache::getObject(font.imageName)->bmp;
	int lx = x;

	int sx, sy;
	int ch;
	for(int i=0,n=text.length();i<n;i++)
	{
		if(text[i] == ' ')
		{
			lx += font.charWidth / 2;
			continue;
		}

		ch = ((int)text[i]) - 33;
		sx = font.charWidth * (ch % 10);
		sy = font.charHeight * (ch / 10);

		masked_blit(bmp, bbuffer, sx + font.left[ch], sy, lx, y, font.right[ch] - font.left[ch], font.charHeight);
		lx += font.right[ch] - font.left[ch];
	}
	*/
}

void Screen::flip() {
}

void Screen::drawLine( int x1, int y1, int x2, int y2, const Color& color /*= Color()*/ ) {
	glDisable( GL_TEXTURE_2D );
	glLoadIdentity();
	glBegin( GL_LINES );
		glColor4ub( color.r, color.g, color.b, color.a );
		glVertex3d( x1, y1, 1 );
		glVertex3d( x2, y2, 1 );
	glEnd();
	glEnable( GL_TEXTURE_2D );
}

void Screen::drawLine( double x1, double y1, double x2, double y2, const Color& color /*= Color()*/ ) {
	glDisable( GL_TEXTURE_2D );
	glLoadIdentity();
	glBegin( GL_LINES );
		glColor4ub( color.r, color.g, color.b, color.a );
		glVertex3d( x1, y1, 1 );
		glVertex3d( x2, y2, 1 );
	glEnd();
	glEnable( GL_TEXTURE_2D );
}

void Screen::drawRectangle( int x1, int y1, int x2, int y2, const Color& color /*= Color()*/ ) {
	glDisable( GL_TEXTURE_2D );
	glLoadIdentity();
	glBegin( GL_LINE_LOOP );
		glColor4ub( color.r, color.g, color.b, color.a );
		glVertex3d( x1, y1, 1 );
		glVertex3d( x1, y2, 1 );
		glVertex3d( x2, y2, 1 );
		glVertex3d( x2, y1, 1 );
	glEnd();
	glEnable( GL_TEXTURE_2D );
}

void Screen::drawFillPoly( const Point* p, int count, const Color& color /*= Color()*/ ) {
	glDisable( GL_TEXTURE_2D );
	glLoadIdentity();
	glBegin( GL_POLYGON );
		glColor4ub( color.r, color.g, color.b, color.a );
		for( int i = 0; i < count; ++i ) {
			glVertex3d( p[i].x, p[i].y, 1 );
		}
	glEnd();
	glEnable( GL_TEXTURE_2D );
}

void Screen::drawPoly( const Point* p, int count, const Color& color /*= Color()*/ ) {
	glDisable( GL_TEXTURE_2D );
	glLoadIdentity();
	glBegin( GL_LINE_LOOP );
		glColor4ub( color.r, color.g, color.b, color.a );
		for( int i = 0; i < count; ++i ) {
			glVertex3d( p[i].x, p[i].y, 1 );
		}
		glEnd();
	glEnable( GL_TEXTURE_2D );
}

void Screen::fillRectangle( int x1, int y1, int x2, int y2, const Color& color /*= Color()*/ ) {
	glDisable( GL_TEXTURE_2D );
	glLoadIdentity();
	glBegin( GL_QUADS );
		glColor4ub( color.r, color.g, color.b, color.a );
		glVertex3d( x1, y1, 1 );
		glVertex3d( x2, y1, 1 );
		glVertex3d( x2, y2, 1 );
		glVertex3d( x1, y2, 1 );
	glEnd();
	glEnable( GL_TEXTURE_2D );
}

void Screen::enableLight( unsigned char index, LightSource* ls ) {
	if( lights_[index] == 0 ) {
		++lightsCount_;
	}
	lights_[index] = ls;
}

void Screen::disableLight( unsigned char index ) {
	if( lights_[index] ) {
		--lightsCount_;
	}
	lights_[index] = 0;
}

void Screen::calcLights( unsigned char* l, int x1, int y1, int x2, int y2 ) {
	if( lightsCount_ == 0 ) {
		l[0] = l[1] = l[2] = l[3] = 255;
		return ;
	}

	float koef, prirustek;
	float dist[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Point p[4] = { Point( x1, y1 ), Point( x2, y1 ), Point( x2, y2 ), Point( x1, y2 ) };

	for( int j = 0; j < MAX_LIGHTS; ++j ) {
		if( lights_[j] ) {
			koef = 255.0f / ( ( float )lights_[j]->intensity * 6.0f );
			for( int i = 0; i < 4; i++ ) {
				prirustek = 255.0f - koef * std::sqrt( std::pow( ( float )p[i].x - lights_[j]->x, 2 ) + std::pow( ( float )p[i].y - lights_[j]->y, 2 ) );
				if( prirustek > 0 ) {
					dist[i] += prirustek;
				}
			}
		}
	}

	for( int i = 0; i < 4; ++i ) {
		if( dist[i] > 255 ) {
			l[i] = 255;
		} else if( dist[i] < 0 ) {
			l[i] = 0;
		} else {
			l[i] = ( unsigned char )dist[i];
		}
	}
}

void Screen::drawSprite( const Sprite& sprite, unsigned char alpha, short angle ) {
	drawImage( sprite.getImage().get(), (int)sprite.getX(), (int)sprite.getY(), alpha, angle );
}

void Screen::drawSprite( const Sprite& sprite, int x, int y, unsigned char alpha, short angle ) {
	drawImage( sprite.getImage().get(), (int)(x + sprite.getX()), (int)(y + sprite.getY()), alpha, angle );
}

}
