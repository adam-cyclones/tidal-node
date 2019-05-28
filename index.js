const Module = require('./wasm/main');
const { resolve, dirname } = require('path');

const toAsyncConstructor = (Target) => new Proxy(class AwaitCtor{
    static async new(...args) {
        return new Promise(next => {
            Reflect.construct(Target, [...args, next]);
        });
    }
},
{
    construct() {
        const className = Target.prototype.constructor.name;
        throw new Error(`${className} has been cast to an async only constructor, use ${className}.new() to construct this class.`);
    }
});

class LuaApi {
    constructor (wasmModule = require('./wasm/main'), next) {
        ;(async ()=>{
            this.L = await new Promise(res => {
                wasmModule.onRuntimeInitialized = () => {
                    res(wasmModule)
                }
            })
            // tell c++ and lua about this projects root.
            try {
                this.L.set_project_root(dirname(require.main.filename || process.mainModule.filename));
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
        return this.L.lua_run_file(pathToFile);
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
    /**
     * [Does the equivalent to t[k] = v, where t is the value at the given valid index and v is the value at the top of the stack. This function pops the value from the stack.]
     * @type {Number} index 
     * @type {String} key 
     */
    setTableField () {

    }
    /**
     * [Pushes onto the stack the value t[key], where t is the value at the given valid index.]
     * @type {Number} index 
     * @type {String} key 
     */
    getTableField () {

    }
    /**
     * [Get value at the given acceptable index]
     * @type {Number} index 
     * @return value
     */
    toValue () {

    }
    /**
     * [Calls a function. Gets the function and arguments from the stack. Pushes the results onto the stack. See https://www.lua.org/manual/5.1/manual.html#pdf-pcall for more information]
     * @type {Number} args
     * @type {Number} results
     * @throws {Exception}
     */
    call () {

    }
    /**
     * [Yields a coroutine.]
     * @type {Number} args 
     */
    yield () {

    }
    /**
     * [Starts and resumes a coroutine in a given thread.]
     * @type {Number} args 
     */
    resume () {

    }
    /**
     * [Pushes a value n onto the stack]
     * @type n
     */
    push () {

    }
    /**
     * [Pops n elements from the stack. Default value is 1]
     * @type {Number} n 
     */
    pop () {

    }
    /**
     * [Returns the index of the top element in the stack. Because indices start at 1, this result is equal to the number of elements in the stack (and so 0 means an empty stack)]
     * @return {Number} 
     */
    getTop () {

    }
    /**
     * [Accepts any acceptable index, or 0, and sets the stack top to this index. If the new top is larger than the old one, then the new elements are filled with nil. If index is 0, then all stack elements are removed]
     * @type {Number} index 
     */
    setTop () {

    }
    /**
     * [Moves the top element into the given position (and pops it), without shifting any element (therefore replacing the value at the given position)]
     * @type {Number} index 
     */
    replace() {

    }
}

const Lua = toAsyncConstructor(LuaApi);

;(async ()=>{

    const L = await Lua.new(Module);


    // L.doFile("./src/hello.lua")
    console.log('lua returned:', L.doFile("/Users/acrockett/Code/Lua/lua-wasm/src/hello.lua"));

})()