#pragma once
#ifndef _PSOUND_H_
#define _PSOUND_H_

#include <boost/shared_ptr.hpp>

namespace LAE {

class Sound;
typedef boost::shared_ptr<const Sound> PSound;

}

#endif // _PSOUND_H_
