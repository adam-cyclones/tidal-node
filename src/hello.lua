-- TODO: nil exports
-- module.exports = nil;

-- TODO: nested functions 
module.exports = {
    foo = 0.69,
    bar = "hmmm",
    itsABool = true,
    42
}

function module.exports.thunder( ... )
    print("woo")
end

function module.exports.flash(a, b)
    print(a, b)
    -- TODO: nested functions 
    return 0.374;
end

module.exports.hotdog = {
    ace = "of spades!"
}

