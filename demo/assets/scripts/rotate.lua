transform = sbx.transform.new();

speed = 0.0;

function on_create()
  speed = 120.0;
end

function on_update()
  local delta_time = sbx.delta_time();

  local euler_angles = sbx.vector3.up * speed * delta_time;

  -- transform:add_euler_angles(euler_angles);
end
