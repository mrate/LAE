//------------------------------------------------------------------
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from it's use.
// Users of this code must verify correctness for their application.
//------------------------------------------------------------------
#pragma once

#include <iostream>

namespace LAE {
// Error codes
enum Error {
    Enot,	// no error
    Edim,	// error: dim of point invalid for operation
    Esum	// error: sum not affine (cooefs add to 1)
};
}
