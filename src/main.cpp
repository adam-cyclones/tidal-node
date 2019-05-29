#define SOL_CHECK_ARGUMENTS 1
#define SOL_ALL_SAFETIES_ON 1

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream> //fs
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten.h>
#include "../tmp/lua-5.3.5/src/lua.hpp"
#include "./include/sol2/sol.hpp"
#include <cassert>
#include <typeinfo>
#include "./include/tidal/tidal-constants.hpp"
#include "./include/tidal/tidal-utils.hpp"

using namespace tidal;
using namespace std;
using namespace emscripten;

// Globals
lua_State* L = luaL_newstate();
sol::state_view lua(L);
string PROJECT_ROOT;
const char SEP = '/';

void set_project_root (const string project_root) {
	PROJECT_ROOT = project_root;
}

typedef std::variant<string, int> alpha_numeric_key;
typedef std::variant<string, double, bool> lua_table_values;
typedef map<alpha_numeric_key, lua_table_values> lua_table_map;
// tidal_require
sol::table tidal_require (string path_to_file, bool isMain = false)
{
	const int last_index_of_sep = path_to_file.find_last_of(SEP);
	string dirname = path_to_file.substr(0, last_index_of_sep);
	// designed to store a lua table with keys
	map<variant<string, int>, variant<
		string,
		double,
		bool
	>> table_transport;

	val table_transport_js_object = val::object();

	// Call modified Lua require
	auto file_exports = lua["require"](path_to_file);

	return file_exports;
}

// TODO: refactor to have resolving handlers inside module.lua
/**
 * cpp require handler
 * @param string
*/
auto require (string path_to_file, bool isMain = false)
{
	// Load tidal Module file contents
	ifstream t(PROJECT_ROOT + SEP + "src" + SEP + "module.lua");
	stringstream buffer;
	buffer << t.rdbuf();
	string tidal_module_bootstrap = buffer.str();

	// Bootstrap Tidal to Lua
	// * Replaces require with a node.js clone
	luaL_dostring(L, tidal_module_bootstrap.c_str());

	const char EXT_SEP = '.';
	const string LUA_EXTNAME = ".lua";


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
}

// string lua_run_string(string script) {
// 	luaL_openlibs(L);
	
// 	int res = luaL_dostring(L, script.c_str());

// 	size_t len;
// 	string value = lua_tolstring(L, lua_gettop(L), &len);

// 	return value;
// }

// lua_run_file
val lua_run_file (const string &path_to_file)
{
	luaL_openlibs(L);
	// Bootstrap new require
	lua.set_function("require", &require);

	auto file_exports = lua.script("return require('"+ path_to_file +"', true);");

	val table_transport_js_object = val::object();

	// if (type_name(L, file_exports) == "string") {
	// 	return (string)file_exports;
	// }
	// if (type_name(L, file_exports) == "number") {
	// 	return (int)file_exports;
	// }
	if (type_name(L, file_exports) == "table") {

		for (auto& kv : (sol::table) file_exports)
		{
			if (kv.first.get_type() == sol::type::string)
			{
				// Get table key as string.
				string key = kv.first.as<string>();
				cout << key << endl;

				// Handle value typess
				if (kv.second.get_type() == sol::type::string)
				{
					auto value = kv.second.as<string>();
					cout << value << endl;
					table_transport_js_object.set(key, value);
				}
				if (kv.second.get_type() == sol::type::number)
				{
					auto value = kv.second.as<double>();
					cout << value << endl;
					table_transport_js_object.set(key, value);

				}
				if (kv.second.get_type() == sol::type::boolean)
				{
					auto value = kv.second.as<bool>();
					cout << value << endl;
					table_transport_js_object.set(key, value);
				}
				if (kv.second.get_type() == sol::type::table)
				{
					// recurse
				}
			}

			if (kv.first.get_type() == sol::type::number)
			{
				// Get table key as number.
				lua_Number index = kv.first.as<lua_Number>();
				cout << "index: " << index << endl;
				// Handle value typess
				if (kv.second.get_type() == sol::type::string)
				{
					auto value = kv.second.as<string>();
					cout << value << endl;
					table_transport_js_object.set(index, value);
				}
				if (kv.second.get_type() == sol::type::number)
				{
					auto value = kv.second.as<double>();
					cout << value << endl;
					table_transport_js_object.set(index, value);
				}
				if (kv.second.get_type() == sol::type::boolean)
				{
					auto value = kv.second.as<bool>();
					cout << value << endl;
					table_transport_js_object.set(index, value);
				}
				if (kv.second.get_type() == sol::type::table)
				{
					// recurse
				}
			}

			// You can access the value of this pair using `kv.second()`.
		}
	}

	// create a new JavaScript Object
	val lua_table_values = val::object();
	lua_table_values.set("hello", "world");


	return table_transport_js_object;
}

EMSCRIPTEN_BINDINGS (my_module) {
	emscripten::function("set_project_root", &set_project_root);
	// emscripten::function("lua_run_string", &lua_run_string);
	emscripten::function("lua_run_file", &lua_run_file);
};
