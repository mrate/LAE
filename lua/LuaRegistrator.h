#pragma once

#include <luabind/luabind.hpp>

namespace LAE {

class TextStorage;

namespace lua {

#define LUA_RESUME_FUNCTION(__L,__func,...) { try { luabind::resume_function<void>(__L, __func, ##__VA_ARGS__); } catch( luabind::error& e ) { lua::handleError(e); } }
#define LUA_RESUME(__L,...) { try { luabind::resume<void>(__L, ##__VA_ARGS__); } catch( luabind::error& e ) { lua::handleError(e); } }

lua_State* createNewState();
void handleError( const luabind::error& e );

void loadScript( lua_State* L, const char* fname );
void registerTexts( lua_State* L, const TextStorage& texts );

}

}
