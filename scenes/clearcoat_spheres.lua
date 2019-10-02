module.main = function(aya)
  aya.camera(50.0,
             0.0, 0.0, 2.0,
             0.0, 0.0, -1.0,
             0.0, 1.0, 0.0,
             0.0)
  diffuse_gray = aya.mat_lambert(0.5, 0.5, 0.5)
  diffuse_pink = aya.mat_lambert(0.8, 0.3, 0.3)
  diffuse_yellow = aya.mat_lambert(0.3, 0.2, 0.1)
  diffuse_blue = aya.mat_lambert(0.1, 0.2, 0.5)
  dielectric_1 = aya.mat_dielectric(1.5)
  reddish_metal = aya.mat_metal(0.8, 0.6, 0.2, 1.0)
  gray_metal = aya.mat_metal(0.8, 0.8, 0.8, 0.3)

  aya.sphere (0.0,    0.0, -1.0 ,   0.5, dielectric_1)
  aya.sphere (0.0,    0.0, -1.0 ,  0.48, diffuse_blue)
  aya.sphere (0.0, -100.5, -1.0 , 100.0, dielectric_1)
  aya.sphere (0.0, -100.5, -1.0 ,  99.9, diffuse_yellow)
  aya.sphere (-1.0,    0.0, -1.0,    0.5, dielectric_1 )
  aya.sphere (-1.0,    0.0, -1.0,   0.48, diffuse_pink)
end