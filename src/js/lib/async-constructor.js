/**
 * @file utility to transform a js class instantiation into an async action, the purpose is in our case to abstract away the loading of a wasm module.
 */

const toAsyncConstructor = (Target) => new Proxy(class AwaitCtor{
    /**
     * Injects a static method onto the target class allowing .new() to construct async
    */
    static async new(...args) {
        if (!args.length) {
            args = [null]
        }
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

module.exports = toAsyncConstructor;