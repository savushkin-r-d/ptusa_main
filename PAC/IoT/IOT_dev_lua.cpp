/*
** Lua binding: IOT_dev
** Generated automatically by tolua++-1.0.92 on Thu Feb 16 14:48:05 2023.
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

/* method: add_node of class  altivar_manager */
#ifndef TOLUA_DISABLE_tolua_IOT_dev_altivar_manager_add_node00
static int tolua_IOT_dev_altivar_manager_add_node00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"altivar_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  altivar_manager* self = (altivar_manager*)  tolua_tousertype(tolua_S,1,0);
  const char* IP_address = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int port = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int timeout = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* article = ((const char*)  tolua_tostring(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_node'", NULL);
#endif
  {
   self->add_node(IP_address,port,timeout,article);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_node'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

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
  tolua_cclass(tolua_S,"altivar_manager","altivar_manager","",NULL);
  tolua_beginmodule(tolua_S,"altivar_manager");
   tolua_function(tolua_S,"add_node",tolua_IOT_dev_altivar_manager_add_node00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_ALTIVAR_MANAGER",tolua_IOT_dev_G_ALTIVAR_MANAGER00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_IOT_dev (lua_State* tolua_S) {
 return tolua_IOT_dev_open(tolua_S);
};
#endif

