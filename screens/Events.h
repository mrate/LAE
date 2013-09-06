#pragma once

// boost
#include <boost/shared_ptr.hpp>
// stl
#include <string>

#include "../objects/Actor.h"

namespace LAE {
class Actor;
class Item;
class SoundChannel;

/// zakladni trida eventu
struct SBaseWaitEvent {
	SBaseWaitEvent( unsigned long id ) : id( id ) {}
	virtual ~SBaseWaitEvent() {}
	enum { ID  = 0 };

	const unsigned long id;

	virtual void update( long timeMili ) {}
	virtual bool raised() const {
		return true;
	}
};

typedef boost::shared_ptr<SBaseWaitEvent> TWaitEvent;

/// udalost cekani na stav postav
struct SWaitActorEvent : public SBaseWaitEvent {
	SWaitActorEvent( const Actor* actor, Actor::EActorState state ) : SBaseWaitEvent( ID ), actor( actor ), state( state ) {}
	enum { ID  = 1 };

	const Actor* actor;		///< cilova postava
	Actor::EActorState state;		///< pozadovany stav

	//inline virtual bool raised() const
	//{
	//	return actor->state == state;
	//}
};

/// udalost cekani na timer
struct SWaitTimerEvent : public SBaseWaitEvent {
	SWaitTimerEvent( unsigned long timeToWait ) : SBaseWaitEvent( ID ), timeToWait( timeToWait ) {}
	enum { ID = 2 };

	unsigned long timeToWait;	///< cas timeru

	//inline virtual void update(long timeMili)
	//{
	//	if(timeToWait < timeMili) timeToWait = 0;
	//	else timeToWait -= timeMili;
	//}

	//inline virtual bool raised() const
	//{
	//	return timeToWait == 0;
	//}
};

/// udalost mluveni
struct SWaitSpeechEvent : public SBaseWaitEvent {
	SWaitSpeechEvent( Actor* actor, unsigned long timeToWait, SoundChannel* soundChannel = nullptr ) 
		: SBaseWaitEvent( ID )
		, actor( actor )
		, timeToWait( timeToWait )
		, soundChannel( soundChannel ) 
	{}
	enum { ID = 3 };

	Actor* actor;				///< postava, ktera mluvi
	unsigned long timeToWait;	///< cas mluveni
	SoundChannel* soundChannel;	///< kanal zvuku

	//inline virtual void update(long timeMili)
	//{
	//	if(timeToWait < timeMili)
	//		timeToWait = 0;
	//	else
	//		timeToWait -= timeMili;

	//}

	//inline virtual bool raised() const
	//{
	//	bool res = false;
	//	if(sound)
	//		res = !MediaPlayer::i()->channelPlaying(soundChannel);
	//	else
	//		res = timeToWait == 0;

	//	// odkliknuti pravou mysi
	//	if(mouse.lbutton_pressed) res = true;
	//	if(res)
	//	{
	//		if(sound) MediaPlayer::i()->stopSound(soundChannel);
	//		hideText();
	//	}
	//}
};

/// udalost animace predmetu
struct SWaitAnimationEvent : public SBaseWaitEvent {
	SWaitAnimationEvent( const AnimationPlayer* player ) : SBaseWaitEvent( ID ), player( player ) {}
	enum { ID = 4 };

	const AnimationPlayer*	player;		///< cilovy predmet

	//virtual bool raised() const
	//{
	//	return !item->isAnimPlaying();
	//}
};

/// cekani na nacteni mistnosti
struct SWaitLoadRoomEvent : public SBaseWaitEvent {
	SWaitLoadRoomEvent( const std::string& fname ) : SBaseWaitEvent( ID ), roomName( fname ) {}
	enum { ID = 5 };

	std::string roomName;	///< nazev souboru s mistnosti
};

/// cekani na dokonceni animace postavy
//struct SWaitActorAnimEvent : public SBaseWaitEvent {
//	SWaitActorAnimEvent() : SBaseWaitEvent( ID ) {}
//	enum { ID = 6 };
//};

/// cekani na kameru
struct SWaitCameraEvent : public SBaseWaitEvent {
	//SWaitCameraEvent(Camera* camera) : SBaseWaitEvent(ID), camera(camera) {}
	SWaitCameraEvent() : SBaseWaitEvent( ID ) {}
	enum { ID = 7 };

	//Camera* camera;
	//virtual bool raised() const
	//{
	//	return camera->isIdle();
	//}
};

/// cekani na dialog
struct SWaitDialogEvent : public SBaseWaitEvent {
	SWaitDialogEvent() : SBaseWaitEvent( ID ) {}
	enum { ID = 8 };
};
}
