transform = sbx.transform.new();

move_speed = 5.0;

function on_create()
  
end

function on_update()
  local delta_time = sbx.delta_time();

  local movement = sbx.vector3.new();
  local rotation = 0.0;

  if (sbx.input.is_key_pressed(sbx.key.w)) then
    movement = movement + sbx.vector3.backward;
  end

  if (sbx.input.is_key_pressed(sbx.key.s)) then
    movement = movement + sbx.vector3.forward;
  end

  if (sbx.input.is_key_pressed(sbx.key.a)) then
    movement = movement + sbx.vector3.left;
  end

  if (sbx.input.is_key_pressed(sbx.key.d)) then
    movement = movement + sbx.vector3.right;
  end

  transform:move_by(movement:normalize() * move_speed * delta_time);

  if (sbx.input.is_key_pressed(sbx.key.q)) then
    rotation = 120.0;
  elseif (sbx.input.is_key_pressed(sbx.key.e)) then
    rotation = -120.0;
  end
  
  transform:add_euler_angles(sbx.vector3.up * rotation * delta_time);

  if (sbx.input.is_key_pressed(sbx.key.r)) then
    transform:set_position(sbx.vector3.new(0.0, 0.0, 5.0));
    transform:set_euler_angles(sbx.vector3.zero);
  end
end
