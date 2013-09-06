#ifdef WIN32
#	include <Windows.h>
#	include <glut.h>
#else
#	include <GL/glut.h>
#endif
// GL
#include <GL/gl.h>
#include <GL/glu.h>
// LAE
#include "../../LAE_Core/logger/Logger.h"
#include "../../LAE_Core/logger/OStreamContext.h"
#include "../../LAE_Core/game/Global.h"
#include "../../LAE_Core/types/types.h"
#include "../../LAE_Core/screens/GameManager.h"
#include "../../LAE_Core/screens/RoomPlayer.h"
#include "../../LAE_Core/screens/ScriptPlayer.h"
#include "../../LAE_Core/cache/AnimationCache.h"
#include "../../LAE_Core/cache/ImageCache.h"
// boost
#include <boost/program_options.hpp>

using namespace LAE;

namespace {

SMouseState mouse;
SKeyboardState keyboard;
GameManager* manager = 0;

GLvoid ReSizeGLScene( GLsizei width, GLsizei height ) {	// Resize And Initialize The GL Window
	if( height == 0 ) {									// Prevent A Divide By Zero By
		height = 1;										// Making Height Equal One
	}
	glViewport( 0, 0, width, height );						// Reset The Current Viewport
	glMatrixMode( GL_PROJECTION );						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective( 45.0f, ( GLfloat )width / ( GLfloat )height, 0.1f, 100.0f );	// Calculate Window Aspect Ratio
	glMatrixMode( GL_MODELVIEW );							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

bool CreateMainWindow( int width, int height, int bits, int fullscreenflag ) {
	glutCreateWindow("LAE");
	glutReshapeWindow(width, height);
	if( fullscreenflag ) {
		glutFullScreen();
	}

	return true;
}

void init( const std::string& settings = std::string() ) {
	logger::Logger::addContext( new logger::OStreamContext( std::cout, std::cerr ) );
	logger::Logger::addContext( new logger::OStreamContext( "log.txt" ) );

	try {
		bool res;
		if( settings.empty() ) {
			res = Global::i()->load();
		} else {
			res = Global::i()->load( settings );
		}

		if( !res ) {
			std::cerr << "Chyba nacitani settings.xml" << std::endl;
			exit(1);
		}
	} catch( std::exception& ex ) {
		std::cerr << "Chyba inicializace: " << ex.what() << std::endl;
		exit(1);
	}

	// Perform application initialization:
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );

	if( !CreateMainWindow( Global::i()->screenWidth(), Global::i()->screenHeight(), Global::i()->screenDepth(), false ) ) {
		exit(1);
	}
	glutInitWindowPosition( 100, 100 );

	//@@TODO: hack, aby se znovu nacetly animace a obrazky (kurzory, fonty atd.) az po tom, co se nainicializuje OpenGL
	AnimationManager::i()->clearAll();
	ImageManager::i()->clearAll();
	if( settings.empty() ) {
		Global::i()->load();
	} else {
		Global::i()->load( settings );
	}

	// Main message loop:
	manager = new GameManager();
}

void mouseHandler(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse.lbutton = true;
			mouse.lbutton_pressed = true;
		}
		if (state == GLUT_UP) {
			mouse.lbutton = false;
			mouse.lbutton_pressed = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse.rbutton = true;
			mouse.rbutton_pressed = true;
		}
		if (state == GLUT_UP) {
			mouse.rbutton = true;
			mouse.rbutton_pressed = true;
		}
	}
	mouse.x = x;
	mouse.y = y;
}

void mouseMotionHandler(int x, int y) {
	mouse.x = x;
	mouse.y = y;

	glutPostRedisplay();
}

void keyboardDownHandler(unsigned char c, int x, int y) {
	if (c == 27) {
		exit(0);	// escape
	}

	if( !keyboard.keyDown[c] ) {
		keyboard.keyDown[c] = true;
		keyboard.keyPressed[c] = true;
	}

	glutPostRedisplay();
}

void keyboardUpHandler(unsigned char c, int x, int y) {
	keyboard.keyDown[c] = false;
	keyboard.keyPressed[c] = false;

	glutPostRedisplay();
}

void renderScene() {
	try {
		manager->update( mouse, keyboard );
	} catch( std::exception& ex ) {
		std::cerr << ex.what() << std::endl;
		exit(1);
	}
	// shozeni priznaku stisknuti tlacitek mysi a klavesnice
	mouse.lbutton_pressed = mouse.rbutton_pressed = false;
	memset( keyboard.keyPressed, 0, 256 );

	glutSwapBuffers();
}

void idleHandler() {
	glutPostRedisplay();
}

}

int main( int argc, char* argv[] ) {
	GameScreen* screen = nullptr;

	using namespace boost::program_options;
	options_description desc("options");
	desc.add_options()
		( "config,c", value<std::string>(), "Configuration file" )
		( "script,s", value<std::string>(), "Run script" )
		( "room,r", value<std::string>(), "Run room" );

	try {
		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);

		glutInit( &argc, argv );

		std::string configFile;
		if( vm.count("config") ) {
			configFile = vm["config"].as<std::string>();
		}

		init(configFile);

		manager->init();

		if( vm.count("script") ) {
			// explicit script
			ScriptPlayer* scriptPlayer = new ScriptPlayer();
			scriptPlayer->init();
			scriptPlayer->loadScript(vm["script"].as<std::string>());
			screen = scriptPlayer;
		} else if( vm.count("room") ) {
			// explicit room
			RoomPlayer* roomPlayer = new RoomPlayer();
			roomPlayer->init();
			roomPlayer->loadRoom(vm["room"].as<std::string>());
			screen = roomPlayer;
		} else {
			// default
			ScriptPlayer* scriptPlayer = new ScriptPlayer();
			scriptPlayer->init();
			scriptPlayer->prepareScript(Global::i()->globalScript(), Global::i()->globalInitFunction());
			//scriptPlayer->loadScript(Global::i()->initScript());
			screen = scriptPlayer;
		}

		manager->addScreen(screen);
	} catch(std::exception& ex ) {
		std::cerr << "Unknown arguments: " << ex.what() << std::endl;
		return 1;
	}

	// glut
	glutDisplayFunc(renderScene);
	glutMouseFunc(mouseHandler);
	glutMotionFunc(mouseMotionHandler);
	glutPassiveMotionFunc(mouseMotionHandler);
	//glutVisibilityFunc(visible);
	glutKeyboardFunc(keyboardDownHandler);
	glutKeyboardUpFunc(keyboardUpHandler); 
	//glutSpecialFunc(special);
	glutIdleFunc(idleHandler);
	glutSetCursor(GLUT_CURSOR_NONE); 

	glutMainLoop();

	manager->free();
	delete manager;

	return 0;
}
