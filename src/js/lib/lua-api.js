const { dirname, resolve } = require('path');
const toAsyncConstructor = require('./async-constructor');

class LuaApi {
    constructor (customConfig, next) {
        if (customConfig === null) {
            customConfig = {};
        }

        const PROJECT_ROOT = customConfig.projectRoot || dirname(require.main.filename || process.mainModule.filename);
        const BINARY_DIR = "wasm/";
        const EM_GLUE_NAME = "main";
        const WASM_MODULE = require(resolve(PROJECT_ROOT, BINARY_DIR, EM_GLUE_NAME ));

        ;(async ()=>{
            this.L = await new Promise(res => {
                WASM_MODULE.onRuntimeInitialized = () => {
                    res(WASM_MODULE)
                }
            })
            // tell c++ and lua about this projects root.
            try {
                this.L.set_project_root(PROJECT_ROOT);
            }
            catch (err) {
                console.log(err);
            }
            next(this);
        })();
    }
    /**
     * [Add a path to the lua package.path variable. Set a root path for lua require (see example)]
     * @param  {String} path 
     */
    doString (script) {
        return this.L.lua_run_string(script);
    }
    /**
     * [Loads and runs the given file]
     * @type {String} file 
     * @throws {Exception} 
     */
    doFile (pathToFile) {
        const fromLua = this.L.lua_run_file(pathToFile);
        // handle single values
        const len = Object.keys(fromLua).length;

        // handle tables
        for (const entry of Object.entries(fromLua)) {

            if (typeof entry[1] === "function") {

                const key = entry[0];
                // rename for clarity
                Object.defineProperty(fromLua[key], "name", {
                    value: key+"_lua_function"
                });
                fromLua[key] = new Proxy(fromLua[key], {
                    apply: (target, thisArg, argumentsList) => {
                        // An object acting as an array
                        for (let arg of Array.from(argumentsList)) {
                            switch (typeof arg) {
                                case "string":
                                    this.L.push_fn_arg_string(arg);
                                    break
                                case "number":
                                    if (parseFloat(arg) % 1 === 0) {
                                        this.L.push_fn_arg_int(arg);
                                    }
                                    else {
                                        this.L.push_fn_arg_double(arg);
                                    }
                                    break
                                case "boolean":
                                    this.L.push_fn_arg_bool(arg);
                                    break
                                case "object":
                                    if (arg === null) {
                                        this.L.push_fn_arg_string("null");
                                    }
                                    break
                                case "undefined":
                                    this.L.push_fn_arg_string("undefined");
                                    break
                                default:
                                    throw new Error("cannot handle type of argument " + typeof arg)
                            }
                        }
                        // Cleanup
                        const luaReturned = this.L.lua_call_function(pathToFile, key);

                        switch (luaReturned.value) {
                            case "null":
                                return null;
                            default:
                                return luaReturned.value;
                        }

                    }
                })
            }
        }

        if (len === 1) {
            return fromLua[Object.keys(fromLua)[0]];
        }
        else if (len === 0) {
            return null;
        }

        return fromLua;
    }
    /**
     * [Loads and runs the given string]
     * @type {String} str 
     * @throws {Exception} 
     */
    addPackagePath () {

    }
    /**
     * [Sets the function f as the new value of global name]
     * @param  {String} name [name of the global in lua]
     * @param  {Function} f [function to set]
     */
    registerFunction () {

    }
    /**
     * [Pops a value from the stack and sets it as the new value of global name]
     * @type {String} name 
     */
    setGlobal () {

    }
    /**
     * [Pushes onto the stack the value of the global name]
     * @type {String} name 
     */
    getGlobal () {

    }
}

module.exports = toAsyncConstructor(LuaApi);