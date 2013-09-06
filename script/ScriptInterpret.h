#pragma once
#ifndef _SCRIPTINTERPRET_H_
#define _SCRIPTINTERPRET_H_

// stl
#include <string>
// LUA
#include "../lua/lua.h"
#include "../lua/LuaRegistrator.h"

namespace LAE {

class ScriptContext {
public:
	ScriptContext();
	~ScriptContext();

	lua_State* getState();

	lua_State* getMain() { return mainState_; }
	lua_State* getThread() { return threadState_; }

private:
	lua_State* mainState_;
	lua_State* threadState_;
};

class ScriptInterpret {
public:
	ScriptInterpret();
	~ScriptInterpret();

	ScriptContext* createNewContext() const;

	void loadFile( ScriptContext* context, const std::string& fileName ) const;

	template<typename T>
	void registerObject( ScriptContext* context, const std::string& name, T obj ) const {
		luabind::globals(context->getState())[name] = obj;
	}

	void resume( ScriptContext* context ) {
		LUA_RESUME( context->getState() );
	}

	template<typename T>
	void resume( ScriptContext* context, T p ) {
		LUA_RESUME( context->getState(), p );
	}

	void runFunction( ScriptContext* context, const std::string& functionName ) {
		LUA_RESUME_FUNCTION( context->getState(), functionName.c_str() );
	}

	template<typename T>
	void runFunction( ScriptContext* context, const std::string& functionName, T p ) {
		LUA_RESUME_FUNCTION( context->getState(), functionName.c_str(), p );
	}

	template<typename T1, typename T2>
	void runFunction( ScriptContext* context, const std::string& functionName, T1 p1, T2 p2 ) {
		LUA_RESUME_FUNCTION( context->getState(), functionName.c_str(), p1, p2 );
	}

	template<typename T1, typename T2, typename T3>
	void runFunction( ScriptContext* context, const std::string& functionName, T1 p1, T2 p2, T3 p3 ) {
		LUA_RESUME_FUNCTION( context->getState(), functionName.c_str(), p1, p2, p3 );
	}
};

}

#endif // _SCRIPTINTERPRET_H_
