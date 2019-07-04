#define SOL_CHECK_ARGUMENTS 1
#define SOL_ALL_SAFETIES_ON 1

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream> //fs
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten.h>
#include <string>
#include <vector>
#include <stdarg.h>
#include "../tmp/lua-5.3.5/src/lua.hpp"
#include "./include/sol2/sol.hpp"
#include <cassert>
#include <typeinfo>

// Lua
lua_State* L = luaL_newstate();
sol::state_view lua(L);

// Tidal
#include "./include/tidal/tidal-constants.hpp"
#include "./include/tidal/tidal-utils.hpp"
#include "./include/tidal/tidal-lua-funcs.hpp"
#include "./include/tidal/tidal-initialize.hpp"

using namespace tidal;
using namespace std;
using namespace emscripten;

#include "./include/tidal/tidal-type-convertion.hpp"

// TODO: refactor to have resolving handlers inside module.lua
/**
 * cpp require handler
 * makes paths absolute depending on how we called require
 * @param string
*/
auto require (string path_to_file, bool isMain = false)
{
	// Load tidal Module file contents
	ifstream t(PROJECT_ROOT + SEP + "src" + SEP + "lua" + SEP + "lib" + SEP + "module.lua");
	stringstream buffer;
	buffer << t.rdbuf();
	string tidal_module_bootstrap = buffer.str();

	// Bootstrap Tidal to Lua
	// * Replaces require with a node.js like module system and other usefull code
	luaL_dostring(L, tidal_module_bootstrap.c_str());

	const int last_index_of_ext_sep = path_to_file.find_last_of(EXT_SEP);
	string rawname = path_to_file.substr(0, last_index_of_ext_sep);

	// Is Relative
	if ( (path_to_file[0] == '.' && path_to_file[0] != SEP) )
	{
		string clean_rawname = rawname.substr(1, rawname.length());
		const string absolute_path_to_file = PROJECT_ROOT + clean_rawname + LUA_EXTNAME;
		// absolute_path_to_file is now safe to check for existance
		cout << Error_resolve_module(path_to_file, resolve_R) << endl;
	}
	// Is Absolute
	else if (path_to_file[0] == SEP)
	{
		const string absolute_path_to_file = rawname + LUA_EXTNAME;
		// absolute_path_to_file is now safe to check for existance
		if (fexists(absolute_path_to_file))
		{
			// Load Lua file and inject locals
			return tidal_require(absolute_path_to_file, isMain);
		}
		else
		{
			cout << Error_resolve_module(path_to_file, resolve_A) << endl;
		}
	}
	// Is package lookup
	else if (path_to_file.find(SEP) == string::npos)
	{
		// TODO: lookup lua_modules and or package path
		const string lookup = path_to_file;
		// Get lua path
		lua_getglobal( L, "package" );
		lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
		string lua_path = lua_tostring( L, -1 );
		// Error logging
		cout << Error_resolve_module(lookup, resolve_L, lua_path) << endl;
		cout << Warn()+" Lua package lookup is not implimented yet. requested lookup '"+lookup+"' will return nil \n" << endl;
	}
	// never reached but removes error
}

int main(int argc, char *argv[])
{
	cout << argv[1] << endl;
}

// lua_run_file
val lua_run_file (const string &path_to_file)
{
	luaL_openlibs(L);
	// Bootstrap new require
	lua.set_function("require", &require);
	lua.set_function("printT", &print);

	cout << path_to_file << endl;

	auto file_exports = lua.script("return require('"+ path_to_file +"', true);", [](lua_State*, sol::protected_function_result pfr) {
		return pfr;
	});

	assert(file_exports.valid());

	sol::table cast_return_value = (sol::table)file_exports;
	val table_transport_js_object = lua_table_to_js_obj(cast_return_value);

	return table_transport_js_object;
}

#include "./include/tidal/tidal-ffi.hpp"
#include "./include/tidal/tidal-bindings.hpp"

