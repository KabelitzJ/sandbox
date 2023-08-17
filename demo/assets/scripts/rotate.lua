transform = sbx.transform.new();

speed = 0.0;

function on_create()
  
end

function on_update()
  local delta_time = sbx.delta_time();

  local euler_angles = sbx.vector3.new(0.0, 0.0, speed * delta_time);

  transform:add_euler_angles(euler_angles);
end
