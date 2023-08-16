position = sbx.vector3.new()
rotation = sbx.vector3.new()

function on_create()
  sbx.logger.info("hello from main")
end

function on_update()
  local delta_time = sbx.delta_time()
  sbx.logger.info(rotation.z);
  
  -- position = sbx.vector3.new(0, 10, 0);
  rotation.z = rotation.z + 1 * delta_time;
  sbx.logger.info(rotation.z);
end
