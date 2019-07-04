-- util
function split(s, delimiter)
    result = {};
    for match in (package.path..";"):gmatch("(.-)"..delimiter) do
        table.insert(result, match);
    end
    return result;
end

function lastIndexOf(haystack, needle)
    local i=haystack:match(".*"..needle.."()")
    if i==nil then return nil else return i-1 end
end

function pathParse(str)
    local lastSep = lastIndexOf(str, '/')
    return {
        dirname = string.sub(str, 1, lastSep - 1);
    }
end

-- Fs

local fs = {}

function fs.readFile(filename)
    local fh = assert(io.open(filename, "rb"))
    local contents = assert(fh:read("a"))
    fh:close()
    return contents
end


--------
-- global require
--------
require = {}
require__prototype_handlers = {}
require.__cache = {}

--------
-- local Module
--------

local Module = {}
local Module__prototype_handlers = {}

--------
function Module__prototype_handlers.__call( this, request, parent )
    -- always existing
    local isMain = true;

    -- construct module if not already existing and cached
    local module = this.__load(request, parent, isMain);

    -- What things apear to be global in the module scope.
    local moduleScope = {
        module.exports, -- exports
        module.require, -- modified _require
        require.__cache[request],
        require.__cache[request].filename, -- __filename
        pathParse(require.__cache[request].filename).dirname -- __diename
    }

    return moduleScope;
end
setmetatable(Module, Module__prototype_handlers)
--------

--[[
    Arguments are exposed to file
    @param {exports} - A table containing declared exported values, alias for module.exports.
    @param {require} - a copy of require relative to this file.
    @param {module} - this Module.
    @param {__filename} - The absolute path to this file.
    @param {__dirname} - The absolute path to the directory of this file.
--]]
function Module._(exports, require, module, __filename, __dirname)
    -- what to expose to a lua module
    local env = {
        __dirname = __dirname,
        __filename = __filename,
        module = module,
        exports = exports,
        require = require,
        print = print
    }
    local content = fs.readFile(__filename);
    content = content .. [[
return module.exports;
    ]]
    -- TODO: Throw assertions
    local module_exports, err = load(content, nil, nil, env);
    if module_exports then
        local okay, module_exports = pcall(module_exports)
        if okay then
            return module_exports;
        else
            print("Execution error:", module_exports)
        end
    else
        print("Compilation error:", module_exports)
    end
end


function require__prototype_handlers.__call( this, request )
    -- TODO: return and call and parent all need finalizing
    local _exports = Module._(
        table.unpack(
            Module(
                request, -- thisModule
                nil or nil -- parModule from cache?
            )
        )
    )
    -- Wrap all values as tables if not not exporting a table
    if ( type(_exports) ~= "table") then
        return { _exports };
    else
        return _exports;
    end
end
setmetatable(require, require__prototype_handlers)


--------
function Module.__load(request, parent, isMain)
    local exports = {};
    local SEP = "/";

    -- TODO: request lookup to abs path

    local function _require( request )
        -- TODO: scoped relative to this file - should use cache copy if exists
        require( request )
    end

    -- read file

    if ( require.__cache[request] == nil ) then
        local module = {
            filename = request,
            exports = exports,
            parent = parent,
            require = _require,
            paths = split(package.path, ';')
        }
        -- save a cached copy of this module
        require.__cache[request] = module;
        return module;
    else
        return require.__cache[request];
    end
end
--------

return Module;