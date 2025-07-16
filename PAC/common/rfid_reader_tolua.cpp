/*
** Lua binding: rfid_reader
** Generated automatically by tolua++-1.0.92 on 05/24/18 17:39:53.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_rfid_reader_open (lua_State* tolua_S);

#include "rfid_reader.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"EPC_info");
 tolua_usertype(tolua_S,"rfid_reader");
}

/* get function: EPC_str of class  EPC_info */
#ifndef TOLUA_DISABLE_tolua_get_EPC_info_EPC_str
static int tolua_get_EPC_info_EPC_str(lua_State* tolua_S)
{
  EPC_info* self = (EPC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'EPC_str'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)self->EPC_str);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: EPC_str of class  EPC_info */
#ifndef TOLUA_DISABLE_tolua_set_EPC_info_EPC_str
static int tolua_set_EPC_info_EPC_str(lua_State* tolua_S)
{
  EPC_info* self = (EPC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'EPC_str'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->EPC_str = ((char*)  tolua_tostring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: antenna of class  EPC_info */
#ifndef TOLUA_DISABLE_tolua_get_EPC_info_antenna
static int tolua_get_EPC_info_antenna(lua_State* tolua_S)
{
  EPC_info* self = (EPC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'antenna'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->antenna);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: antenna of class  EPC_info */
#ifndef TOLUA_DISABLE_tolua_set_EPC_info_antenna
static int tolua_set_EPC_info_antenna(lua_State* tolua_S)
{
  EPC_info* self = (EPC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'antenna'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->antenna = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: RSSI of class  EPC_info */
#ifndef TOLUA_DISABLE_tolua_get_EPC_info_RSSI
static int tolua_get_EPC_info_RSSI(lua_State* tolua_S)
{
  EPC_info* self = (EPC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'RSSI'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->RSSI);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: RSSI of class  EPC_info */
#ifndef TOLUA_DISABLE_tolua_set_EPC_info_RSSI
static int tolua_set_EPC_info_RSSI(lua_State* tolua_S)
{
  EPC_info* self = (EPC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'RSSI'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->RSSI = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_reader of class  rfid_reader */
#ifndef TOLUA_DISABLE_tolua_rfid_reader_rfid_reader_add_reader00
static int tolua_rfid_reader_rfid_reader_add_reader00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"rfid_reader",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* ip_address = ((const char*)  tolua_tostring(tolua_S,2,0));
  int number = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   rfid_reader* tolua_ret = (rfid_reader*)  rfid_reader::add_reader(ip_address,number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"rfid_reader");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_reader'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluate of class  rfid_reader */
#ifndef TOLUA_DISABLE_tolua_rfid_reader_rfid_reader_evaluate00
static int tolua_rfid_reader_rfid_reader_evaluate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"rfid_reader",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  rfid_reader* self = (rfid_reader*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'evaluate'", NULL);
#endif
  {
   int tolua_ret = (int)  self->evaluate();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'evaluate'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_EPC_info of class  rfid_reader */
#ifndef TOLUA_DISABLE_tolua_rfid_reader_rfid_reader_get_EPC_info00
static int tolua_rfid_reader_rfid_reader_get_EPC_info00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const rfid_reader",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const rfid_reader* self = (const rfid_reader*)  tolua_tousertype(tolua_S,1,0);
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_EPC_info'", NULL);
#endif
  {
   const EPC_info* tolua_ret = (const EPC_info*)  self->get_EPC_info(idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"const EPC_info");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_EPC_info'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_EPC_cnt of class  rfid_reader */
#ifndef TOLUA_DISABLE_tolua_rfid_reader_rfid_reader_get_EPC_cnt00
static int tolua_rfid_reader_rfid_reader_get_EPC_cnt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const rfid_reader",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const rfid_reader* self = (const rfid_reader*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_EPC_cnt'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_EPC_cnt();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_EPC_cnt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_rfid_reader_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"EPC_info","EPC_info","",NULL);
  tolua_beginmodule(tolua_S,"EPC_info");
   tolua_variable(tolua_S,"EPC_str",tolua_get_EPC_info_EPC_str,tolua_set_EPC_info_EPC_str);
   tolua_variable(tolua_S,"antenna",tolua_get_EPC_info_antenna,tolua_set_EPC_info_antenna);
   tolua_variable(tolua_S,"RSSI",tolua_get_EPC_info_RSSI,tolua_set_EPC_info_RSSI);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"rfid_reader","rfid_reader","",NULL);
  tolua_beginmodule(tolua_S,"rfid_reader");
   tolua_constant(tolua_S,"ST_ACTIVE",rfid_reader::ST_ACTIVE);
   tolua_constant(tolua_S,"ST_CANT_CONNECT",rfid_reader::ST_CANT_CONNECT);
   tolua_function(tolua_S,"add_reader",tolua_rfid_reader_rfid_reader_add_reader00);
   tolua_function(tolua_S,"evaluate",tolua_rfid_reader_rfid_reader_evaluate00);
   tolua_function(tolua_S,"get_EPC_info",tolua_rfid_reader_rfid_reader_get_EPC_info00);
   tolua_function(tolua_S,"get_EPC_cnt",tolua_rfid_reader_rfid_reader_get_EPC_cnt00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_rfid_reader (lua_State* tolua_S) {
 return tolua_rfid_reader_open(tolua_S);
};
#endif

