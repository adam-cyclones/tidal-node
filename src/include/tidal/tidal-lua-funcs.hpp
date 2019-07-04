sol::table tidal_require (const std::string&  path_to_file, bool isMain = false)
{
	const int last_index_of_sep = path_to_file.find_last_of(tidal::SEP);
	std::string dirname = path_to_file.substr(0, last_index_of_sep);
	// Call modified Lua require
	// auto file_exports = lua["require"](path_to_file);

	const std::string exec_require = "return require('" + path_to_file + "')";
	auto file_exports = lua.script(exec_require, [](lua_State*, sol::protected_function_result pfr) {
		return pfr;
	});

	assert(file_exports.valid());

	return file_exports;
}

/**
 * Enhanced Lua print function replaces lua print
*/
void print (const std::string& str)
{
	std::cout << "Lua: " << str << std::endl;
}