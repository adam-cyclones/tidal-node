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
	// never reached but removes error
}

// string lua_run_string(string script) {
// 	luaL_openlibs(L);
	
// 	int res = luaL_dostring(L, script.c_str());

// 	size_t len;
// 	string value = lua_tolstring(L, lua_gettop(L), &len);

// 	return value;
// }

val lua_table_to_js_obj (sol::table &table) {
	val table_transport_js_object = val::object();

	for (auto& kv : (sol::table) table)
	{
		if (kv.first.get_type() == sol::type::string)
		{
			// Get table key as string.
			string key = kv.first.as<string>();

			// Handle value types
			if (kv.second.get_type() == sol::type::string)
			{
				auto value = kv.second.as<string>();
				table_transport_js_object.set(key, value);
			}
			if (kv.second.get_type() == sol::type::number)
			{
				auto value = kv.second.as<double>();
				table_transport_js_object.set(key, value);
			}
			if (kv.second.get_type() == sol::type::boolean)
			{
				auto value = kv.second.as<bool>();
				table_transport_js_object.set(key, value);
			}
			if (kv.second.get_type() == sol::type::function)
			{
				// create a stub to be proxied in JavaScript
				val blank_function = val::global("Function").new_();
				table_transport_js_object.set(key, blank_function);
			}
			if (kv.second.get_type() == sol::type::table)
			{
				auto value = kv.second.as<sol::table>();
				table_transport_js_object.set(key, lua_table_to_js_obj(value));
			}
		}

		if (kv.first.get_type() == sol::type::number)
		{
			// Get table key as number.
			lua_Number index = kv.first.as<lua_Number>();
			// Handle value typess
			if (kv.second.get_type() == sol::type::string)
			{
				auto value = kv.second.as<string>();
				table_transport_js_object.set(index, value);
			}
			if (kv.second.get_type() == sol::type::number)
			{
				auto value = kv.second.as<double>();
				table_transport_js_object.set(index, value);
			}
			if (kv.second.get_type() == sol::type::boolean)
			{
				auto value = kv.second.as<bool>();
				table_transport_js_object.set(index, value);
			}
			if (kv.second.get_type() == sol::type::function)
			{
				// create a stub to be proxied in JavaScript
				val blank_function = val::global("Function").new_();
				table_transport_js_object.set(index, blank_function);
			}
			if (kv.second.get_type() == sol::type::table)
			{
				// recurse
			}
		}
	}

	return table_transport_js_object;
}

// lua_run_file
val lua_run_file (const string &path_to_file)
{
	luaL_openlibs(L);
	// Bootstrap new require
	lua.set_function("require", &require);

	auto compile_lua_module = lua.load("return require('"+ path_to_file +"', true);");
	auto file_exports = compile_lua_module();

	sol::table cast_return_value = (sol::table)file_exports;
	val table_transport_js_object = lua_table_to_js_obj(cast_return_value);

	return table_transport_js_object;
}

// FFI Javascript to Lua function calls
vector<std::variant<string, char, int, double, bool>> tmp_store_fn_args;

// FFI handle args in
void push_fn_arg_int (int arg) {
	tmp_store_fn_args.push_back(arg);
}
void push_fn_arg_double (double arg) {
	tmp_store_fn_args.push_back(arg);
}
void push_fn_arg_string (string arg) {
	if (arg == "null") {
		tmp_store_fn_args.push_back(-1);
	}
	else if (arg == "undefined") {
		tmp_store_fn_args.push_back(-1);
	}
	else {
		tmp_store_fn_args.push_back(arg);
	}
}
void push_fn_arg_bool (bool arg) {
	tmp_store_fn_args.push_back(arg);
}

val lua_call_function (string path_to_file, string func_name) {
	// tables ruturned by lua which contain functions are represented by emscripten blank functions
	// the function from the JavaScript side will be substitued with a Proxy, the proxy will be passed -
	// the file name, function name, and args
	// the function will have already been stored in a lua global table
	// the table will be have a key of path_to_file and that function
	// the call to that function will provide the arguments from here
	// the return values will be passed back to here and returned to JavaScript

	val returns_transport_js_object = val::object();
	sol::table lua_call_function_args_to_lua = lua.create_table();
	int index = 0;

	for (const auto& js_value : tmp_store_fn_args) {
		std::visit([&](const auto &value) {
			index += 1;
			lua_call_function_args_to_lua.set(index, value);
		}, js_value);
	}


	// Store a temp global in lua with the args from JavaScript
	lua["lua_call_function_args"] = lua_call_function_args_to_lua;

	auto call_fn = lua.load("return require.__cache['"+ path_to_file +"'].exports['"+ func_name +"'](table.unpack(lua_call_function_args))");

	auto lua_return_value = call_fn();
	string lua_return_typename = luaL_typename(L, -1);
	if (lua_return_typename == "string") {
		string cast_return_value = (string)lua_return_value;
		returns_transport_js_object.set("value", cast_return_value);
	}
	else if (lua_return_typename == "number") {
		double cast_return_value = (double)lua_return_value;
		returns_transport_js_object.set("value", cast_return_value);
	}
	else if (lua_return_typename == "nil") {
		returns_transport_js_object.set("value", "null");
	}
	else if (lua_return_typename == "table") {
		sol::table cast_return_value = (sol::table)lua_return_value;
		returns_transport_js_object.set("value", lua_table_to_js_obj(cast_return_value));
	}
	else { // bool
		bool cast_return_value = (bool)lua_return_value;
		returns_transport_js_object.set("value", cast_return_value);
	}

	return returns_transport_js_object;
}

EMSCRIPTEN_BINDINGS (tidal_node) {
	emscripten::register_vector<int>("vector<int>");
	emscripten::function("set_project_root", &set_project_root);
	// emscripten::function("lua_run_string", &lua_run_string);
	emscripten::function("lua_run_file", &lua_run_file);

	// FFI
	emscripten::function("lua_call_function", &lua_call_function);
	emscripten::function("push_fn_arg_string", &push_fn_arg_string);
	emscripten::function("push_fn_arg_int", &push_fn_arg_int);
	emscripten::function("push_fn_arg_double", &push_fn_arg_double);
	emscripten::function("push_fn_arg_bool", &push_fn_arg_bool);
};
