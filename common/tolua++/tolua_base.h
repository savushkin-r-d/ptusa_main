#ifndef TOLUA_BASE_H
#define TOLUA_BASE_H

#include <tolua++.h>

class ToluaBase {

	int lua_instance;

protected:

	lua_State* lua_state;

	bool push_method(const char* name, lua_CFunction f) const {

		if (!lua_state) return false;

		lua_getref(lua_state, lua_instance);
		lua_pushstring(lua_state, name);
		lua_gettable(lua_state, -2);

		if (lua_isnil(lua_state, -1)) {

			// pop the table
			lua_pop(lua_state, 2);
			return false;

		} else {

			if (f) {
				if (lua_iscfunction(lua_state, -1)) {
					lua_pop(lua_state, 2);
					return false;
				};
				/* // not for now
				lua_pushcfunction(lua_state, f);
				if (lua_rawequal(lua_state, -1, -2)) {

					// avoid recursion, pop both functions and the table
					lua_pop(lua_state, 3);
					return false;
				};

				// pop f
				lua_pop(lua_state, 1);
				*/
			};

			// swap table with function
			lua_insert(lua_state, -2);
		};

		return true;
	};

	void dbcall(lua_State* L, int nargs, int nresults) const {

		// using lua_call for now
		lua_call(L, nargs, nresults);
	};
public:

	void tolua__set_instance(lua_State* L, lua_Object lo) {

		lua_state = L;

		lua_pushvalue(L, lo);
		lua_instance = lua_ref(lua_state, 1);
	};

	ToluaBase() {

		lua_state = NULL;
	};

	~ToluaBase() {

		if (lua_state) {

			lua_unref(lua_state, lua_instance);
		};
	};
};

#endif


