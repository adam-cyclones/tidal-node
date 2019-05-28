# wasm-lua-node-tidal

Welcome to my experiment, you will find most of the emscripten compiler commands in npm package.json "config". There is also a installer task for lua built in.

### Requirements
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

### What isnt working.
- returning strings and numbers (simple TODO)

### To do
- Return lua tables to node!
- Returning strings
- Returning complex data
- Returning functions
- Functions from Lua
- Tables from Lua to JavaScript objects

[Inspiration | https://github.com/vvanders/wasm_lua] by the fantastic @vvanders