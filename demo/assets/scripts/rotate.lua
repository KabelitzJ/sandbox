transform = sbx.transform.new();

speed = 0.0;

function on_create()
  sbx.logger.info("speed:" .. speed);
end

function on_update()
  local delta_time = sbx.delta_time();

  local euler_angles = sbx.vector3.new(0.0, 0.0, speed * delta_time);

  transform:add_euler_angles(euler_angles);

  if (sbx.input.is_key_pressed(sbx.key.space)) then
    sbx.logger.info("space key is down");
  end
end
