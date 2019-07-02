sol::table tidal_require (std::string path_to_file, bool isMain = false)
{
	const int last_index_of_sep = path_to_file.find_last_of(tidal::SEP);
	std::string dirname = path_to_file.substr(0, last_index_of_sep);
	// designed to store a lua table with keys
	map<variant<std::string, int>, variant<std::string, double, bool>> table_transport;

	emscripten::val table_transport_js_object = emscripten::val::object();

	// Call modified Lua require
	auto file_exports = lua["require"](path_to_file);

	return file_exports;
}