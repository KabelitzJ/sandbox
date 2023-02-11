count = 0

function startup()
  local vector1 = sbx.vector3.new(1, 2, 3)
  local vector2 = sbx.vector3.new(4, 5, 6)

  sbx.logger.info(string.format("Vector1: %f, %f, %f", vector1.x, vector1.y, vector1.z))
  sbx.logger.info(string.format("Vector2: %f, %f, %f", vector2.x, vector2.y, vector2.z))
  
  local vector3 = vector1 + vector2
  
  sbx.logger.info(string.format("Vector3: %f, %f, %f", vector3.x, vector3.y, vector3.z))
  sbx.logger.info(string.format("Vector3 length: %f", vector3:length()))
end

function update(delta_time)

end
