transform = sbx.transform.new();

amount = 0.0;

function on_create()
  
end

function on_update()
  local delta_time = sbx.delta_time();

  local euler_angles = sbx.vector3.new(0.0, 0.0, amount * delta_time);

  transform:add_euler_angles(euler_angles);
end
