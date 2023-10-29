transform = sbx.transform.new();

center = sbx.vector3.new();
speed = 0.0;
angle_degrees = 0.0;

function on_create()

end

function on_update()
  local delta_time = sbx.delta_time();

  local angle_radians = angle_degrees * math.pi / 180.0;

  local sin_angle = sbx.sin(angle_radians);
  local cos_angle = sbx.cos(angle_radians);

  local new_position = sbx.vector3.new();

  new_position.x = center.x + cos_angle * 3.0;
  new_position.y = center.y;
  new_position.z = center.z + sin_angle * 3.0;

  transform:set_position(new_position);

  angle_degrees = angle_degrees + speed * delta_time;
end
