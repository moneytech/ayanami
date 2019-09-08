#pragma once

#include <stddef.h>
#include <lua.hpp>

/**
 * Encapsulates the Lua VM.
 */
class lua_env {
public:
  using error_callback = void(*)(const char*);

  explicit lua_env(error_callback);

  bool global_exists(const char *name);
  
  void register_func(const char *func_name,
                     const char *module_name,
                     int (*native_fn_ptr)(lua_State*));

  void load_module(const char *name,
                   const char *code,
                   size_t      code_len);

  void do_string(const char *str);

  lua_State *raw() { return l_; }

private:
  void invoke_err_cb(const char*);

private:
  lua_State      *l_;
  error_callback  err_cb_;
};

#define LUA_CHECK_NUMARGS(n) (lua_gettop(l) == n || luaL_error(l, "too few arguments"))
#define LUA_CHECKED_GET(index, varname, type) { \
  (lua_is##type##(l, index) || luaL_error(l, "Type error\n")); \
  varname = static_cast<decltype(varname)>(lua_to##type##(l, index)); \
}