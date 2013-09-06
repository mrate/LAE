// LAE
#include "RoomPlayer.h"
#include "../logger/Logger.h"
#include "../utils/utils.h"
#include "../cache/TextStorage.h"
#include "../game/Global.h"
#include "../game/GameEngine.h"
#include "../game/GameState.h"
#include "../audio/MediaPlayer.h"
#include "../exception/Exception.h"
// game objects
#include "../graphics/Animation.h"
#include "../graphics/AnimationPlayer.h"
#include "../graphics/Font.h"
#include "../objects/Actor.h"
// cache
#include "../cache/ImageCache.h"
#include "../cache/AnimationCache.h"
#include "../cache/SoundCache.h"
// boost
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
// stl
#include <iostream>
#include <fstream>
#include <limits>
#include <cmath>

namespace LAE {

RoomPlayer::RoomPlayer()
	: activeInvItem_( INVALID_INVENTORY_ITEM )
	, fadeTime_( 0 )
	, fadeType_( FADE_NONE )
	, cursorX_( 0 )
	, cursorY_( 0 )
	, cursorVisible_( true )
	, player_( GameEngine::i()->getPlayer() )
	, scriptRoom_( nullptr )
	, scriptInventory_( nullptr )
	, scriptActive_( nullptr )
	, scriptRunning_( false )
	, scriptWaiting_( false )
	, scriptSkipping_( false )
	, debug_( true ) {
		
		drawDebugPath_ = false;
		debugNearestBox_ = -1;
		debugPanel_.setX( 50 );
		debugPanel_.setY( 100 );
}

RoomPlayer::~RoomPlayer() {
	free();
	delete scriptInventory_;
}

void RoomPlayer::init() {
#ifdef _DEBUG
	debugActor_.load("actors/blank.xml");
#endif
}

ScriptContext* RoomPlayer::createContext() {
	ScriptContext* context = interpret_.createNewContext();

	interpret_.registerObject( context, "room", this );
	interpret_.registerObject( context, "camera", camera_ );

	// register inventory room.items
	for( auto i = Global::i()->itemsMap().begin(); i != Global::i()->itemsMap().end(); i++ ) {
		registerString( context, i->first, i->second.name );
	}

	return context;
}

void RoomPlayer::free() {
	texts_.clear();
	room_.free();

	if( scriptRoom_ ) {
		if( scriptActive_ == scriptRoom_ ) {
			scriptActive_ = nullptr;
		}

		delete scriptRoom_;
		scriptRoom_ = nullptr;
	}
}

void RoomPlayer::loadScript( ScriptContext* context, const std::string& fname ) const {
	LOG_DEBUG( "Loading script '" << fname << "' ..." );
	interpret_.loadFile( context, fname );
	LOG_DEBUG( "ok" );
}

void RoomPlayer::loadRoom( const std::string& fname ) {
	free();
	
	Global::i()->setDefaultCursor();
	
	// load room
	room_.load( fname.c_str() );

	// init camera
	camera_.follow( player_ );
	camera_.setBorders( room_.getWidth(), room_.getHeight() );

	// init player state
	scriptRunning_ = false;
	scriptWaiting_ = false;
	scriptSkipping_ = false;

	// wait event
	fadeType_ = FADE_NONE;
	resetWaitEvent();

	// init player
	//@@TODO:
	player_->setPosition( room_.getBoxes()[0].center.x, room_.getBoxes()[0].center.y );
	player_->setBoxIndex( 0 );
	player_->setState( Actor::STATE_IDLE );

	// init speech / description
	description_.enabled = false;
	speechText_.enabled = false;

	// init script
	scriptRoom_ = createContext();
	loadScript( scriptRoom_, room_.getRoomScript() );
	loadScript( scriptRoom_, Global::i()->inventoryScript() );

	for( auto it = room_.getItems().begin(); it != room_.getItems().end(); ++it ) {
		registerItem( scriptRoom_, it->second );
	}

	// global texts
	lua::registerTexts( scriptRoom_->getMain(), Global::i()->texts() );
	// room texts
	texts_.load( room_.getTextFile() );
	lua::registerTexts( scriptRoom_->getMain(), texts_ );

	// run init function
	setActiveContext(scriptRoom_);
	interpret_.runFunction( scriptRoom_, "init" );

	assert( !scriptRunning_ );
	if( scriptRunning_ ) {
		throw LAEException( "Init called yield function!" );
	}

	// run main function
	interpret_.runFunction( scriptRoom_, "main" );

	// fade-in
	fadeIn();

	// done
	LOG_DEBUG( "done" );
}

void RoomPlayer::registerString( ScriptContext* context, const std::string& name, const std::string& str ) const {
	interpret_.registerObject( context, name, str );
}

void RoomPlayer::registerItem( ScriptContext* context, Item& item ) const {
	interpret_.registerObject( context, item.getName(), &item );
}

void RoomPlayer::cacheAnim( const std::string& name ) {
	LOG_DEBUG("cacheAnim(" << name << ")");
	animationCache_.push_back( AnimationManager::i()->createObject(name) );
}

void RoomPlayer::uncacheAnim( const std::string& name ) {
	LOG_DEBUG("uncacheAnim(" << name << ")");
	animationCache_.erase( 
		std::remove_if( animationCache_.begin(), animationCache_.end(), [&name]( PAnimation anim ) { return anim->getFileName() == name; } ), 
		animationCache_.end() 
		);
}

void RoomPlayer::cacheSound( const std::string& name ) {
	LOG_DEBUG("cacheSound(" << name << ")");
	soundCache_.push_back( SoundManager::i()->createObject(name) );
}

void RoomPlayer::uncacheSound( const std::string& name ) {
	LOG_DEBUG("uncacheSound(" << name << ")");
	soundCache_.erase( 
		std::remove_if( soundCache_.begin(), soundCache_.end(), [&name]( PSound snd ) { return snd->getFileName() == name; } ), 
		soundCache_.end() 
		);
}

void RoomPlayer::inventoryInsert( const std::string& name ) {
	Global::i()->setCursor( Global::i()->defaultCursor() );
	GameState::i()->addToInventory(name);
	setActiveInventoryItem( INVALID_INVENTORY_ITEM );
}

void RoomPlayer::inventoryRemove( const std::string& name ) {
	Global::i()->setCursor( Global::i()->defaultCursor() );
	GameState::i()->removeFromInventory(name);
	setActiveInventoryItem( INVALID_INVENTORY_ITEM );
}

bool RoomPlayer::waitLoadRoom( SBaseWaitEvent* event ) {
	return true;
}

void RoomPlayer::scrLoadRoom( const std::string& fname ) {
	setWaitEvent( new SWaitLoadRoomEvent( fname ) );
	fadeOut();
}

void RoomPlayer::activateBox( unsigned char boxIndex, bool active ) {
	room_.setBoxActive( boxIndex, active );
}

void RoomPlayer::waitActor() {
	LOG_DEBUG( "actor_wait" );
	setWaitEvent( new SWaitActorEvent( player_, Actor::STATE_IDLE ) );
}

void RoomPlayer::waitAnimation( const AnimationPlayer* animation ) {
	LOG_DEBUG("wait_animation");
	setWaitEvent( new SWaitAnimationEvent( animation ) );
}

void RoomPlayer::wait( unsigned long time ) {
	if( scriptSkipping_ ) {
		return ;
	}

	setWaitEvent( new SWaitTimerEvent( time ) );
}

void RoomPlayer::showSpeechText( const std::string& text, short x, short y ) {
	if( scriptSkipping_ ) {
		return ;
	}

	speechText_.enabled = true;
	speechText_.text = text;
	speechText_.x = x - Global::i()->defaultFont()->getWidth( text ) / 2;
	speechText_.y = y;
}

void RoomPlayer::showSpeechText( const std::string& text, double x, double y ) {
	showSpeechText( text, (short)x, (short)y );
}

void RoomPlayer::hideSpeechText() {
	speechText_.enabled = false;
}

void RoomPlayer::waitCamera() {
	if( scriptSkipping_ ) {
		camera_.fastForward();
		return ;
	}

	setWaitEvent( new SWaitCameraEvent() );
}

void RoomPlayer::goActor( int x, int y ) {
	findPath( x, y, player_ );

	if( scriptSkipping_ ) {
		player_->clearPath();
		player_->setPosition( (double)x, (double)y );
	}
}

void RoomPlayer::say( Actor* actor, const char* text ) {
	if( scriptSkipping_ ) {
		return ;
	}

	actor->setState(Actor::STATE_TALK);
	showSpeechText( text, actor->getX(), actor->getY() - actor->getHeight() );
	setWaitEvent( new SWaitSpeechEvent( actor, 5000 ) ); // TODO: zatim natvrdo 5 sekund
}

void RoomPlayer::say( Actor* actor, const std::string& text, const std::string& sound ) {
	if( scriptSkipping_ ) {
		return ;
	}

	actor->setState(Actor::STATE_TALK);
	showSpeechText( text, actor->getX(), actor->getY() - actor->getHeight() );
	speechText_.sound.load( sound );
	speechText_.soundChannel = MediaPlayer::i()->playSound( GameEngine::SPEECH_AUDIO_CHANNEL, &speechText_.sound, Global::i()->getSpeechVolume() );
	setWaitEvent( new SWaitSpeechEvent( actor, 5000, &speechText_.soundChannel ) ); // TODO: zatim natvrdo 5 sekund
}

int RoomPlayer::dialogChoice( const std::string* choices, unsigned char choicesCount ) {
	for( int i = 0; i < choicesCount; ++i ) {
		dialog_.choices.push_back( *choices++ );
	}

	dialog_.selectedAnswer = -1;
	description_.enabled = false;

	setWaitEvent( new SWaitDialogEvent() );
	return 0; // yield
}

void RoomPlayer::runExamineFunc( ScriptContext* context, const std::string& func ) {
	if( scriptRunning_ || scriptWaiting_ ) {
		return;
	}

	player_->clearPath();
	player_->setState( Actor::STATE_IDLE );

	setActiveContext(context);
	LOG_DEBUG( " running examine function " << func );
	interpret_.runFunction( context, func );
}

void RoomPlayer::runUseFunc( ScriptContext* context, const std::string& func ) {
	if( scriptRunning_ || scriptWaiting_ ) {
		return;
	}

	setActiveContext(context);
	player_->clearPath();
	player_->setState( Actor::STATE_IDLE );
	if( activeInvItem_ != INVALID_INVENTORY_ITEM ) {
		std::string activeItemName = GameState::i()->getInventory()[activeInvItem_].name;
		setActiveInventoryItem( INVALID_INVENTORY_ITEM );
		LOG_DEBUG( " running use function: " << func );
		interpret_.runFunction( context, func, activeItemName.c_str() );
	} else {
		LOG_DEBUG( " running use function: " << func );
		interpret_.runFunction( context, func, "" );
	}
}

unsigned long RoomPlayer::getWaitEvent() const {
	return waitEvent_ ? waitEvent_->id : SBaseWaitEvent::ID;
}

void RoomPlayer::render( Screen& screen ) {
	// tmp
	screen.clear();
	screen.setEffect( Screen::EFFECT_NONE );

	// bg
	screen.drawImage( room_.getBgImage().get(), -camera_.getX(), -camera_.getY() );

	// room.items in z-order
	for( int i = Global::i()->maxZPlaneIndex(); i >= Global::i()->minZPlaneIndex(); i-- ) {
		//LOG_DEBUG( "Rendering z-order: " << i );
		const AnimatedSprites* sprites = room_.getLayer(i);
		if( sprites != nullptr ) {
			for( auto zplaneIt = sprites->begin(); zplaneIt != sprites->end(); ++zplaneIt ) {
				const AnimatedSprite* sprite = *zplaneIt;
//				if( const Item* item = dynamic_cast<const Item*>(sprite) ) {
//					LOG_DEBUG("Rendering item " << item->getName() );
//				} else {
//					LOG_DEBUG("Rendering z-plane");
//				}

				if( sprite->getAnimation() ) {
					//	LOG_DEBUG( "rendering anim: " << sprite->getAnimation()->getFileName() );
					screen.setEffect( sprite->getEffect() );
					sprite->render( screen, -camera_.getX(), -camera_.getY() );
				} else {
					LOG_DEBUG(" !! NO ANIMATION !!");
				}
			}
		}

		//LOG_DEBUG("Rendering boxes")
		if( room_.getBoxes()[player_->getBoxIndex()].z == i ) {
			// render actor
//			LightSource light1;
//			light1.x = 322 - camera_.getX();
//			light1.y = 21 - camera_.getY();
//			light1.intensity = 100;
//
//			screen.enableLight( 0, &light1 );
			player_->render( screen, -camera_.getX(), -camera_.getY() );
//			screen.disableLight( 0 );
		}
	}

	screen.setEffect( Screen::EFFECT_NONE );
	if( speechText_.enabled ) {
		const Font& bmpFont = *Global::i()->defaultFont();

		int sx = (int)speechText_.x;
		int sy = (int)speechText_.y;
		int width = bmpFont.getWidth( speechText_.text );
		if( sx < camera_.getX() ) {
			sx = camera_.getX();
		}
		if( sx + width > camera_.getX() + Global::i()->screenWidth() ) {
			sx = camera_.getX() + Global::i()->screenWidth() - width;
		}
		if( sy < camera_.getY() ) {
			sy = camera_.getY();
		}
		if( sy + bmpFont.getCharHeight() > camera_.getY() + Global::i()->screenHeight() ) {
			sy = camera_.getY() + Global::i()->screenHeight() - bmpFont.getCharHeight();
		}
		screen.drawText( speechText_.text, bmpFont, sx - camera_.getX(), sy - camera_.getY() );
	}

	// inventory
	renderInventory(screen);

	// dialog
	renderDialog(screen);

	// debug
	renderDebug(screen);

	// cursor
	renderCursor(screen);

	// fade in/out
	renderFade(screen);
}

void RoomPlayer::fadeIn() {
	fadeTime_ = 0;
	fadeType_ = FADE_IN;
}

void RoomPlayer::fadeOut() {
	fadeTime_ = 0;
	fadeType_ = FADE_OUT;
}

void RoomPlayer::findNearestBox( int x, int y, unsigned char srcBox, unsigned char& dstBox, Point& nearest ) const {
	// find nearest box
	nearest.x = room_.getWidth();
	nearest.y = room_.getHeight();

	double nearestDist = -1;
	int nearestBox = srcBox;
	bool inBox = false;

	Segment line1;
	line1.p0 = Point(x, y);
	for( auto i = room_.getBoxes().begin(); i != room_.getBoxes().end(); ++i, ++dstBox ) {
		const WalkingBox& box = *i;
		if( !box.active ) {
			continue;
		}

		inBox = true;
		line1.p1 = box.center;

		for( int j = 0; j < 4; ++j ) {
			Segment line2( box.p[j], box.p[( j + 1 ) % 4] );

			Point intersection;
			if( lineIntersect( line1, line2, intersection ) ) {
				inBox = false;
				double dist = dist_Point_to_Segment( line1.p0, line2 );
				if( nearestDist < 0 || dist < nearestDist ) {
					nearestDist = dist;
					nearest = intersection;
					nearestBox = dstBox;
				}
			}
		}

		if( inBox ) {
			// point is in some box - we can stop
			nearestBox = dstBox;
			nearest = Point(x, y);
			
			debugNearestBox_ = nearestBox;
			debugPanel_ << "In box: " << (int)nearestBox << "\n";
			return;
		}
	}

	debugNearestBox_ = nearestBox;
	debugPanel_ << "Nearest box: " << (int)nearestBox << "\n";
	dstBox = nearestBox;
}

void RoomPlayer::findPath( int x, int y, Actor* actor ) const {
	/*
	debugPanel_.reset();
	debugPanel_ << "mouse: " << cursorX_ << ", " << cursorY_ << "\n";
	for( size_t i = 0; i < room_.getBoxes().size(); ++i ) {
		debugPanel_ << "N[" << i << "]: ";
		const WalkingBox& box = room_.getBoxes()[i];
		for( auto it = box.neighbour.begin(); it != box.neighbour.end(); ++it ) {
			debugPanel_ << *it << " ";
		}
		debugPanel_ << "\n";
	}
	*/

	// reset actor's path
	actor->clearPath();

	unsigned char srcBox = actor->getBoxIndex();

	// find nearest box
	Point nearest;
	unsigned char dstBox = 0;
	findNearestBox( x, y, srcBox, dstBox, nearest );

	if( srcBox == dstBox ) {
		actor->addToPath( nearest );
		actor->setState( Actor::STATE_GO );
		return ;
	}

	// find path to dstBox
	std::vector<bool> closed(room_.getBoxes().size());
	std::vector<int> pred(room_.getBoxes().size());
	std::vector<int> dist(room_.getBoxes().size());

	bool found = false;
	for( size_t i = 0; i < room_.getBoxes().size(); ++i ) {
		closed[i] = false;
		pred[i] = -1;
		dist[i] = std::numeric_limits<int>::max();
	}

	deque<int> open;
	open.push_back( srcBox );
	dist[srcBox] = 0;
	while( !open.empty() && !found ) {
		int b = open.front();
		open.pop_front();
		closed[b] = true;

		const WalkingBox& box = room_.getBoxes()[b];

		for( auto it = box.neighbour.begin(); it != box.neighbour.end(); ++it ) {
			const WalkingBox& neigh = room_.getBoxes()[*it];

			if( !neigh.active ) {
				closed[*it] = true;
			}
			if( closed[*it] ) {
				continue;
			}

			int alt = dist[b] + 1;
			if( alt < dist[*it] ) {
				dist[*it] = alt;
				pred[*it] = b;
			}

			if( *it == dstBox ) {
				found = true;
				break;
			}
			open.push_back( *it );
		}
	}

	// we have path of boxes, now construct path of points
	if( found ) {
		std::deque<Point> path;
		findPath( srcBox, dstBox, &pred[0], actor->getPosition(), nearest, path );
		// copy path to actor's path
		for( auto it = path.begin(); it != path.end(); ++it ) {
			actor->addToPath( *it );
		}
		// set actor walking
		actor->setState( Actor::STATE_GO );
	} else {
		// no path found
		actor->setState( Actor::STATE_IDLE );
	}

	return ;
}

void RoomPlayer::findPath( int srcBox, int dstBox, int* pred, const Point& start, const Point& end, std::deque<Point>& path ) const {
	Segment lastLine( start, end );
	int currentBoxIndex = dstBox;

	// add destination to path
	Point nEnd = end;
	moveCloser( nEnd, room_.getBoxes()[dstBox].center );
	path.push_back(nEnd);
	// for all boxes until start:
	while( currentBoxIndex != srcBox ) {
		const WalkingBox& currentBox = room_.getBoxes()[currentBoxIndex];

		// check, if current path line has intersection with boxes
		int previousBoxIndex = pred[currentBoxIndex];
		auto interIt = currentBox.inter.find(previousBoxIndex);
		if( interIt != currentBox.inter.end() ) {
			const Segment& boxesInter = interIt->second;

			Point intersection;
			if( lineIntersect(lastLine, boxesInter, intersection ) ) {
				// path goes through boxes - no need for adding anything to path
			} else {
				// it's not possible to walk - we need to add more points to path
				double d1 = dist_Point_to_Segment( boxesInter.p0, lastLine );
				double d2 = dist_Point_to_Segment( boxesInter.p1, lastLine );

				lastLine.p1 = d1 < d2 ? boxesInter.p0 : boxesInter.p1;
				if( d1 < d2 ) {
					moveCloser( lastLine.p1, boxesInter.p1 );
				} else {
					moveCloser( lastLine.p1, boxesInter.p0 );
				}

				path.clear();
				findPath( currentBoxIndex, dstBox, pred, lastLine.p1, end, path );
				path.push_front( lastLine.p1 );
			}
		}
		// continue with previous box from path
		currentBoxIndex = previousBoxIndex;
	}
}

void RoomPlayer::moveCloser( Point& point, const Point& toPoint ) const {
	const int offset = 2;
	if( point.x < toPoint.x ) {
		point.x += offset;
	} else if( point.x > toPoint.x ) {
		point.x -= offset;
	}
	if( point.y < toPoint.y ) {
		point.y += offset;
	} else if( point.y > toPoint.y ) {
		point.y -= offset;
	}
}

GameScreen::EScreenState RoomPlayer::update( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili ) {
	if( keyboard.keyPressed[27] ) {
		return GameScreen::PAUSED;
	}

	if( keyboard.keyDown['p'] ) {
		drawDebugPath_ = true;
		debugActor_.setPosition( player_->getPosition() );
		debugActor_.setBoxIndex( player_->getBoxIndex() );
		findPath(mouse.x, mouse.y, &debugActor_);
	}

	if( !keyboard.keyDown['p'] ) {
		drawDebugPath_ = false;
	}

	camera_.update(timeMili);
	const Font& bmpFont = *Global::i()->defaultFont();

	// handle input
	cursorX_ = mouse.x;
	cursorY_ = mouse.y;

	int offsetMouseX = mouse.x + camera_.getX();
	int offsetMouseY = mouse.y + camera_.getY();

	cursorVisible_ = true;
	PAnimation newCursor = Global::i()->defaultCursor();

	bool lpressed = mouse.lbutton_pressed;
	bool rpressed = mouse.rbutton_pressed;
	int INVENTORY_POS_Y = Global::i()->inventoryPosY();
	int INVENTORY_ITEM_WIDTH = Global::i()->invItemWidth();
	int INVENTORY_ITEM_HEIGHT = Global::i()->invItemHeight();

	if( activeInvItem_ != INVALID_INVENTORY_ITEM ) {
		newCursor = Global::i()->invItem( GameState::i()->getInventory()[activeInvItem_].name ).animation;
	}

	if( keyboard.keyPressed['d'] || keyboard.keyPressed['D'] ) {
		debug_ = !debug_;
	}

	if( getWaitEvent() == SWaitDialogEvent::ID ) {
		// ceka se na vyber obdpovedi pri rozhovoru
		dialog_.selectedAnswer = -1;

		if( mouse.y > INVENTORY_POS_Y - bmpFont.getCharHeight() * ( int )dialog_.choices.size() && mouse.y < INVENTORY_POS_Y ) {
			// vybrana odpoved
			dialog_.selectedAnswer = ( mouse.y - ( INVENTORY_POS_Y - bmpFont.getCharHeight() * dialog_.choices.size() ) ) / bmpFont.getCharHeight();
		}
	} else if( scriptRunning_ ) {
		// bezi skript -> schovat mys
		cursorVisible_ = false;
	} else {
		// skript nebezi
		if( rpressed && activeInvItem_ != INVALID_INVENTORY_ITEM ) {
			// zruseni aktivniho predmetu
			setActiveInventoryItem( INVALID_INVENTORY_ITEM );
			newCursor = Global::i()->defaultCursor();
			rpressed = false;
			LOG_DEBUG( "deactivated inventory item" );
		}

		int activeItem = INVALID_INVENTORY_ITEM;
		int invItem = INVALID_INVENTORY_ITEM;
		description_.enabled = false;

		if( mouse.y >= INVENTORY_POS_Y && mouse.y <= INVENTORY_POS_Y + INVENTORY_ITEM_HEIGHT ) {
			invItem = mouse.x / INVENTORY_ITEM_WIDTH;
			if( invItem >= ( int )GameState::i()->getInventory().size() ) {
				invItem = INVALID_INVENTORY_ITEM;
			} else {
				const SInventoryItem& item = Global::i()->invItem( GameState::i()->getInventory()[invItem].name );
				description_.enabled = true;
				description_.text = getText( item.description );

				if( lpressed && activeInvItem_ == INVALID_INVENTORY_ITEM ) {
					// select item
					activeInvItem_ = invItem;
					newCursor = item.animation;
				} else if( lpressed && invItem != activeInvItem_ ) {
					runUseFunc( scriptRoom_, item.onUse );
				} else if( rpressed && activeInvItem_ == INVALID_INVENTORY_ITEM ) {
					// examine inv. item
					runExamineFunc( scriptRoom_, item.onExamine );
				} else if( activeInvItem_ == INVALID_INVENTORY_ITEM ) {
					// item hover
					newCursor = Global::i()->cursorAnim( item.cursorName );
				}
			}
		} else {
			for( auto i = room_.getItems().begin(); i != room_.getItems().end(); i++ ) {
				const Item& item = ( *i ).second;
				if( !item.isActive() ) {
					continue;
				}

				if( offsetMouseX >= item.getX() + item.getActiveRect().left() 
					&& offsetMouseX <= item.getX() + item.getActiveRect().right() 
					&& offsetMouseY >= item.getY() + item.getActiveRect().top() 
					&& offsetMouseY <= item.getY() + item.getActiveRect().bottom() ) {
						description_.enabled = true;
						if( activeInvItem_ == INVALID_INVENTORY_ITEM ) {
							newCursor = Global::i()->cursorAnim( item.getCursorName() );
						}

						//description = item.descr;
						description_.text = texts_.getText(item.getDescription());

						if( lpressed && !item.getOnUse().empty() ) {
							lpressed = false;
							runUseFunc( scriptRoom_, item.getOnUse() );
						}

						if( rpressed && !item.getOnExamine().empty() && activeItem == -1 ) {
							rpressed = false;
							runExamineFunc( scriptRoom_, item.getOnExamine() );
						}

						break;
				}
			}
		}

		if( description_.enabled ) {
			description_.x = mouse.x - bmpFont.getWidth( description_.text ) / 2;
			description_.y = mouse.y - bmpFont.getCharHeight();
		}

		if( lpressed && mouse.y < INVENTORY_POS_Y ) {
			findPath( offsetMouseX, offsetMouseY, player_ );
			LOG_DEBUG( "mx=" << offsetMouseX << ", my=" << offsetMouseY );
		}
	}

	// scale update
	for( size_t i = 0; i < room_.getBoxes().size(); ++i ) {
		// determine on which box the player is on right now
		const WalkingBox& box = room_.getBoxes()[i];
		if( pointInBox( box, player_->getPosition() ) ) {
			// update scale of player
			player_->setBoxIndex( i );
			const Scale& scale = room_.getScales()[box.scale];

			if( player_->getY() <= scale.p0.y ) {
				player_->setScale( scale.scale1 );
			}
			if( player_->getY() >= scale.p1.y ) {
				player_->setScale( scale.scale2 );
			}
			if( player_->getY() > scale.p0.y && player_->getY() < scale.p1.y ) {
				double scl_size = scale.p1.y - scale.p0.y;
				double scl_y = player_->getY() - scale.p0.y;
				double scl_div = scl_y / scl_size;
				double scl_int = scl_div * ( scale.scale2 - scale.scale1 );
				player_->setScale( (unsigned char)( scale.scale1 + scl_int ) );
			}
			break;
		}
	}

	// update cursor
	Global::i()->setCursor( newCursor );

	// update animations
	if( fadeType_ != FADE_NONE ) {
		fadeTime_ += timeMili;
		if( fadeTime_ > FADE_TIMEOUT ) {
			fadeType_ = FADE_NONE;
			fadeTime_ = FADE_TIMEOUT;
		}
	}

	// update actors
	player_->update( timeMili );

	// update room objects
	room_.update( timeMili );
	// update inventory objects
	GameState::i()->update( timeMili );

	// event
	handleEvents( mouse, keyboard, timeMili );

	return GameScreen::RUNNING;
}

void RoomPlayer::handleEvents( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili ) {
	if( !scriptRunning_ ) {
		scriptSkipping_ = false;
	}

	bool isWaitDialog = false;
	bool eventRaised = false;

	switch( getWaitEvent() ) {	
	case SBaseWaitEvent::ID:
		eventRaised = true;
		break;

	case SWaitActorEvent::ID: 
		{
			SWaitActorEvent* event = static_cast<SWaitActorEvent*>( waitEvent_.get() );
			eventRaised = ( event->actor->getState() == event->state );
		}
		break;

	//case SWaitActorAnimEvent::ID:
	//	eventRaised = !player_->isAnimPlaying();
	//	break;

	case SWaitTimerEvent::ID: 
		{
			SWaitTimerEvent* event = static_cast<SWaitTimerEvent*>( waitEvent_.get() );
			if( event->timeToWait <= timeMili ) {
				eventRaised = true;
			} else {
				event->timeToWait -= timeMili;
			}
		}
		break;

	case SWaitCameraEvent::ID:
		eventRaised = camera_.isIdle();
		break;

	case SWaitDialogEvent::ID:
		if( dialog_.selectedAnswer >= 0 && mouse.lbutton_pressed ) {
			dialog_.choices.clear();
			isWaitDialog = true;
			eventRaised = true;
		}
		break;

	case SWaitSpeechEvent::ID: 
		{
			// pokud hraje zvuk, pak se ceka na dohrani, jinak se ceka na timer
			SWaitSpeechEvent* event = static_cast<SWaitSpeechEvent*>( waitEvent_.get() );
			if( event->soundChannel != nullptr ) {
				eventRaised = !MediaPlayer::i()->channelPlaying( speechText_.soundChannel );
			} else if( event->timeToWait < timeMili ) {
				eventRaised = true;
			} else {
				event->timeToWait -= timeMili;
			}
			// odkliknuti pravou mysi
			if( mouse.lbutton_pressed ) {
				eventRaised = true;
			}
			if( eventRaised ) {
				event->actor->setState(Actor::STATE_IDLE);
				if( event->soundChannel != nullptr ) {
					MediaPlayer::i()->stopChannel( speechText_.soundChannel );
				}
				hideSpeechText();
			}
		}
		break;

	case SWaitAnimationEvent::ID: 
		{
			SWaitAnimationEvent* event = static_cast<SWaitAnimationEvent*>( waitEvent_.get() );
			eventRaised = !event->player->isPlaying();
		}
		break;

	case SWaitLoadRoomEvent::ID: 
		{
			SWaitLoadRoomEvent* event = static_cast<SWaitLoadRoomEvent*>( waitEvent_.get() );
			if( fadeType_ == FADE_NONE ) {
				loadRoom( event->roomName );
			}
		}
		break;
	}

	if( scriptWaiting_ && eventRaised ) {
		LOG_DEBUG( "event raised" );
		resetWaitEvent();
		if( isWaitDialog ) {
			interpret_.resume( scriptRoom_, dialog_.selectedAnswer );
		} else {
			interpret_.resume( scriptRoom_ );
		}
	}
}

void RoomPlayer::setActiveInventoryItem( short inventoryItem ) {
	activeInvItem_ = inventoryItem;
}

void RoomPlayer::renderInventory( Screen& screen ) {
	int INVENTORY_ITEM_WIDTH = Global::i()->invItemWidth();
	int INVENTORY_ITEM_HEIGHT = Global::i()->invItemHeight();
	int INVENTORY_POS_Y = Global::i()->inventoryPosY();

	int x = INVENTORY_ITEM_WIDTH / 2;
	for( auto i = GameState::i()->getInventory().begin(); i != GameState::i()->getInventory().end(); ++i ) {
		i->render( screen, x, INVENTORY_POS_Y + INVENTORY_ITEM_HEIGHT / 2 );
		x += INVENTORY_ITEM_WIDTH;
	}
}

void RoomPlayer::renderDialog( Screen& screen ) {
	if( getWaitEvent() == SWaitDialogEvent::ID ) {
		const Font& bmpFont = *Global::i()->defaultFont();

		int startY = Global::i()->inventoryPosY() - dialog_.choices.size() * bmpFont.getCharHeight();
		int n = 0;
		for( size_t i = 0; i < dialog_.choices.size(); ++i ) {
			if( n++ == dialog_.selectedAnswer ) {
				screen.drawText( dialog_.choices[i], bmpFont, 0, startY, Color(0, 255, 0) );
			} else {
				screen.drawText( dialog_.choices[i], bmpFont, 0, startY );
			}
			startY += bmpFont.getCharHeight();
		}
	}
}

void RoomPlayer::drawNicePoly( Screen& screen, const Point* p, const Color& color ) const {
	Color color2;
	color2.r = std::max<int>( color.r - 100, 0 );
	color2.g = std::max<int>( color.g - 100, 0 );
	color2.b = std::max<int>( color.b - 100, 0 );
	color2.a = std::max<int>( color.a - 100, 0 );

	Point pp[4];
	memcpy( pp, p, sizeof(Point) * 4 );
	for( int i = 0; i < 4; ++i ) {
		pp[i].x -= camera_.getX();
		pp[i].y -= camera_.getY();
	}

	screen.drawFillPoly( pp, 4, color2 );
	screen.drawPoly( pp, 4, color );
}

void RoomPlayer::renderPath( Screen& screen, Actor* actor ) const {
	Point lastPos = actor->getPosition();
	lastPos.x -= camera_.getX();
	lastPos.y -= camera_.getY();

	for( auto it = actor->getPath().begin(); it != actor->getPath().end(); ++it ) {
		GamePosition pos = *it;
		pos.x -= camera_.getX();
		pos.y -= camera_.getY();

		screen.drawLine( lastPos, pos, Color( 255, 0, 255 ) );
		lastPos = pos;
	}
}

void RoomPlayer::renderDebug( Screen& screen ) {
	if( !debug_ ) {
		return ;
	}

	const Font& bmpFont = *Global::i()->defaultFont();

	// debug box
	int index = 0;
	for( auto boxIt = room_.getBoxes().begin(); boxIt != room_.getBoxes().end(); ++boxIt, ++index ) {
		int r = 0;
		int g = 0;
		int b = 200;

		if( index == player_->getBoxIndex() ) {
			r = 200;
			b = 0;
		}

		if( index == debugNearestBox_ ) {
			r = 255;
			g = 201;
			b = 13;
		}

		drawNicePoly(screen, boxIt->p, Color( r, g, b ) );

		for( auto it = boxIt->inter.begin(); it != boxIt->inter.end(); ++it ) {
			Segment s( it-> second );
			s.p0.x -= camera_.getX();
			s.p0.y -= camera_.getY();
			s.p1.x -= camera_.getX();
			s.p1.y -= camera_.getY();
			screen.drawLine( s, Color( 0, 255, 0 ) );
		}

		std::string indexStr = boost::lexical_cast<std::string>(index);
		int cx = static_cast<int>( boxIt->center.x - bmpFont.getWidth(indexStr) / 2 );
		int cy = static_cast<int>( boxIt->center.y - bmpFont.getCharHeight() / 2 );

		screen.drawText( indexStr, bmpFont, cx - camera_.getX(), cy - camera_.getY() );
	}

	// debug item
	for( auto itemIt = room_.getItems().begin(); itemIt != room_.getItems().end(); itemIt++ ) {
		const Item& item = ( *itemIt ).second;
		screen.drawRectangle(
			item.getX() + item.getActiveRect().left() - camera_.getX(),
			item.getY() + item.getActiveRect().top() - camera_.getY(),
			item.getX() + item.getActiveRect().right() - camera_.getX(),
			item.getY() + item.getActiveRect().bottom() - camera_.getY()
			);

		screen.drawText( item.getName(), bmpFont, item.getX() - camera_.getX(), item.getY() - camera_.getY() );
	}

	renderPath(screen, player_);

	if( drawDebugPath_ ) {
		renderPath(screen, &debugActor_);
	}

	debugPanel_.autoResize();
	debugPanel_.setX( 20 );
	debugPanel_.setY( Global::i()->screenHeight() - debugPanel_.getHeight() - 20 );
	debugPanel_.render(screen);
}

void RoomPlayer::renderFade( Screen& screen ) {
	if( fadeType_ != FADE_NONE ) {
		int alpha = (int)( 255.0f * ( ( float )fadeTime_ / ( float )FADE_TIMEOUT ) );
		if( fadeType_ == FADE_IN ) {
			alpha = 255 - alpha;
		}
		screen.fillRectangle( 0, 0, Global::i()->screenWidth(), Global::i()->screenHeight(), Color( 0, 0, 0, alpha ) );
	}
}

void RoomPlayer::renderCursor( Screen& screen ) {
	if( cursorVisible_ ) {
		Global::i()->getCursor().render( screen, cursorX_, cursorY_ );

		// description
		if( description_.enabled ) {
			screen.drawText( description_.text, *Global::i()->defaultFont(), description_.x, description_.y, Color( 255, 0, 0 ) );
		}
	}
}

void RoomPlayer::setWaitEvent( SBaseWaitEvent* event ) {
	waitEvent_.reset( event );
	scriptWaiting_ = true;
	scriptRunning_ = true;
}

void RoomPlayer::resetWaitEvent() {
	waitEvent_.reset();
	scriptWaiting_ = false;
	scriptRunning_ = false;
}

void RoomPlayer::setActiveContext( ScriptContext* context ) {
	scriptActive_ = context;
	scriptRunning_ = false;
}

const std::string& RoomPlayer::getText( const std::string& name ) const {
	static const std::string empty;
	auto it = texts_.find( name );
	if( it != texts_.end() ) {
		return it->second;
	} else {
		it = Global::i()->texts().find( name );
		if( it != Global::i()->texts().end() ) {
			return it->second;
		}
	}

	return empty;
}

}
