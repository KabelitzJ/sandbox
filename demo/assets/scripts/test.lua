local sbx = require("sbx")

local test_script = {
  my_value = 0,
  tag = nil
}

function test_script:on_create()
  self.tag = self:get_component("tag")

  sbx.info("test_script:on_create of {}", self.tag)
end

function test_script:on_update(delta_time)
  -- sbx.info("test_script:on_update of {}: {}", self.tag, delta_time)
end

return test_script
