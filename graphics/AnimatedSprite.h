#pragma once
#ifndef _ANIMATEDSPRITE_H_
#define _ANIMATEDSPRITE_H_

// LAE
#include "AnimationPlayer.h"
#include "Screen.h"
#include "PAnimation.h"
#include "Renderable.h"
// stl
#include <string>

namespace LAE {

/// Single sprite with animation
///  It has it's animation and position
class AnimatedSprite : public Renderable {
public:
	AnimatedSprite();
	virtual ~AnimatedSprite();

	/// Sets new x position
	void setX( short x ) { x_ = x; }
	/// Returns x position
	short getX() const { return x_; }

	/// Sets new y position
	void setY( short y ) { y_ = y; }
	/// Returns y position
	short getY() const { return y_; }

	/// Sets new z position
	void setZ( short z ) { z_ = z; }
	/// Returns z position
	short getZ() const { return z_; }

	/// set item visible
	void setVisible( bool visible ) { visible_ = visible; }

	/// Sets new animation
	void setAnimation( PAnimation animation, bool loop );
	/// Sets new animation
	void setAnimation( PAnimation animation );
	/// Sets new animation
	void setAnimation( const std::string& animation, bool loop );
	/// Sets new animation
	void setAnimation( const std::string& animation );
	/// Returns current animation
	PAnimation getAnimation() const { return anim_.getAnimation(); }

	/// Update sprite
	virtual void update( unsigned long time );
	/// Render sprite with offsets dx and dy
	virtual void render( Screen& scr, int dx, int dy, unsigned char alpha = 255 ) const;
	
	/// Returns current animation player
	const AnimationPlayer& getAnimationPlayer() const { return anim_; }

	/// Returns effect
	Screen::EffectType getEffect() const { return effect_; }
	/// Sets new render effect
	void setEffect( Screen::EffectType effect ) { effect_ = effect; }

	/// Returs flag if sprite is affected by light system while rendering
	bool isLightable() const { return lightable_; }
	/// Sets flag if sprite is affected by light system while rendering
	void setLightable( bool lightable ) { lightable_ = lightable; }

private:
	short x_;	///< x-ova pozice spritu
	short y_;	///< y-ova pozice spritu
	short z_;	///< z-ova pozice spritu

	AnimationPlayer anim_;
	Screen::EffectType effect_;
	bool lightable_;
	bool visible_;
};

/// Bidirectional queue of animated sprites
typedef std::deque<AnimatedSprite*>		AnimatedSprites;
/// Vector of animated sprites
//typedef std::vector<AnimatedSprite>		AnimatedSpritesVector;

}

#endif // _ANIMATEDSPRITE_H_
