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

// val lua_table_to_js_obj (sol::table &table)
// {
// 	val table_transport_js_object = val::object();

// 	for (auto& kv : (sol::table) table)
// 	{
// 		if (kv.first.get_type() == sol::type::string)
// 		{
// 			// Get table key as string.
// 			string key = kv.first.as<string>();

// 			// Handle value types
// 			if (kv.second.get_type() == sol::type::string)
// 			{
// 				auto value = kv.second.as<string>();
// 				table_transport_js_object.set(key, value);
// 			}
// 			else if (kv.second.get_type() == sol::type::number)
// 			{
// 				auto value = kv.second.as<double>();
// 				table_transport_js_object.set(key, value);
// 			}
// 			else if (kv.second.get_type() == sol::type::boolean)
// 			{
// 				auto value = kv.second.as<bool>();
// 				table_transport_js_object.set(key, value);
// 			}
// 			else if (kv.second.get_type() == sol::type::function)
// 			{
// 				// create a stub to be proxied in JavaScript
// 				val blank_function = val::global("Function").new_();
// 				table_transport_js_object.set(key, blank_function);
// 			}
// 			else if (kv.second.get_type() == sol::type::table)
// 			{
// 				auto value = kv.second.as<sol::table>();
// 				table_transport_js_object.set(key, lua_table_to_js_obj(value));
// 			}
// 			else
// 			{
// 				table_transport_js_object.set(key, -1);
// 			}
// 		}

// 		if (kv.first.get_type() == sol::type::number)
// 		{
// 			// Get table key as number.
// 			lua_Number index = kv.first.as<lua_Number>();
// 			// Handle value typess
// 			if (kv.second.get_type() == sol::type::string)
// 			{
// 				auto value = kv.second.as<string>();
// 				table_transport_js_object.set(index, value);
// 			}
// 			else if (kv.second.get_type() == sol::type::number)
// 			{
// 				auto value = kv.second.as<double>();
// 				table_transport_js_object.set(index, value);
// 			}
// 			else if (kv.second.get_type() == sol::type::boolean)
// 			{
// 				auto value = kv.second.as<bool>();
// 				table_transport_js_object.set(index, value);
// 			}
// 			else if (kv.second.get_type() == sol::type::function)
// 			{
// 				// create a stub to be proxied in JavaScript
// 				val blank_function = val::global("Function").new_();
// 				table_transport_js_object.set(index, blank_function);
// 			}
// 			else if (kv.second.get_type() == sol::type::table)
// 			{
// 				auto value = kv.second.as<sol::table>();
// 				table_transport_js_object.set(index, lua_table_to_js_obj(value));
// 			}
// 			else
// 			{
// 				table_transport_js_object.set(index, -1);
// 			}
// 		}
// 	}

// 	return table_transport_js_object;
// }

// TODO: refactor to have resolving handlers inside module.lua
/**
 * cpp require handler
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
	// * Replaces require with a node.js clone
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

// lua_run_file
val lua_run_file (const string &path_to_file)
{
	luaL_openlibs(L);
	// Bootstrap new require
	lua.set_function("require", &require);

	cout << path_to_file << endl;

	auto compile_lua_module = lua.load("return require('"+ path_to_file +"', true);");
	auto file_exports = compile_lua_module();

	sol::table cast_return_value = (sol::table)file_exports;
	val table_transport_js_object = lua_table_to_js_obj(cast_return_value);

	return table_transport_js_object;
}

#include "./include/tidal/tidal-ffi.hpp"
#include "./include/tidal/tidal-bindings.hpp"

