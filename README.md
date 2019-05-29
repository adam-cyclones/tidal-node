# wasm-lua-node-tidal

Imagine a world where Lua was adopted and popular. I think Lua is like JavaScript only much much faster, samller and elegent. Lua surely desearves some love. 

Lua was concieved as an embeded language, designed to compliment other languages, a simple scripting language for compiled platforms. I can see this having a use for those needing performance and multithreading but in a way that isnt 100 miles away from JavaScript. Trust me Lua is so small and sort of firmilair you can be up and running in around an hour.

This is a web assembly project, it's WIP and has not been tested on another machines yet, you will find most of the emscripten compiler commands in npm package.json "config". There is also a installer task for Lua built in.

## Requirements
#### compiler:
emscripten 1.38.30^
#### runtime:
node 10.0.0^
#### build tools:
- make
- curl

### Building
1. `npm run install-lua`
The task install-lua will go ahead and download a copy of lua 5.3.5 which is the latest stable.
Compile with emscripten.
Rewire the emscripten js gluecode.
Run `index.js`

#### Compile only
To just compile use `npm run compile`.
#### run only
To just run use `npm start`

---

## What is working
- Return flat Lua tables to node!

### Highlights - "It's alive!"
- Returning flat Lua tables to JavaScript as Objects
- Use module.exports and require like node.js
- All lua files are modules by default and return a Lua table - Like node where all files return a JavaScript Object

### Todo
- Refactor cleanup optimize repeat
- Unit testing with Jest
- Return strings
- Return nil as null
- Return dobules as number
- Return nested tables
- Returning functions ?!
- Feature toggles (how much magic do you want?)
- Proxy node require to safely load Lua in the same way we load JavaScript, (feature toggle)
- Change from platform inrto library distributable format
- Smarter build system
- OS support - (developed on Mac Os)
- Beanchmarking
- Improve nodelike Lua require to search paths lua_modules
- Integrate lua-rocks and wrap so that installs are more node like
- Test and validate fresh installs and install scripts
- Reduce and remove c++ dependencies where posible
- General repo management
- Create branding, website docs and RC.alpha-1

[Inspiration | https://github.com/vvanders/wasm_lua] by the fantastic @vvanders