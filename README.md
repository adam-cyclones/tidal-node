# Tidal Node

Lua was concieved as an embeded language, designed to compliment other languages, Tidal Node is a library which brings Lua to node.js through WebAssembly and c++.

Tidal Node is WIP and not production ready - however I welcome the one or two people in the world that want to use this to get those PR's coming in. You will find most of the emscripten compiler commands in npm package.json "config". There is also a installer task for Lua built in, everything is subject to change.

## Requirements
#### compiler:
emscripten 1.38.30^
#### runtime:
node 10.0.0^
#### build tools:
- make
- curl

## Commands
| command | purpose |
| ------------- | ------------- |
| npm start | just run the project |
| npm run compile | just compile |
| npm run install-lua | download a copy of lua 5.3.5 and build with emscripten |

## What is working
- Return flat Lua tables to node!

### Highlights - "It's alive!"
- Returning flat Lua tables to JavaScript as Objects
- Use module.exports and require like node.js
- All lua files are modules by default and return a Lua table - Like node where all files return a JavaScript Object

### Roadmap
- Investigate calling Libuv from Lua, interop between JavaScript and Lua and IPC.
- Create running examples, maybe a lua Webserver and a Game engine port.
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
