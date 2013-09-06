#pragma once
#ifndef _ANIMATION_H_
#define _ANIMATION_H_

// LAE
#include "Screen.h"
#include "../types/types.h"
#include "../audio/Sound.h"
#include "../audio/PSound.h"
// stl
#include <string>
// boost
#include <boost/shared_ptr.hpp>

namespace LAE {

/// Single animation frame
struct AnimationFrame {
	PSound sample;			///< sample sound
	unsigned short delay;	///< delay in ms
	Sprites sprites;		///< list of sprites
};
/// Vector of animation frames
typedef std::vector<AnimationFrame> AnimationFrames;

/// Definition of animation
class Animation {
public:
	Animation();
	virtual ~Animation();

	/// Load animation 
	//void loadBin( const std::string& fname );
	//void saveBin( const std::string& fname ) const;

	/// Loads animation from given file name
	void load( const std::string& fname );
	/// Saves animation to given file
	void save( const std::string& fname ) const;
	
	/// Release all resources for animation
	void free();
	/// Render single frame by given frame index
	void renderFrame( Screen& scr, int x, int y, unsigned long index, unsigned char alpha = 255 ) const;
	/// Render single frame by given time (and loop flag)
	void renderFrame( Screen& scr, int x, int y, unsigned long time, bool loop, unsigned char alpha = 255 ) const;
	void renderFrameScaled( Screen& scr, int x, int y, unsigned long time, bool loop, unsigned char scale, unsigned char alpha = 255 ) const;

	/// Returns index of frame at specified time
	int getFrameIndex( unsigned long time, bool loop ) const;

	/// Returns true if sample with given index has sound sample attached to it
	bool hasFrameSample( int frameIndex ) const;

	/// Initialization
	void init();

	/// Inserts new frame to given index ( if index is -1, the frame will be added to the end )
	void addFrame( const AnimationFrame& frame, int frameIndex = -1 );
	/// Remove frame from given index
	void removeFrame( int frameIndex );

	/// Returns time length of animation [ms]
	unsigned long getLength() const { return length_; }
	/// Returns pointer to sound sample on given index
	const Sound* getFrameSample( int frameIndex ) const { return frames[frameIndex].sample.get(); }
	/// Returns frame on given index
	AnimationFrame& getFrame( int frameIndex ) { return frames[frameIndex]; }
	/// Returns count of frames in animation
	size_t getFrameCount() const { return frames.size(); }

	/// Returns name of file from which animation was loaded
	const std::string& getFileName() const { return fileName_; }

private:
	unsigned long length_;	///< time length of animation [ms]
	AnimationFrames frames;	///< vector of frames
	std::string fileName_;	///< file name

	/// Loads animation from xml file
	void loadFromXml( const std::string& fname );
	/// Loads animation from single image file
	void loadFromImage( const std::string& fname );
};

}

#endif // _ANIMATION_H_
