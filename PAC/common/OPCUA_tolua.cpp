/*
** Lua binding: OPC_UA
** Generated automatically by tolua++-1.0.92 on 08/29/18 17:02:49.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_OPC_UA_open (lua_State* tolua_S);

#include "OPCUAServer.h"
#include "PAC_dev.h"
#include "tech_def.h"
#ifdef WIN_OS
#pragma warning(disable: 4800) //Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#endif // WIN_OS

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"device");
 tolua_usertype(tolua_S,"OPCUAServer");
 tolua_usertype(tolua_S,"tech_object");
}

/* method: BaseConfig of class  OPCUAServer */
#ifndef TOLUA_DISABLE_tolua_OPC_UA_OPCUAServer_BaseConfig00
static int tolua_OPC_UA_OPCUAServer_BaseConfig00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"OPCUAServer",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  OPCUAServer* self = (OPCUAServer*)  tolua_tousertype(tolua_S,1,0);
  bool readonly = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BaseConfig'", NULL);
#endif
  {
   self->BaseConfig(readonly);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BaseConfig'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addPumpObject of class  OPCUAServer */
#ifndef TOLUA_DISABLE_tolua_OPC_UA_OPCUAServer_addPumpObject00
static int tolua_OPC_UA_OPCUAServer_addPumpObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"OPCUAServer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  OPCUAServer* self = (OPCUAServer*)  tolua_tousertype(tolua_S,1,0);
  device* dev = ((device*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addPumpObject'", NULL);
#endif
  {
   self->addPumpObject(dev);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addPumpObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTechObject of class  OPCUAServer */
#ifndef TOLUA_DISABLE_tolua_OPC_UA_OPCUAServer_addTechObject00
static int tolua_OPC_UA_OPCUAServer_addTechObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"OPCUAServer",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  OPCUAServer* self = (OPCUAServer*)  tolua_tousertype(tolua_S,1,0);
  tech_object* tobj = ((tech_object*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addTechObject'", NULL);
#endif
  {
   self->addTechObject(tobj);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addTechObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: OPC_UA_SERV */
#ifndef TOLUA_DISABLE_tolua_OPC_UA_OPC_UA_SERV00
static int tolua_OPC_UA_OPC_UA_SERV00(lua_State* tolua_S)
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
   OPCUAServer& tolua_ret = (OPCUAServer&)  OPC_UA_SERV();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"OPCUAServer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OPC_UA_SERV'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_OPC_UA_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"OPCUAServer","OPCUAServer","",NULL);
  tolua_beginmodule(tolua_S,"OPCUAServer");
   tolua_function(tolua_S,"BaseConfig",tolua_OPC_UA_OPCUAServer_BaseConfig00);
   tolua_function(tolua_S,"addPumpObject",tolua_OPC_UA_OPCUAServer_addPumpObject00);
   tolua_function(tolua_S,"addTechObject",tolua_OPC_UA_OPCUAServer_addTechObject00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"OPC_UA_SERV",tolua_OPC_UA_OPC_UA_SERV00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_OPC_UA (lua_State* tolua_S) {
 return tolua_OPC_UA_open(tolua_S);
};
#endif

