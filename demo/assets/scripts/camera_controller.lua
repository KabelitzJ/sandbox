transform = sbx.transform.new();

move_speed = 5.0;
rotation_speed = 120.0;

local y_rotation = 0.0;
local initial_position = sbx.vector3.new(0.0, 0.0, 0.0);

function on_create()
  initial_position = transform:position();
end

function on_update()
  local delta_time = sbx.delta_time();

  sbx.logger.info(delta_time);

  -- Rotation

  if (sbx.input.is_key_pressed(sbx.key.q)) then
    y_rotation = y_rotation + rotation_speed * delta_time;
  end

  if (sbx.input.is_key_pressed(sbx.key.e)) then
    y_rotation = y_rotation - rotation_speed * delta_time;
  end

  transform:set_euler_angles(sbx.vector3.new(0.0, y_rotation, 0.0));

  -- Movement

  local move_direction = sbx.vector3.new(0.0, 0.0, 0.0);

  if (sbx.input.is_key_pressed(sbx.key.w)) then
    move_direction = move_direction + transform:forward();
  end

  if (sbx.input.is_key_pressed(sbx.key.s)) then
    move_direction = move_direction - transform:forward();
  end

  if (sbx.input.is_key_pressed(sbx.key.a)) then
    move_direction = move_direction - transform:right();
  end

  if (sbx.input.is_key_pressed(sbx.key.d)) then
    move_direction = move_direction + transform:right();
  end

  transform:move_by(move_direction:normalize() * move_speed * delta_time);

  -- Reset

  if (sbx.input.is_key_pressed(sbx.key.r)) then
    y_rotation = 0.0;
    transform:set_euler_angles(sbx.vector3.new(0.0, y_rotation, 0.0));
    transform:set_position(initial_position);
  end
end
