#include "ScriptInterpret.h"

namespace LAE {

ScriptInterpret::ScriptInterpret() {

}

ScriptInterpret::~ScriptInterpret() {

}

ScriptContext* ScriptInterpret::createNewContext() const {
	return new ScriptContext();
}

void ScriptInterpret::loadFile( ScriptContext* context, const std::string& fileName ) const {
	lua::loadScript( context->getState(), fileName.c_str() );
}


ScriptContext::ScriptContext() {
	mainState_ = lua::createNewState();
	threadState_ = lua_newthread( mainState_ );
}

ScriptContext::~ScriptContext() {
	lua_close( mainState_ );
}

lua_State* ScriptContext::getState() {
	return threadState_;
}

}
