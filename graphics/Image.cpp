#include "Image.h"
#include "../logger/Logger.h"
#include "../exception/Exception.h"

#define PNGSIGSIZE	8

// stl
#include <fstream>

namespace LAE {

void Image::Readfunc( png_structp pngPtr, png_bytep data, png_size_t length ) {
	std::ifstream* str = ( std::ifstream* )png_get_io_ptr( pngPtr );
	str->read( ( char* )data, length );
}

Image::Image()
	: texture_( 0 )
	, imgWidth_( 0 )
	, imgHeight_( 0 )
	, loaded_( false ) {
}

Image::~Image() {
	free();
}

void Image::load( const std::string& name ) {
	LOG_DEBUG("..loading image " << name);

	free();

	png_byte pngsig[PNGSIGSIZE];

	int is_png = 0;
	int bitdepth, channels;

	std::ifstream str;
	str.open( name.c_str(), std::ios_base::in | std::ios_base::binary );
	if( !str.is_open() ) {
		throw LAEException( "Failed to open image file '" ) << name << "' - file not found";
	}

	str.read( ( char* )pngsig, PNGSIGSIZE );

	is_png = !png_sig_cmp( pngsig, 0, PNGSIGSIZE );
	if( !is_png ) {
		throw LAEException( "Failed to open image file '" ) << name << "' - invalid format";
	}

	png_structp pngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	png_infop infoPtr = png_create_info_struct( pngPtr );

	png_set_read_fn( pngPtr, &str, Image::Readfunc );
	png_set_sig_bytes( pngPtr, PNGSIGSIZE );
	png_read_info( pngPtr, infoPtr );

	imgWidth_ = infoPtr->width;
	imgHeight_ = infoPtr->height;
	bitdepth = infoPtr->bit_depth;
	channels = infoPtr->channels;

	const unsigned int stride = imgWidth_ * bitdepth * channels / 8;
	png_bytep* rowPtrs = new png_bytep[imgHeight_];
	const char* imgData = new char[stride * imgHeight_];

	for( int i = 0; i < imgHeight_; i++ ) {
		rowPtrs[i] = ( png_bytep )imgData + ( i * stride );
	}

	png_read_image( pngPtr, rowPtrs );
	str.close();

	glGenTextures( 1, &texture_ );
	glBindTexture( GL_TEXTURE_2D, texture_ );
	glTexImage2D( GL_TEXTURE_2D, 0, 4, imgWidth_, imgHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	loaded_ = true;
	fileName_ = name;
}

void Image::free() {
	if( loaded_ ) {
		glDeleteTextures( 1, &texture_ );
	}
	loaded_ = false;
	fileName_ = "";
}

}
