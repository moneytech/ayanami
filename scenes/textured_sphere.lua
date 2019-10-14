module.main = function(scn)
	local checker = scn.tex_checker(1.0, 0.5, 0.6,
	                                0.6, 0.5, 1.0,
	                                50)
	local checker_mat = scn.mat_lambert_textured(checker)
	scn.sphere(0, 0, -20, 5, checker_mat)
	scn.sphere(0,-20, -20, 15, checker_mat)
	scn.sphere(0, 20, -20, 15, checker_mat)
end