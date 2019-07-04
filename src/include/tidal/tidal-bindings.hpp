
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
	// emscripten::function("clear_fn_args", &clear_fn_args);
};