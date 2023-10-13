transform = sbx.transform.new();

speed = 0.0;

function on_create()
  speed = 120.0;

  sbx.logger.info("speed:" .. speed);
end

function on_update()
  local delta_time = sbx.delta_time();

  local euler_angles = sbx.vector3.new();

  if (sbx.input.is_key_pressed(sbx.key.a)) then
    euler_angles.z = -1.0;
  elseif (sbx.input.is_key_pressed(sbx.key.d)) then
    euler_angles.z = 1.0;
  end
  
  if (sbx.input.is_key_pressed(sbx.key.w)) then
    euler_angles.x = -1.0;
  elseif (sbx.input.is_key_pressed(sbx.key.s)) then
    euler_angles.x = 1.0;
  end
  
  if (sbx.input.is_key_pressed(sbx.key.q)) then
    euler_angles.y = 1.0;
  elseif (sbx.input.is_key_pressed(sbx.key.e)) then
    euler_angles.y = -1.0;
  end

  if (sbx.input.is_key_pressed(sbx.key.space)) then
    transform:set_euler_angles(sbx.vector3.new());
  elseif (euler_angles:length() > 0.0) then
    euler_angles:normalize();
    transform:add_euler_angles(euler_angles * speed * delta_time);
  end
end
