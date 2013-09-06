#pragma once
#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

namespace LAE {

class Screen;

class Renderable {
public:
	Renderable() {}
	virtual ~Renderable() {}

	virtual void update( unsigned long time ) = 0;
	virtual void render( Screen& scr, int dx, int dy, unsigned char alpha ) const = 0;
};

}

#endif // _RENDERABLE_H_
