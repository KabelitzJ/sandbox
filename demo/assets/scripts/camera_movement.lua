transform = sbx.transform.new();

move_speed = 5.0;
rotation_speed = 120.0;

local y_rotation = 0.0;

function on_create()
  
end

function on_update()
  local delta_time = sbx.delta_time();

  -- Rotation

  if (sbx.input.is_key_pressed(sbx.key.q)) then
    y_rotation = y_rotation + rotation_speed * delta_time;
  end

  if (sbx.input.is_key_pressed(sbx.key.e)) then
    y_rotation = y_rotation - rotation_speed * delta_time;
  end

  transform:set_euler_angles(sbx.vector3.new(0.0, y_rotation, 0.0));

  -- Movement

  -- local move_direction = sbx.vector3.new(0.0, 0.0, 0.0);
end
