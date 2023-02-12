position = sbx.vector3.new()
velocity = sbx.vector3.new()
acceleration = sbx.vector3.new()

function startup()
  position = position * 2

  sbx.logger.info("Position (" .. position.x .. ", " .. position.y .. ", " .. position.z .. ")")
  sbx.logger.info("Velocity (" .. velocity.x .. ", " .. velocity.y .. ", " .. velocity.z .. ")")
  sbx.logger.info("Acceleration (" .. acceleration.x .. ", " .. acceleration.y .. ", " .. acceleration.z .. ")")
end

function update(delta_time)

end
