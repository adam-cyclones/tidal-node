/**
 * @test_notes cannot use async / await as per the docs https://jestjs.io/docs/en/tutorial-async due to false failures, unknown cause, workaround as per implimentation.
 */
const Lua = require('../../src/js/tidal-node.js');
const { resolve } = require('path');
const NO_IMPL = "TODO: Not implimented yet:";

describe("Lua Api", () => {
    beforeAll(async ()=>{
        const TEST_PROJECT_ROOT = resolve(__dirname, "../../");
        const LuaVM = Lua.new({ projectRoot: TEST_PROJECT_ROOT });
        global.L = await LuaVM;
    });
    describe("Node like Lua module exports", () => {
        test("Lua should load a module and export a <number> value", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-number.lua");
            const result = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(result).toEqual(42);
        });
        test("Lua should load a module and export a <string> value", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-string.lua");
            const result = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(result).toEqual("Hello Node.js my name is Lua");
        });
        test("Lua should load a module and export a <boolean> value", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-boolean.lua");
            const result = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(result).toEqual(true);
        });
        test("Lua should load a module and export a <float> value", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-double.lua");
            const result = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(result).toEqual(0.1);
        });
        test("Lua should load a module and export a <null> value as nil", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-nil.lua");
            const result = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(result).toEqual(null);
        });
        test("Lua should load a module and export a <function> value", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-function.lua");
            const addFn = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(typeof addFn).toEqual("function");
        });
        test("Lua should load a module and export a <table> table as <object> value", () => {
            const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            const LUA_PATH_TO_EXPORT_OBJECT = resolve(LUA_SAMPLE_PATH, "export-table.lua");
            const result = L.doFile(LUA_PATH_TO_EXPORT_OBJECT);
            expect(result).toEqual({
                some_char: "a",
                some_string: "a string",
                some_number: 123,
                some_float: 1.23,
                some_t_bool: true,
                some_f_bool: false
            });
            expect(result.some_nil).toEqual(undefined);
        });
        describe("FFI", () => {
            test("Lua function should return <boolean>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-boolean.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                expect(fn()).toEqual(true);
            });
            test("Lua function should return <function>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-function.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                console.warn(NO_IMPL,"anonamous functions returned by functions.");
            });
            test("Lua function should return <nil> as <null>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-nil.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                expect(fn()).toEqual(null);
            });
            test("Lua function should return <number>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-number.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                expect(fn()).toEqual(22600001);
            });
            test("Lua function should return <double>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-double.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                expect(fn()).toEqual(0.72);
            });
            test("Lua function should return <string>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-string.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                expect(fn()).toEqual("I am a lua string from a lua function!");
            });
            test("Lua function should return <table> as <object>", () => {
                const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
                const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-table.lua");
                const fn = L.doFile(LUA_PATH_TO_FFI);
                expect(fn()).toEqual({
                    some_char: "a",
                    some_string: "a string",
                    some_number: 123,
                    some_float: 1.23,
                    some_t_bool: true,
                    some_f_bool: false
                });
            });
            // test("Lua function should return <table> with <function>", () => {
            //     const LUA_SAMPLE_PATH = resolve(__dirname, "samples");
            //     const LUA_PATH_TO_FFI = resolve(LUA_SAMPLE_PATH, "ffi-return-table=function.lua");
            //     const fn = L.doFile(LUA_PATH_TO_FFI);
            //     console.log(fn)
            // });
        });
    });

});