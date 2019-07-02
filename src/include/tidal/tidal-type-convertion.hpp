

emscripten::val lua_table_to_js_obj (sol::table &table)
{
	emscripten::val table_transport_js_object = emscripten::val::object();

	for (auto& kv : (sol::table) table)
	{
		if (kv.first.get_type() == sol::type::string)
		{
			// Get table key as string.
			string key = kv.first.as<std::string>();
      auto lua_type = kv.second.get_type();

			// Handle value types
			if (lua_type == sol::type::string)
			{
				auto value = kv.second.as<std::string>();
				table_transport_js_object.set(key, value);
			}
			else if (lua_type == sol::type::number)
			{
				auto value = kv.second.as<double>();
				table_transport_js_object.set(key, value);
			}
			else if (lua_type == sol::type::boolean)
			{
				auto value = kv.second.as<bool>();
				table_transport_js_object.set(key, value);
			}
			else if (lua_type == sol::type::function)
			{
				// create a stub to be proxied in JavaScript
				emscripten::val blank_function = val::global("Function").new_();
				table_transport_js_object.set(key, blank_function);
			}
			else if (lua_type == sol::type::table)
			{
				auto value = kv.second.as<sol::table>();
				table_transport_js_object.set(key, lua_table_to_js_obj(value));
			}
			else
			{
				table_transport_js_object.set(key, -1);
			}
		}

		if (kv.first.get_type() == sol::type::number)
		{
			// Get table key as number.
			lua_Number index = kv.first.as<lua_Number>();
			// Handle value typess
			if (kv.second.get_type() == sol::type::string)
			{
				auto value = kv.second.as<std::string>();
				table_transport_js_object.set(index, value);
			}
			else if (kv.second.get_type() == sol::type::number)
			{
				auto value = kv.second.as<double>();
				table_transport_js_object.set(index, value);
			}
			else if (kv.second.get_type() == sol::type::boolean)
			{
				auto value = kv.second.as<bool>();
				table_transport_js_object.set(index, value);
			}
			else if (kv.second.get_type() == sol::type::function)
			{
				// create a stub to be proxied in JavaScript
				val blank_function = val::global("Function").new_();
				table_transport_js_object.set(index, blank_function);
			}
			else if (kv.second.get_type() == sol::type::table)
			{
				auto value = kv.second.as<sol::table>();
				table_transport_js_object.set(index, lua_table_to_js_obj(value));
			}
			else
			{
				table_transport_js_object.set(index, -1);
			}
		}
	}

	return table_transport_js_object;
}