/*
** Lua binding: IOT_dev
** Generated automatically by tolua++-1.0.92 on 03/18/19 16:40:53.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_IOT_dev_open (lua_State* tolua_S);

#include "iot_altivar.h"
#include "PAC_dev.h"
#include "tech_def.h"
#ifdef WIN_OS
#pragma warning(disable: 4800) //Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#endif // WIN_OS

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"altivar_manager");
}

/* function: G_ALTIVAR_MANAGER */
#ifndef TOLUA_DISABLE_tolua_IOT_dev_G_ALTIVAR_MANAGER00
static int tolua_IOT_dev_G_ALTIVAR_MANAGER00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   altivar_manager* tolua_ret = (altivar_manager*)  G_ALTIVAR_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"altivar_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_ALTIVAR_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_IOT_dev_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"G_ALTIVAR_MANAGER",tolua_IOT_dev_G_ALTIVAR_MANAGER00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_IOT_dev (lua_State* tolua_S) {
 return tolua_IOT_dev_open(tolua_S);
};
#endif

