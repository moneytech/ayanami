#include "scene.h"
#include "sphere.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include "simple_light.h"
#include <assert.h>
#include <vector>
#include <string>
#include <chrono>

namespace {
  static constexpr char  *kSceneFunctionsModuleName = "__aya";
  static constexpr char  *kSceneScriptModuleName    = "scene";
  static constexpr char  *kSceneContextName         = "__scn";
  static constexpr float  kCamFovYDeg               = 50.0f;
  static constexpr float  kCamAperture              = 0.0f;
}

namespace {

static int aya_log_LUA(lua_State *l) {
  const char *msg = nullptr;
  LUA_CHECK_NUMARGS(1);
  LUA_CHECKED_GET(1, msg, string);
  fprintf(stderr, "LUA: %s", msg);
  return 0;
}

static int aya_sphere_LUA(lua_State *l) {
  scene        *scn = nullptr;
  float          cx,
                 cy,
                 cz,
                  r;
  const material *mat = nullptr;
  LUA_CHECK_NUMARGS(6);
  LUA_CHECKED_GET(1, scn, userdata);
  LUA_CHECKED_GET(2,  cx, number);
  LUA_CHECKED_GET(3,  cy, number);
  LUA_CHECKED_GET(4,  cz, number);
  LUA_CHECKED_GET(5,   r, number);
  LUA_CHECKED_GET(6, mat, userdata);
  scn->add_hitable(std::make_unique<sphere>(nm::float3 {cx, cy, cz}, r, mat));
  return 0;
}

static int aya_camera_LUA(lua_State *l) {
  scene *scn;
  float  fovy,
         lookfromx,
         lookfromy,
         lookfromz,
         lookatx,
         lookaty,
         lookatz,
         upvectorx,
         upvectory,
         upvectorz,
         aperture;
  LUA_CHECK_NUMARGS(12);
  LUA_CHECKED_GET(1, scn, userdata);
  LUA_CHECKED_GET(2, fovy, number);
  LUA_CHECKED_GET(3, lookfromx, number);
  LUA_CHECKED_GET(4, lookfromy, number);
  LUA_CHECKED_GET(5, lookfromz, number);
  LUA_CHECKED_GET(6, lookatx, number);
  LUA_CHECKED_GET(7, lookaty, number);
  LUA_CHECKED_GET(8, lookatz, number);
  LUA_CHECKED_GET(9, upvectorx, number);
  LUA_CHECKED_GET(10, upvectory, number);
  LUA_CHECKED_GET(11, upvectorz, number);
  LUA_CHECKED_GET(12, aperture, number);
  scn->set_camera(fovy,
                  nm::float3{lookfromx, lookfromy, lookfromz},
                  nm::float3{lookatx, lookaty, lookatz},
                  nm::float3{upvectorx, upvectory, upvectorz},
                  aperture);
  return 0;
}

static int aya_mat_lambert_LUA(lua_State *l) {
  scene        *scn = nullptr;
  float          ar,
                 ag,
                 ab;

  LUA_CHECK_NUMARGS(4);
  LUA_CHECKED_GET(1, scn, userdata);
  LUA_CHECKED_GET(2,  ar, number);
  LUA_CHECKED_GET(3,  ag, number);
  LUA_CHECKED_GET(4,  ab, number);
  auto mat = std::make_unique<lambertian>(nm::float3 { ar, ag, ab });
  lua_pushlightuserdata(l, mat.get());
  scn->add_material(std::move(mat));
  return 1;
}

static int aya_mat_simple_light_LUA(lua_State *l) {
  scene        *scn = nullptr;
  float          ar,
                 ag,
                 ab;

  LUA_CHECK_NUMARGS(4);
  LUA_CHECKED_GET(1, scn, userdata);
  LUA_CHECKED_GET(2,  ar, number);
  LUA_CHECKED_GET(3,  ag, number);
  LUA_CHECKED_GET(4,  ab, number);
  auto mat = std::make_unique<simple_light>(nm::float3 { ar, ag, ab });
  lua_pushlightuserdata(l, mat.get());
  scn->add_material(std::move(mat));
  return 1;
}

static int aya_mat_metal_LUA(lua_State *l) {
  scene        *scn = nullptr;
  float          ar,
                 ag,
                 ab,
                 f;
  LUA_CHECK_NUMARGS(5);
  LUA_CHECKED_GET(1, scn, userdata);
  LUA_CHECKED_GET(2,  ar, number);
  LUA_CHECKED_GET(3,  ag, number);
  LUA_CHECKED_GET(4,  ab, number);
  LUA_CHECKED_GET(5,   f, number);
  auto mat = std::make_unique<metal>(nm::float3 { ar, ag, ab }, f);
  lua_pushlightuserdata(l, mat.get());
  scn->add_material(std::move(mat));
  return 1;
}

static int aya_mat_dielectric_LUA(lua_State *l) {
  scene        *scn = nullptr;
  float          ri;
  LUA_CHECK_NUMARGS(2);
  LUA_CHECKED_GET(1, scn, userdata);
  LUA_CHECKED_GET(2,  ri, number);
  auto mat = std::make_unique<dielectric>(ri);
  lua_pushlightuserdata(l, mat.get());
  scn->add_material(std::move(mat));
  return 1;
}

}

class scene_module_builder {
public:
  explicit scene_module_builder(lua_env &lua) : lua_(lua) {}

  ~scene_module_builder() {
    static const std::string kFactoryPrefix = R"(
function _scene_wrapper_factory(scene)
  fwd_fact = function(f) return function(...) return f(scene, ...) end end
  return {)";
    static const std::string kFactorySuffix = "}\nend";
    std::string buf;
    buf.reserve(kFactoryPrefix.length() +
                kFactorySuffix.length() +
                buf_length_);
    buf = kFactoryPrefix;
    for (const auto &s : func_names_) buf += s + " = fwd_fact(__aya." + s + "),";
    buf += kFactorySuffix;
    lua_.do_string(buf.c_str());
  }
  
  scene_module_builder& register_func(const char *name,
                                      int (*native_fn_ptr)(lua_State*)) {
    lua_.register_func(name, kSceneFunctionsModuleName, native_fn_ptr);
    func_names_.emplace_back(name);
    buf_length_ += 2 * func_names_.back().length() + 20u;
    return *this;
  }
private:
  
  lua_env                 &lua_;
  std::vector<std::string> func_names_;
  uint32_t                 buf_length_ = 0;
};

scene::scene(lua_env    &lua,
             float       aspect,
             const char *script,
             size_t      script_len,
             bvh_node::cons_strat bvh_cons_strat) : cam_ {
               kCamFovYDeg,
               aspect,
               nm::float3 {  0.0f,  0.0f,  2.0f },
               nm::float3 {  0.0f,  0.0f, -1.0f },
               nm::float3 {  0.0f,  1.0f,  0.0 },
               kCamAperture
             },
             aspect_(aspect) {
  if (!lua.global_exists(kSceneFunctionsModuleName)) {
    // register native funcs
    scene_module_builder sm { lua };
    sm.register_func("log", aya_log_LUA)
      .register_func("mat_lambert", aya_mat_lambert_LUA)
      .register_func("mat_metal", aya_mat_metal_LUA)
      .register_func("mat_dielectric", aya_mat_dielectric_LUA)
      .register_func("mat_simple_light", aya_mat_simple_light_LUA)
      .register_func("sphere", aya_sphere_LUA)
      .register_func("camera", aya_camera_LUA);
  }
  lua.load_module(kSceneScriptModuleName, script, script_len);
  lua_pushlightuserdata(lua.raw(), (void*)this);
  lua_setglobal(lua.raw(), "tmp");
  const std::chrono::time_point  lua_start = std::chrono::system_clock::now();
  lua.do_string("scene.main(_scene_wrapper_factory(tmp))");
  const std::chrono::time_point lua_end = std::chrono::system_clock::now();
  const std::chrono::duration<double> lua_dur = lua_end - lua_start;
  stats_.lua_runtime = lua_dur.count();

  rebuild_bvh(bvh_cons_strat);
}

 scene::~scene() {
  for (hitable *h : hitables_) delete h;
 }

void scene::rebuild_bvh(bvh_node::cons_strat bvh_cons_strat) {
  const std::chrono::time_point  bvh_ctime_start = std::chrono::system_clock::now();
  root_node_ = std::make_unique<bvh_node>(hitables_.data(), hitables_.size(), bvh_cons_strat);
  const std::chrono::time_point bvh_ctime_end = std::chrono::system_clock::now();
  const std::chrono::duration<double> bvh_ctime_dur = bvh_ctime_end - bvh_ctime_start;
  stats_.bvh_construction = bvh_ctime_dur.count();
}

const camera& scene::cam() const {
  return cam_;
}

void scene::set_camera(float             fov_v,
                       const nm::float3 &look_from,
                       const nm::float3 &look_at,
                       const nm::float3 &upvector,
                       float             aperture) {
  cam_ = camera(fov_v, aspect_, look_from, look_at, upvector, aperture);
}

void scene::add_material(std::unique_ptr<material> &&mat) {
  mats_.emplace_back(std::move(mat));
}

void scene::add_hitable(std::unique_ptr<hitable> &&h) {
  //root_.append(std::move(h));
  hitables_.push_back(h.release());
}

nm::float3 scene::color(const ray &r, int bounce) const {
  static constexpr int kMaxBounces = 100;
  hit_record hit;
  if (root_node_->hit_test(r, 0.001f, 1000.0f, hit)) {
    nm::float3 attn;
    ray        scattered;
    nm::float3 emitted = hit.mat->emitted(hit.p);
    if (bounce < kMaxBounces && hit.mat->scatter(r, hit, attn, scattered)) {
      return emitted + attn * color(scattered, bounce + 1);
    } else {
      return emitted;
    }
  }
  return nm::float3 { 0.0f };
}
