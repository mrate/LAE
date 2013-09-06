#pragma once
#ifndef _MOUSESTATE_H_
#define _MOUSESTATE_H_

namespace LAE {

struct SMouseState {
	short x;
	short y;
	bool lbutton;
	bool rbutton;
	bool lbutton_pressed;
	bool rbutton_pressed;
};

}

#endif // _MOUSESTATE_H_
