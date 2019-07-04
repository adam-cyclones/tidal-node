// FFI Javascript to Lua function calls
std::vector<std::variant<std::string, char, int, double, bool>> tmp_store_fn_args;

// FFI handle args in
void push_fn_arg_int (int arg)
{
	tmp_store_fn_args.push_back(arg);
}
void push_fn_arg_double (double arg)
{
	tmp_store_fn_args.push_back(arg);
}
void push_fn_arg_string (std::string arg)
{
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
void push_fn_arg_bool (bool arg)
{
	tmp_store_fn_args.push_back(arg);
}
void clear_fn_args ()
{
	tmp_store_fn_args.clear();
}

emscripten::val lua_call_function (std::string path_to_file, std::string func_name)
{
	int index = 0;
	emscripten::val returns_transport_js_object = emscripten::val::object();
	sol::table lua_call_function_args_to_lua = lua.create_table();

	for (const auto& js_value : tmp_store_fn_args)
	{
		std::visit([&](const auto &value) {
			cout << value << " CPP" << index << endl;
			index += 1;
			lua_call_function_args_to_lua.set(index, value);
		}, js_value);
	}


	// Store a temp global in lua with the args from JavaScript
	lua["__lua_call_function_args"] = lua_call_function_args_to_lua;

	const auto& initiateLuaFnCallByLookup = R"(
		local args = table.unpack(__lua_call_function_args);
		return require.__cache[')" + path_to_file + "'].exports['" + func_name + "'](args)" + R"(
	)";

	auto call_fn = lua.load(initiateLuaFnCallByLookup);
	// Handle errors to lookup the function
	if (!call_fn.valid()) {
		sol::error err = call_fn;
		std::cerr << "failed to load string-based script in the program" << err.what() << std::endl;
	}
	auto lua_return_value = call_fn();
	// Teardown
	clear_fn_args();

	// Handle returns
	string lua_return_typename = luaL_typename(L, -1);
	if (lua_return_typename == "string")
	{
		string cast_return_value = (string)lua_return_value;
		returns_transport_js_object.set("value", cast_return_value);
	}
	else if (lua_return_typename == "number")
	{
		double cast_return_value = (double)lua_return_value;
		returns_transport_js_object.set("value", cast_return_value);
	}
	else if (lua_return_typename == "nil")
	{
		returns_transport_js_object.set("value", "null");
	}
	else if (lua_return_typename == "table")
	{
		sol::table cast_return_value = (sol::table)lua_return_value;
		returns_transport_js_object.set("value", lua_table_to_js_obj(cast_return_value));
	}
	else
	{ // bool
		bool cast_return_value = (bool)lua_return_value;
		returns_transport_js_object.set("value", cast_return_value);
	}

	return returns_transport_js_object;
}