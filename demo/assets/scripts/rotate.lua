transform = sbx.transform.new();

speed = 0.0;

function on_create()
  speed = 120.0;

  sbx.logger.info("speed:" .. speed);
end

function on_update()
  local delta_time = sbx.delta_time();

  if (sbx.input.is_key_pressed(sbx.key.a)) then
    transform:add_euler_angles(sbx.vector3.new(0.0, 0.0, -speed * delta_time));
  elseif (sbx.input.is_key_pressed(sbx.key.d)) then
    transform:add_euler_angles(sbx.vector3.new(0.0, 0.0, speed * delta_time));
  elseif (sbx.input.is_key_pressed(sbx.key.w)) then
    transform:add_euler_angles(sbx.vector3.new(-speed * delta_time, 0.0, 0.0));
  elseif (sbx.input.is_key_pressed(sbx.key.s)) then
    transform:add_euler_angles(sbx.vector3.new(speed * delta_time, 0.0, 0.0));
  elseif (sbx.input.is_key_pressed(sbx.key.q)) then
    transform:add_euler_angles(sbx.vector3.new(0.0, speed * delta_time, 0.0));
  elseif (sbx.input.is_key_pressed(sbx.key.e)) then
    transform:add_euler_angles(sbx.vector3.new(0.0, -speed * delta_time, 0.0));
  elseif (sbx.input.is_key_pressed(sbx.key.space)) then
    transform:set_euler_angles(sbx.vector3.new(0.0, 0.0, 0.0));
  end
end
