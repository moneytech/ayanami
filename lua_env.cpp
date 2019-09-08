#include "lua_env.h"

#include <assert.h>
#include <string.h>

namespace {

  struct chunk_reader_state {
    int n = 0;
    const char* code;
    size_t code_size;
  };

  const char* chunk_reader(lua_State*, void* data, size_t* size) {
    static const char* chunk_prefix = "local module = {} ";
    static const size_t chunk_prefix_size = strlen(chunk_prefix);
    static const char* chunk_suffix = "\nreturn module\n";
    static const size_t chunk_suffix_size = strlen(chunk_suffix);

    chunk_reader_state* state = static_cast<chunk_reader_state*>(data);
    switch (state->n++) {
    case 0:
      *size = chunk_prefix_size;
      return chunk_prefix;
    case 1:
      *size = state->code_size;
      return state->code;
    case 2:
      *size = chunk_suffix_size;
      return chunk_suffix;
    case 3:
      *size = 0;
      return nullptr;
    default:
      assert(0);
    }
    return nullptr;
  }

}
lua_env::lua_env(lua_env::error_callback cb) : l_      { luaL_newstate() },
                                               err_cb_ { cb } {
  luaL_openlibs(l_);
}

void lua_env::register_func(const char *func_name,
                            const char *module_name,
                            int (*native_fn_ptr)(lua_State*)) {
  lua_getglobal(l_, module_name);
  if (lua_isnil(l_, -1)) {
    lua_newtable(l_);
    lua_setglobal(l_, module_name);
    lua_getglobal(l_, module_name);
  }
  lua_pushstring(l_, func_name);
  lua_pushcfunction(l_, native_fn_ptr);
  lua_settable(l_, -3);
}

void lua_env::invoke_err_cb(const char *msg) {
  if(err_cb_) err_cb_(msg);
}

void lua_env::load_module(const char *name,
                          const char *code,
                          size_t      code_len) {
  chunk_reader_state reader_state{ 0, code, code_len };
  int load_result =
    lua_load(l_, chunk_reader, &reader_state, name, "t");
  if (load_result == LUA_OK) {
    if (lua_pcall(l_, 0, 1, 0) == LUA_OK) {
      lua_setglobal(l_, name);
    }
    else {
      const char* err_msg = lua_tostring(l_, -1);
      invoke_err_cb(err_msg);
    }
  }
  else {
    const char* err_msg = lua_tostring(l_, -1);
    invoke_err_cb(err_msg);
  }
}

void lua_env::do_string(const char *str) {
  if (luaL_dostring(l_, str)) {
    const char* err_msg = lua_tostring(l_, -1);
    invoke_err_cb(err_msg);
  }
}

bool lua_env::global_exists(const char *name) {
  lua_getglobal(l_, name);
  return !(lua_isnil(l_, -1));
}
