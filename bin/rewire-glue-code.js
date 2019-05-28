const { config } = require('../package.json');
const { readFile, writeFile } = require('fs').promises;
const { resolve } = require('path');
const { wasm } = config;

const OUTPUT_FILE = resolve(__dirname, '../', wasm.output.split(/\s/).pop());
const messageSuccess = __target__filename => `Successfully re-wired Emscripten glue code ${__target__filename}`;
const messageRedundent = __target__filename => `Already re-wired Emscripten glue code ${__target__filename}, exiting.`;

const exposeRuntimeMethodsArray = wasm.exposeRuntimeMethods
    .replace(/\[|\]/gm, '')
    .split(/\s|,/)
    .filter(Boolean)
    .map(item => item.replace(/'/gm, ''));

;(async ()=> {
    // attempt to read file
    let source;
    try {
        source = await readFile(OUTPUT_FILE);
        if (/module\.exports\.asmGlobalArg/.test(source)) {
            console.log(messageRedundent(OUTPUT_FILE));
            process.exit(0);
        } else {
            source = await source.toString();
            source = source.replace('var asmGlobalArg =', 'var asmGlobalArg = module.exports.asmGlobalArg =');
            source = source.replace('var asmLibraryArg =', 'var asmLibraryArg = module.exports.asmLibraryArg =');
            source = source.replace('var info =', 'var info = module.exports.info =');

            // expose extra runtime methods based on package.json variable
            for(const methodName of exposeRuntimeMethodsArray) {
                source = source.replace(`Module["${methodName}"] = ${methodName}`, `Module["${methodName}"] = ${methodName}; module.exports.${methodName} = ${methodName}`);
            }
        }
    } catch(err) {
        console.error(err);
    }
    // attempt to write to file
    try {
        await writeFile(OUTPUT_FILE, source);
        console.log(messageSuccess(OUTPUT_FILE));
    } catch(err) {
        console.error(err);
    }
})();