#define LUA_LIB

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "aoi.h"
#include "skynet_malloc.h"

struct alloc_cookie {
	int count;
	int max;
	int current;
};

static void *
my_alloc(void * ud, void *ptr, size_t sz) {
	struct alloc_cookie * cookie = ud;
	if (ptr == NULL) {
		void *p = skynet_malloc(sz);
		++ cookie->count;
		cookie->current += sz;
		if (cookie->max < cookie->current) {
			cookie->max = cookie->current;
		}
//		printf("%p + %u\n",p, sz);
		return p;
	}
	-- cookie->count;
	cookie->current -= sz;
//	printf("%p - %u \n",ptr, sz);
	free(ptr);
	return NULL;
}

static int
registerCallback( lua_State *L ) {
  return luaL_ref( L, LUA_REGISTRYINDEX );
}

static void
call_callback( lua_State *L, int callback_reference ) {
  lua_rawgeti( L, LUA_REGISTRYINDEX, callback_reference );
  if ( 0 != lua_pcall( L, 0, 0, 0 ) ) {
    printf("Failed to call the callback!\n %s\n", lua_tostring( L, -1 ) );
    return;
  }
}

static int
laoi_update(lua_State *L) {
  return 0;
}

static int
laoi_message(lua_State *L) {
  return 0;
}

static int
laoi_release(lua_State *L) {
  return 0;
}
// aoi = {
//   new,
//   update
//   aoi_message,
//   release
// }
static int
laoi_new(lua_State *L) {
  struct alloc_cookie cookie = { 0,0,0 };
	struct aoi_space * space = aoi_create(my_alloc , &cookie);

  lua_createtable(L, 0, 1);

  int t = lua_gettop(L);

  lua_pushlightuserdata(L, space);
  lua_setfield(L, t, "space");

	lua_pushcfunction(L, laoi_update);
	lua_setfield(L, t, "update");

  lua_pushcfunction(L, laoi_message);
  lua_setfield(L, t, "message");

  lua_pushcfunction(L, laoi_release);
  lua_setfield(L, t, "release");

	return 1;
}

LUAMOD_API int
luaopen_aoi(lua_State *L) {
	luaL_Reg l[] = {
		{ "new", laoi_new },
		{ NULL, NULL },
	};
	luaL_newlib(L,l);
	return 1;
}
