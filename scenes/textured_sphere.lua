module.main = function(scn)
	local checker_tex = scn.tex_checker(1.0, 0.5, 0.6,
	                             	     0.6, 0.5, 1.0,
	                                	  50)
	local checker_mat = scn.mat_lambert_textured(checker_tex)
	local fnoise = scn.fractal_noise(7)
	local pm = scn.mat_perlin(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, fnoise)
	scn.sphere(0, 0, -20, 5, checker_mat)
	scn.sphere(0,-500, -20, 495, pm)
end