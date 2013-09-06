// LAE
#include "LuaRegistrator.h"
#include "../logger/Logger.h"
#include "../audio/MediaPlayer.h"
#include "../audio/Sound.h"
#include "../audio/SoundChannel.h"
#include "../game/Global.h"
#include "../game/GameState.h"
#include "../game/GameEngine.h"
#include "../objects/Actor.h"
#include "../objects/Camera.h"
#include "../objects/Item.h"
#include "../screens/RoomPlayer.h"
#include "../screens/ScriptPlayer.h"
#include "../graphics/Screen.h"
#include "../graphics/Image.h"
#include "../graphics/AnimatedSprite.h"
#include "../graphics/AnimationPlayer.h"
#include "../cache/ImageCache.h"
#include "../cache/SoundCache.h"
#include "../cache/AnimationCache.h"
#include "../exception/Exception.h"
// luabind
#include <luabind/raw_policy.hpp>

namespace LAE {
namespace lua {

//////////////////////////////////////////////////////////////////////////
// registrators

/// prints info message to log
void log_info( const std::string& msg ) { LOG_INFO( msg ); }
/// prints debug message to log
void log_debug( const std::string& msg ) { LOG_DEBUG( msg ); }
/// prints error message to log
void log_error( const std::string& msg ) { LOG_ERROR( msg ); }

luabind::scope registerItem() {
	using namespace luabind;
	return class_<Item>("Item")
		.def(constructor<>())
		.def("setAnimation", (void(AnimatedSprite::*)( PAnimation, bool ) )&AnimatedSprite::setAnimation)
		.def("setAnimation", (void(AnimatedSprite::*)( PAnimation ) )&AnimatedSprite::setAnimation)
		.def("setAnimation", (void(AnimatedSprite::*)( const std::string&, bool ) )&AnimatedSprite::setAnimation)
		.def("setAnimation", (void(AnimatedSprite::*)( const std::string& ) )&AnimatedSprite::setAnimation)
		.def("setActive", &Item::setActive)
		.def("setDescription", &Item::setDescription)
		.def("getAnimation", &AnimatedSprite::getAnimationPlayer)
		.property("x", &Item::getX)
		.property("y", &Item::getY);
}

luabind::scope registerActor() {
	using namespace luabind;
	return class_<Actor>("Actor")
		.property("x", &Actor::getX)
		.property("y", &Actor::getY)
		.property("animation", &Actor::getAnimationPlayer)
		.def("setPosition", (void(Actor::*)( double, double ))&Actor::setPosition)
		.def("setBoxIndex", &Actor::setBoxIndex)
		.def("setAnimation", (void(Actor::*)( PAnimation, bool ) )&Actor::setAnim)
		.def("setAnimation", (void(Actor::*)( PAnimation ) )&Actor::setAnim)
		.def("setAnimation", (void(Actor::*)( const std::string& ) )&Actor::setAnim)
		.def("setAnimation", (void(Actor::*)( const std::string&, bool ) )&Actor::setAnim)
		.def("load", &Actor::load)
		.def("setState", &Actor::setState)
		.enum_("state")
		[
			value("IDLE", Actor::STATE_IDLE),
			value("TALK", Actor::STATE_TALK)
		];
}

luabind::scope registerCamera() {
	using namespace luabind;
	return class_<Camera>("Camera")
		.def("moveTo", &Camera::moveTo)
		.def("follow", (void (Camera::*)(Actor*))&Camera::follow)
		.def("follow", (void (Camera::*)(Sprite*))&Camera::follow)
		.def("focusOnItem", &Camera::focusOnItem);
}

luabind::scope registerMediaPlayer() {
	using namespace luabind;
	return class_<MediaPlayer>("MediaPlayer")
		.def("openStream", &MediaPlayer::openStream)
		.def("playStream", &MediaPlayer::playStream)
		.def("stopStream", &MediaPlayer::stopStream)
		.def("pauseStream", &MediaPlayer::pauseStream)
		.def("setStreamVolume", &MediaPlayer::setStreamVolume)
		.def("channelPlaying", &MediaPlayer::channelPlaying)
		.def("stopChannel", &MediaPlayer::stopChannel)
		.def("playSound", (SoundChannel(MediaPlayer::*)(const Sound*, float))&MediaPlayer::playSound)
		.def("playSound", (SoundChannel(MediaPlayer::*)(const std::string&, float))&MediaPlayer::playSound);
}

//void luaRoomDialogChoices( RoomPlayer* player, int count, const luabind::object& choices ) {
//	std::vector< std::string > choicesV;
//	for( int i = 1; i <= count; ++i ) {
//		choicesV.push_back( luabind::object_cast<std::string>( choices[i] ) );
//	}
//	player->dialogChoice( &choicesV[0], count );
//}

int luaRoomDialogChoices( RoomPlayer* player, lua_State* L ) {
	//RoomPlayer* player = luabind::object_cast<RoomPlayer*>( luabind::from_stack(L, -1) );
	std::vector< std::string > choicesV;
	int top = lua_gettop(L);
	for( int i=0; i<top; ++i ) {
		choicesV.push_back( lua_tostring(L, i+1) );
	}
	player->dialogChoice( &choicesV[0], choicesV.size() );
	return lua_yield(L, 1);
}

luabind::scope registerRoomPlayer() {
	using namespace luabind;
	return class_<RoomPlayer>("RoomPlayer")
		.def("waitActor", &RoomPlayer::waitActor, yield)
		.def("goActor", &RoomPlayer::goActor)
		.def("wait", &RoomPlayer::wait, yield)
		.def("showSpeechText", (void(RoomPlayer::*)(const std::string&, short, short))&RoomPlayer::showSpeechText )
		.def("hideSpeechText", &RoomPlayer::hideSpeechText)
		.def("waitCamera", &RoomPlayer::waitCamera)
		.def("loadRoom", &RoomPlayer::loadRoom)
		.def("dialogChoice", &luaRoomDialogChoices, raw(_2) )
		.def("say", (void(RoomPlayer::*)(Actor*, const char*))&RoomPlayer::say, yield)
		.def("say", (void(RoomPlayer::*)(Actor*, const std::string&, const std::string&))&RoomPlayer::say, yield)
		.def("waitAnimation", &RoomPlayer::waitAnimation, yield)
		.def("cacheAnim", (void(RoomPlayer::*)(const std::string&, bool))&RoomPlayer::cacheAnim)
		.def("cacheAnim", (void(RoomPlayer::*)(const std::string&))&RoomPlayer::cacheAnim)
		.def("uncacheAnim", &RoomPlayer::uncacheAnim)
		.def("cacheSound", (void(RoomPlayer::*)(const std::string&, bool))&RoomPlayer::cacheSound)
		.def("cacheSound", (void(RoomPlayer::*)(const std::string&))&RoomPlayer::cacheSound)
		.def("uncacheSound", &RoomPlayer::uncacheSound)
		.def("activateBox", &RoomPlayer::activateBox);
}

luabind::scope registerLog() {
	using namespace luabind;
	return 
		def( "LOG_INFO", &log_info ),
		def( "LOG_DEBUG", &log_debug ),
		def( "LOG_ERROR", &log_error );
}


luabind::scope registerGlobal() {
	using namespace luabind;
	return class_<Global>("Global")
		.def( "defaultCursor", &Global::defaultCursor )
		.def( "defaultFontName", &Global::defaultFontName )
		.def( "consoleFontName", &Global::consoleFontName )
		.def( "defaultFont", &Global::defaultFont )
		.def( "screenWidth", &Global::screenWidth )
		.def( "screenHeight", &Global::screenHeight )
		.def( "screenDepth", &Global::screenDepth )
		.def( "getMusicVolume", &Global::getMusicVolume )
		.def( "getSoundVolume", &Global::getSoundVolume )
		.def( "getSpeechVolume", &Global::getSpeechVolume )
		.def( "setMusicVolume", &Global::setMusicVolume )
		.def( "setSoundVolume", &Global::setSoundVolume )
		.def( "setSpeechVolume", &Global::setSpeechVolume );
}

luabind::scope registerScriptPlayer() {
	using namespace luabind;
	return class_<ScriptPlayer>( "ScriptPlayer" )
		.def( "loadScript", (void(ScriptPlayer::*)( const std::string& ) )&ScriptPlayer::loadScript )
		.def( "loadScript", (void(ScriptPlayer::*)( const std::string&, const std::string& ) )&ScriptPlayer::loadScript )
		.def( "loadRoom", &ScriptPlayer::loadRoom )
		.def( "resumeRoom", &ScriptPlayer::resumeRoom )
		.def( "exit", &ScriptPlayer::exit );
}

luabind::scope registerImage() {
	using namespace luabind;
	return class_<Image>( "Image" )
		.def( constructor<>() )
		.def( "load", &Image::load );
}

luabind::scope registerSprite() {
	using namespace luabind;
	return class_<AnimatedSprite>( "Sprite" )
		.def(constructor<>())
		.def( "setX", &AnimatedSprite::setX )
		.def( "setY", &AnimatedSprite::setY )
		.def( "setAnimation", (void(AnimatedSprite::*)( PAnimation, bool ) )&AnimatedSprite::setAnimation )
		.def( "setAnimation", (void(AnimatedSprite::*)( PAnimation ) )&AnimatedSprite::setAnimation )
		.def( "render", &AnimatedSprite::render )
		.def( "update", &AnimatedSprite::update );
}

luabind::scope registerScreen() {
	using namespace luabind;
	return class_<Screen>( "Screen")
		.def( "clear", &Screen::clear )
		.def( "drawImage", (void(Screen::*)( const Image*, int, int, unsigned char, short ) )&Screen::drawImage );
}

luabind::scope registerGameState() {
	using namespace luabind;
	return class_<GameState>( "GameState" )
		.def("getState", &GameState::getState)
		.def("setState", &GameState::setState)
		.def("inventoryInsert", &GameState::addToInventory)
		.def("inventoryRemove", &GameState::removeFromInventory);
}

luabind::scope registerAnimationPlayer() {
	using namespace luabind;
	return class_<AnimationPlayer>( "AnimationPlayer" )
		.def("isPlaying", &AnimationPlayer::isPlaying);
}

luabind::scope registerSoundChannel() {
	using namespace luabind;
	return class_<SoundChannel>( "SoundChannel" )
		.def(constructor<>());
}

luabind::scope registerSound() {
	using namespace luabind;
	return class_<Sound>( "Sound" )
		.def("load", &Sound::load);
}

luabind::scope registerCache() {
	using namespace luabind;
	return 
		class_<AnimationManager>( "AnimationManager" )
			.def("get", &AnimationManager::createObject),
		class_<ImageManager>("ImageManager")
			.def("get", &ImageManager::createObject),
		class_<SoundManager>("SoundManager")
			.def("get", &SoundManager::createObject);
}

luabind::scope registerMouseState() {
	using namespace luabind;
	return class_<SMouseState>("Mouse")
		.def_readonly("x", &SMouseState::x)
		.def_readonly("y", &SMouseState::y)
		.def_readonly("lpressed", &SMouseState::lbutton_pressed)
		.def_readonly("ldown", &SMouseState::lbutton)
		.def_readonly("rpressed", &SMouseState::rbutton_pressed)
		.def_readonly("rdown", &SMouseState::rbutton);	
}

bool isKeyboardDown(const SKeyboardState* k, const char* c) {
	return k->keyDown[(int)c[0]];
}

bool isKeyboardPressed(const SKeyboardState* k, const char* c) {
	return k->keyPressed[(int)c[0]];
}

luabind::scope registerKeyboardState() {
	using namespace luabind;
	return class_<SKeyboardState>("Keyboard")
		.def("down", &isKeyboardDown)
		.def("pressed", &isKeyboardPressed);
}

//////////////////////////////////////////////////////////////////////////
lua_State* createNewState() {
	lua_State* newState = luaL_newstate();
	luaL_openlibs( newState );
#ifdef _DEBUG
	luaopen_debug( newState );
#endif

	using namespace luabind;
	open(newState);
	module(newState)
		[
			registerSound(),
			registerSoundChannel(),
			registerItem(),
			registerActor(),
			registerCamera(),
			registerMediaPlayer(),
			registerRoomPlayer(),
			registerGameState(),
			registerScriptPlayer(),
			registerImage(),
			registerSprite(),
			registerScreen(),
			registerLog(),
			registerGlobal(),
			registerAnimationPlayer(),
			registerCache(),
			registerMouseState(),
			registerKeyboardState()
		];

	// register dir constants
	lua_pushnumber( newState, DIR_L );
	lua_setglobal( newState, "DIR_LEFT" );
	lua_pushnumber( newState, DIR_R );
	lua_setglobal( newState, "DIR_RIGHT" );
	lua_pushnumber( newState, DIR_U );
	lua_setglobal( newState, "DIR_UP" );
	lua_pushnumber( newState, DIR_D );
	lua_setglobal( newState, "DIR_DOWN" );

	// singletons
	luabind::globals(newState)["global"] = Global::i();
	luabind::globals(newState)["imageManager"] = ImageManager::i();
	luabind::globals(newState)["animationManager"] = AnimationManager::i();
	luabind::globals(newState)["soundManager"] = SoundManager::i();
	luabind::globals(newState)["mediaPlayer"] = MediaPlayer::i();
	luabind::globals(newState)["game"] = GameState::i();
	luabind::globals(newState)["player"] = GameEngine::i()->getPlayer();

	registerTexts(newState, Global::i()->texts() );

	return newState;
}

void handleError( const luabind::error& e ) {
	lua_State* L = e.state();
	//lua_Debug d;
	//lua_getstack(L, 1, &d);
	//lua_getinfo(L, "Sln", &d);
	//std::string err = lua_tostring(L, -1);
	//lua_pop(L, 1);
	//std::stringstream msg;
	//msg << d.short_src << ":" << d.currentline;
	//if (d.name != 0) {
	//	msg << "(" << d.namewhat << " " << d.name << ")";
	//}
	//msg << " " << err;
	//LOG_ERROR( msg.str() );

	luabind::object error_msg(luabind::from_stack(L, -1));
	LOG_ERROR( error_msg );
	std::string traceback = luabind::call_function<std::string>( luabind::globals(L)["debug"]["traceback"] );
	LOG_ERROR( "LUA TRACEBACK: " << traceback );
	throw LAEException( "Script error" );
}

void loadScript( lua_State* L, const char* fname ) {
	if( luaL_loadfile( L, fname ) ) {
		LOG_ERROR( "Script '" << fname << "' load error" );
		throw LAEException( "Script load error" );
	}

	if( lua_pcall( L, 0, 0, 0 ) ) {
		LOG_ERROR( "Script '" << fname << "' post-load error: " << lua_tostring( L, -1 ) );
		throw LAEException( "Script post-load error" );
	}
}

void registerTexts( lua_State* L, const TextStorage& texts ) {
	for( auto it = texts.begin(); it != texts.end(); ++it ) {
		luabind::globals(L)[it->first] = it->second;
	}
}

} } // LAE::lua
