transform = sbx.transform.new();

speed = 0.0;
angle_degrees = 0.0;

function on_create()
  speed = 1.0;
end

function on_update()
  local delta_time = sbx.delta_time();

  local angle_radians = angle_degrees * math.pi / 180.0;

  local cos_theta = math.cos(angle_radians);
  local sin_theta = math.sin(angle_radians);
  
  local rotated = sbx.vector3.new();

  rotated.x = cos_theta * transform:position().x - sin_theta * transform:position().z;
  rotated.y = transform:position().y;
  rotated.z = sin_theta * transform:position().x + cos_theta * transform:position().z;

  transform:set_position(rotated);

  angle_degrees = (angle_degrees + speed * delta_time) % 360.0;
end
