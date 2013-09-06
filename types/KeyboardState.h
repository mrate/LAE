#pragma once
#ifndef _KEYBOARDSTATE_H_
#define _KEYBOARDSTATE_H_

namespace LAE {

struct SKeyboardState {
	bool	keyDown[256];
	bool	keyPressed[256];
};

}

#endif // _KEYBOARDSTATE_H_
