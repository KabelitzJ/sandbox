transform = sbx.transform.new();

speed = 0.0;

function on_create()
  
end

function on_update()
  -- sbx.logger.log("on_update");

  -- local delta_time = sbx.delta_time();

  -- sbx.logger.log("delta_time: " .. delta_time);

  -- local euler_angles = sbx.vector3.up * speed * delta_time;

  -- sbx.logger.log("euler_angles: " .. euler_angles:x() .. ", " .. euler_angles:y() .. ", " .. euler_angles:z());

  -- transform:add_euler_angles(euler_angles);

  -- sbx.logger.log("euler_angles: " .. euler_angles.x .. ", " .. euler_angles.y .. ", " .. euler_angles.z);
end
