#pragma once
#include "includes.h"
#include "tolua++.h"

#include "device/device.h"
#include "device/manager.h"

extern "C" {
    LUA_API int tolua_PAC_dev_open(lua_State* L);
}

