transform = sbx.transform.new();

function on_create()
  sbx.logger.info("hello from main")

  transform:set_position(sbx.vector3.new(7.0, 7.0, 7.0));
  transform:look_at(sbx.vector3.zero);

  sbx.logger.info("position: " .. transform:position().x .. ", " .. transform:position().y .. ", " .. transform:position().z)
  sbx.logger.info("rotation: " .. transform:euler_angles().x .. ", " .. transform:euler_angles().y .. ", " .. transform:euler_angles().z)
end

function on_update()
  -- local delta_time = sbx.delta_time()
  
  -- -- position = sbx.vector3.new(0, 10, 0);
  -- rotation.z = rotation.z + (360 / 2) * delta_time;

  -- -- sbx.logger.info("delta time: " .. delta_time)
  -- -- sbx.logger.info("position: " .. position.x .. ", " .. position.y .. ", " .. position.z)
  -- -- sbx.logger.info("rotation: " .. rotation.x .. ", " .. rotation.y .. ", " .. rotation.z)
end
