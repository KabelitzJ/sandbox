transform = sbx.transform.new();

function on_create()
  transform:set_position(sbx.vector3.new(7.0, 7.0, 7.0));
  transform:look_at(sbx.vector3.zero);

  sbx.logger.info("camera::on_create position: " .. transform:position().x .. ", " .. transform:position().y .. ", " .. transform:position().z)
  sbx.logger.info("camera::on_create rotation: " .. transform:euler_angles().x .. ", " .. transform:euler_angles().y .. ", " .. transform:euler_angles().z)
end

function on_update()

end
