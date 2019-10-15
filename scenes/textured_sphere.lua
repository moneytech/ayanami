module.main = function(scn)
	local fnoise = scn.fractal_noise(7)
	local frac_mat = scn.mat_perlin(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, fnoise)
	local marb_mat = scn.mat_marble(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 3.0, fnoise)
	scn.sphere(0, 0, -20, 5, marb_mat)
	scn.sphere(0,-500, -20, 494.5, frac_mat)
	scn.sphere(0, 0, -20, 5.5, scn.mat_dielectric(1.5))
end