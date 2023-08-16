position = sbx.vector3.new(0, 0, 0);
rotation = sbx.vector3.new(0, 0, 0);

function on_create()
  sbx.logger.info("hello from main")
  sbx.logger.info("position: " .. position.x .. ", " .. position.y .. ", " .. position.z)
  sbx.logger.info("rotation: " .. rotation.x .. ", " .. rotation.y .. ", " .. rotation.z)
end

function on_update()
  -- local delta_time = sbx.delta_time()
  
  -- -- position = sbx.vector3.new(0, 10, 0);
  -- rotation.z = rotation.z + (360 / 2) * delta_time;

  -- -- sbx.logger.info("delta time: " .. delta_time)
  -- -- sbx.logger.info("position: " .. position.x .. ", " .. position.y .. ", " .. position.z)
  -- -- sbx.logger.info("rotation: " .. rotation.x .. ", " .. rotation.y .. ", " .. rotation.z)
end
