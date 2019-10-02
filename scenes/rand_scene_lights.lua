module.main = function(scn)
	scn.camera(45.0,
			   0.0, 2.0, -10.0,
			   0.0,  2.0,   1.0,
			   0.0,  1.0, 0.0,
			   0.1)
	scn.sky_gradient(0.013, 0.01, 0.013, 0.01, 0.0, 0.01)
	scn.sphere(0.0, -1000.0, 0.0,
			   1000.0,
			   scn.mat_lambert(0.5, 0.5, 0.5))
	mat_fns = {
		function()
			return scn.mat_lambert(math.random(),
						    	  math.random(),
	  						    math.random())
		end,
		function()
			return scn.mat_metal(math.random(),
								math.random(),
								math.random(),
								math.random())
		end,
		function()
			return scn.mat_dielectric(1.5)
		end,
		function()
			return scn.mat_simple_light(math.random(),
												         math.random(),
												         math.random())
		end
	}
	for a=-11,10 do
		for b=-11,10 do
			scn.sphere(a+0.9*math.random(), 0.2,
				   b + 0.9*math.random(),
				   0.2,
				   mat_fns[math.random(1, 4)]())
			
		end
	end
	scn.sphere(0, 1, 0, 1.0, scn.mat_simple_light(4.0, 4.0, 4.0))
	scn.sphere(-4, 1, 0, 1.0, scn.mat_lambert(0.4, 0.2, 0.1))
	scn.sphere(4, 1, 0, 1.0, scn.mat_metal(0.7, 0.6, 0.5, 0.0))
end