module.main = function(scn)
	scn.camera(40.0,
			   278, 278, -800,
				 278, 278, 0.0,
			   0.0, 1.0, 0.0,
			   0.0)
	scn.sky_gradient(0, 0, 0,
								  0, 0, 0)
	local red   = scn.mat_lambert(0.65, 0.05, 0.05)
	local green = scn.mat_lambert(0.12, 0.45, 0.15)
	local white = scn.mat_lambert(0.73, 0.73, 0.73)
	local mirror = scn.mat_metal(1.0, 1.0, 1.0, 0.1)
	local light = scn.mat_simple_light(15, 15, 15)
	scn.yzrect(0, 555, 0, 555, 555, false, green)
	scn.yzrect(0, 555, 0, 555, 0, true,   red)
	scn.xzrect(213, 343, 227, 332, 554, false, light)
	scn.xzrect(0, 555, 0, 555, 0, true, white)
  scn.xzrect(0, 555, 0, 555, 555, false, white)
  scn.xyrect(0, 555, 0, 555, 555, false, white)
	scn.sphere(200, 100, 332, 100, mirror)
end