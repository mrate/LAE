#pragma once

// fmod
#include <fmod.hpp>
// stl
#include <string>
// boost
#include <boost/shared_ptr.hpp>

namespace LAE {

/// Wrapper zvuku knihovny FMOD
class Sound {
public:
	Sound();
	virtual ~Sound();

	void load( const std::string& fname );
	void free();
	const std::string& getFileName() const { return fileName_; }

private:
	mutable FMOD::Sound* sample_;
	std::string fileName_;

	friend class MediaPlayer;	///< pro pristup k samplu
};

typedef boost::shared_ptr<const Sound> PSound;

}
