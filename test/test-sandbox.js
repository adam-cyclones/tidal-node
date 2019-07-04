const { resolve } = require('path');
const Lua = require('../src/js/tidal-node.js');
const LUA_PATH = resolve(__dirname);
const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_PATH, "test-sandbox.lua");


;(async ()=>{
  const TEST_PROJECT_ROOT = resolve(__dirname, "../");
  const LuaVM = Lua.new({ projectRoot: TEST_PROJECT_ROOT });
  global.L = await LuaVM;

  const echo = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
  echo(1, 'a', 2)

})();