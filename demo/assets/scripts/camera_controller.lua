transform = sbx.transform.new();

local target = sbx.vector3.new(0.0, 0.0, 0.0); -- Target of the camera

local min_distance = 2.0; -- Minimum distance of the camera from the target
local max_distance = 10.0; -- Maximum distance of the camera from the target
local zoom_speed = 2.0; -- Zoom speed of the camera
local move_speed = 5.0; -- Movement speed of the camera
local sprint_multiplier = 2.5; -- Sprint multiplier of the camera
local rotation_speed = 120.0; -- Rotation speed of the camera
local vertical_angle = 45.0; -- Vertical angle of the camera
local current_rotation = 45.0; -- Current rotation of the camera
local current_distance = 3.0; -- Current distance of the camera

function on_create()

end

function on_update()
  local delta_time = sbx.delta_time();

  -- Movement

  local move_direction = sbx.vector3.new(0.0, 0.0, 0.0);

  if (sbx.input.is_key_down(sbx.key.w)) then
    move_direction = move_direction + sbx.vector3.forward;
  end

  if (sbx.input.is_key_down(sbx.key.s)) then
    move_direction = move_direction - sbx.vector3.forward;
  end

  if (sbx.input.is_key_down(sbx.key.a)) then
    move_direction = move_direction - sbx.vector3.right;
  end

  if (sbx.input.is_key_down(sbx.key.d)) then
    move_direction = move_direction + sbx.vector3.right;
  end

  local speed = move_speed;

  if (sbx.input.is_key_down(sbx.key.left_shift)) then
    speed = speed * sprint_multiplier;
  end

  target = target + move_direction:normalize() * speed * delta_time;

  -- Rotation

  if (sbx.input.is_key_down(sbx.key.q)) then
    current_rotation = current_rotation + rotation_speed * delta_time;

    if (current_rotation > 360.0) then
      current_rotation = current_rotation - 360.0;
    end
  end

  if (sbx.input.is_key_down(sbx.key.e)) then
    current_rotation = current_rotation - rotation_speed * delta_time;

    if (current_rotation < 0.0) then
      current_rotation = current_rotation + 360.0;
    end
  end

  -- Update

  local horizontal_angle = current_rotation * math.pi / 180.0;

  local sin_angle = sbx.sin(horizontal_angle);
  local cos_angle = sbx.cos(horizontal_angle);

  local position = sbx.vector3.new();


  --   /| GAGA
  --  / | HHAG
  -- /__| sct

  local radius = current_distance * sbx.cos(vertical_angle * math.pi / 180.0);

  position.x = target.x + cos_angle * radius;
  position.y = current_distance * sbx.sin(vertical_angle * math.pi / 180.0); 
  position.z = target.z + sin_angle * radius;
  
  transform:set_position(position);

  -- transform:look_at(target);

  -- transform:set_position(sbx.vector3.new(3.0, 3.0, 3.0));
  -- transform:look_at(sbx.vector3.new(0.0, 0.0, 0.0));
end
