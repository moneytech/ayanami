module.main = function(scn)
	local checker_tex = scn.tex_checker(1.0, 0.5, 0.6,
	                             	     0.6, 0.5, 1.0,
	                                	  50)
	local checker_mat = scn.mat_lambert_textured(checker_tex)
	local o1 = scn.perlin_noise(0.5, 0.25)
	local o2 = scn.perlin_noise(2.0, 0.5)
	local o3 = scn.perlin_noise(20.0, 1.0)
	local fnoise = scn.fractal_noise(o1, o2, o3)
	local pm = scn.mat_perlin(0.1, 0.1, 0.2, 0.5, 0.45, 0.45, fnoise)
	scn.sphere(0, 0, -20, 5, checker_mat)
	scn.sphere(0,-500, -20, 495, pm)
end