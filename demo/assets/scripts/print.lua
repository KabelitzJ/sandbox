-- local transform = sbx.transform.new();

value = 12;

function on_create()
  sbx.logger.info("Hello from Lua!");

  value = 13;
  
  -- local position = transform:position();

  -- print("Position: [x: " .. position.x .. ", y: " .. position.y .. ", z: " .. position.z .. "]");
end
