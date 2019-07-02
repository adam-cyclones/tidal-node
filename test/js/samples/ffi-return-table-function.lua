function module.exports.returnsTableFunction( ... )
  local private {}
  function private.member()
    return 1234
  end
  return private;
end