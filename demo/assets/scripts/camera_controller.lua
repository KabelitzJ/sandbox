transform = sbx.transform.new();

move_speed = 5.0;
sprint_multiplier = 2.5;
rotation_speed = 120.0;

local y_rotation = 0.0;
local initial_position = sbx.vector3.new(0.0, 0.0, 0.0);

function on_create()
  initial_position = transform:position();
end

function on_update()
  local delta_time = sbx.delta_time();

  -- Rotation

  if (sbx.input.is_key_down(sbx.key.q)) then
    y_rotation = y_rotation + rotation_speed * delta_time;

    if (y_rotation > 360.0) then
      y_rotation = y_rotation - 360.0;
    end
  end

  if (sbx.input.is_key_down(sbx.key.e)) then
    y_rotation = y_rotation - rotation_speed * delta_time;

    if (y_rotation < 0.0) then
      y_rotation = y_rotation + 360.0;
    end
  end

  transform:set_euler_angles(sbx.vector3.new(0.0, y_rotation, 0.0));

  -- Movement

  local move_direction = sbx.vector3.new(0.0, 0.0, 0.0);

  if (sbx.input.is_key_down(sbx.key.w)) then
    move_direction = move_direction + transform:forward();
  end

  if (sbx.input.is_key_down(sbx.key.s)) then
    move_direction = move_direction - transform:forward();
  end

  if (sbx.input.is_key_down(sbx.key.a)) then
    move_direction = move_direction - transform:right();
  end

  if (sbx.input.is_key_down(sbx.key.d)) then
    move_direction = move_direction + transform:right();
  end

  local speed = move_speed;

  if (sbx.input.is_key_down(sbx.key.left_shift)) then
    speed = speed * sprint_multiplier;
  end

  transform:move_by(move_direction:normalize() * speed * delta_time);

  -- Reset

  if (sbx.input.is_key_down(sbx.key.r)) then
    y_rotation = 0.0;
    transform:set_euler_angles(sbx.vector3.new(0.0, y_rotation, 0.0));
    transform:set_position(initial_position);
  end
end
