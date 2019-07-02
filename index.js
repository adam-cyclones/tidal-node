const Lua = require('./src/js/tidal-node.js');

;(async ()=>{

    const L = await Lua.new();

    const value = L.doFile("/Users/acrockett/Code/Lua/tidal-node/src/hello.lua")
    console.log(value)

})();