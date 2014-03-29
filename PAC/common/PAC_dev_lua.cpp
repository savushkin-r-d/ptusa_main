/*
** Lua binding: PAC_dev
** Generated automatically by tolua++-1.0.92 on 03/29/14 14:48:23.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_PAC_dev_open (lua_State* tolua_S);

#include <stdlib.h>
#include "PAC_dev.h"
#include "tech_def.h"
#include "cip_tech_def.h"
#include "wago.h"
#include "tolua_base.h"
#include "PID.h"
#include "g_device.h"
#include "errors.h"
#include "rm_manager.h"
#include "modbus_client.h"
#include "modbus_serv.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_PID (lua_State* tolua_S)
{
 PID* self = (PID*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_timer (lua_State* tolua_S)
{
 timer* self = (timer*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_cipline_tech_object (lua_State* tolua_S)
{
 cipline_tech_object* self = (cipline_tech_object*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_modbus_client (lua_State* tolua_S)
{
 modbus_client* self = (modbus_client*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_tech_object (lua_State* tolua_S)
{
 tech_object* self = (tech_object*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"ModbusServ");
 tolua_usertype(tolua_S,"i_DO_AO_device");
 tolua_usertype(tolua_S,"action");
 tolua_usertype(tolua_S,"modbus_client");
 tolua_usertype(tolua_S,"rm_manager");
 tolua_usertype(tolua_S,"mode_manager");
 tolua_usertype(tolua_S,"step");
 tolua_usertype(tolua_S,"i_Lua_save_device");
 tolua_usertype(tolua_S,"dev_errors_manager");
 tolua_usertype(tolua_S,"i_DO_device");
 tolua_usertype(tolua_S,"PAC_info");
 tolua_usertype(tolua_S,"PID");
 tolua_usertype(tolua_S,"wago_manager");
 tolua_usertype(tolua_S,"mode");
 tolua_usertype(tolua_S,"run_time_params_float");
 tolua_usertype(tolua_S,"run_time_params_u_int_4");
 tolua_usertype(tolua_S,"cipline_tech_object");
 tolua_usertype(tolua_S,"i_DI_device");
 tolua_usertype(tolua_S,"saved_params_u_int_4");
 tolua_usertype(tolua_S,"saved_params_float");
 tolua_usertype(tolua_S,"device");
 tolua_usertype(tolua_S,"i_counter");
 tolua_usertype(tolua_S,"wago_device");
 tolua_usertype(tolua_S,"i_AO_device");
 tolua_usertype(tolua_S,"timer_manager");
 tolua_usertype(tolua_S,"tech_object_manager");
 tolua_usertype(tolua_S,"i_AI_device");
 tolua_usertype(tolua_S,"device_manager");
 tolua_usertype(tolua_S,"timer");
 tolua_usertype(tolua_S,"valve");
 tolua_usertype(tolua_S,"tech_object");
 tolua_usertype(tolua_S,"dev_stub");
}

/* method: get_state of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_get_state00
static int tolua_PAC_dev_i_DI_device_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DI_device* self = (i_DI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_active of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_is_active00
static int tolua_PAC_dev_i_DI_device_is_active00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DI_device* self = (i_DI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_active'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_active();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_active'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_on00
static int tolua_PAC_dev_i_DO_device_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_off00
static int tolua_PAC_dev_i_DO_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_set_state00
static int tolua_PAC_dev_i_DO_device_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  i_AI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AI_device_get_value00
static int tolua_PAC_dev_i_AI_device_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AI_device* self = (i_AI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AO_device_off00
static int tolua_PAC_dev_i_AO_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AO_device* self = (i_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  i_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AO_device_set_value00
static int tolua_PAC_dev_i_AO_device_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AO_device* self = (i_AO_device*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_get_state00
static int tolua_PAC_dev_i_DO_AO_device_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_set_state00
static int tolua_PAC_dev_i_DO_AO_device_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_on00
static int tolua_PAC_dev_i_DO_AO_device_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_off00
static int tolua_PAC_dev_i_DO_AO_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_set_state01
static int tolua_PAC_dev_i_DO_AO_device_set_state01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
tolua_lerror:
 return tolua_PAC_dev_i_DO_AO_device_set_state00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_get_value00
static int tolua_PAC_dev_i_DO_AO_device_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_off01
static int tolua_PAC_dev_i_DO_AO_device_off01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
tolua_lerror:
 return tolua_PAC_dev_i_DO_AO_device_off00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_set_value00
static int tolua_PAC_dev_i_DO_AO_device_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: __i_AO_device__ of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_get_i_DO_AO_device___i_AO_device__
static int tolua_get_i_DO_AO_device___i_AO_device__(lua_State* tolua_S)
{
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable '__i_AO_device__'",NULL);
#endif
#ifdef __cplusplus
   tolua_pushusertype(tolua_S,(void*)static_cast<i_AO_device*>(self), "i_AO_device");
#else
   tolua_pushusertype(tolua_S,(void*)((i_AO_device*)self), "i_AO_device");
#endif
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: pause of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_pause00
static int tolua_PAC_dev_i_counter_pause00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pause'", NULL);
#endif
  {
   self->pause();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pause'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: start of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_start00
static int tolua_PAC_dev_i_counter_start00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'start'", NULL);
#endif
  {
   self->start();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'start'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_reset00
static int tolua_PAC_dev_i_counter_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: restart of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_restart00
static int tolua_PAC_dev_i_counter_restart00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'restart'", NULL);
#endif
  {
   self->restart();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'restart'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_quantity of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_quantity00
static int tolua_PAC_dev_i_counter_get_quantity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_quantity'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_quantity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_quantity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_flow of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_flow00
static int tolua_PAC_dev_i_counter_get_flow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_flow'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_flow();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_flow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_state00
static int tolua_PAC_dev_i_counter_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_cmd00
static int tolua_PAC_dev_device_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_par of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_par00
static int tolua_PAC_dev_device_set_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  float value = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_par'", NULL);
#endif
  {
   self->set_par(idx,offset,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_rt_par of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_rt_par00
static int tolua_PAC_dev_device_set_rt_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_rt_par'", NULL);
#endif
  {
   self->set_rt_par(idx,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_rt_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_property of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_property00
static int tolua_PAC_dev_device_set_property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* field = ((const char*)  tolua_tostring(tolua_S,2,0));
  device* dev = ((device*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_property'", NULL);
#endif
  {
   self->set_property(field,dev);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_descr of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_descr00
static int tolua_PAC_dev_device_set_descr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* description = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_descr'", NULL);
#endif
  {
   self->set_descr(description);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_descr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: V */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_V00
static int tolua_PAC_dev_V00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  V(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'V'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: VC */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_VC00
static int tolua_PAC_dev_VC00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AO_device* tolua_ret = (i_AO_device*)  VC(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'VC'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: M */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_M00
static int tolua_PAC_dev_M00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_AO_device* tolua_ret = (i_DO_AO_device*)  M(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'M'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_LS00
static int tolua_PAC_dev_LS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  LS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: FS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FS00
static int tolua_PAC_dev_FS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  FS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: AI */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_AI00
static int tolua_PAC_dev_AI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  AI(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: AO */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_AO00
static int tolua_PAC_dev_AO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AO_device* tolua_ret = (i_AO_device*)  AO(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: FQT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FQT00
static int tolua_PAC_dev_FQT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_counter* tolua_ret = (i_counter*)  FQT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_counter");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FQT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: TE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_TE00
static int tolua_PAC_dev_TE00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  TE(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'TE'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_LT00
static int tolua_PAC_dev_LT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  LT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_GS00
static int tolua_PAC_dev_GS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  GS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: HA */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_HA00
static int tolua_PAC_dev_HA00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  HA(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HA'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: HL */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_HL00
static int tolua_PAC_dev_HL00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  HL(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HL'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SB */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_SB00
static int tolua_PAC_dev_SB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  SB(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SB'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DI */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_DI00
static int tolua_PAC_dev_DI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  DI(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DO */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_DO00
static int tolua_PAC_dev_DO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  DO(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: QT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_QT00
static int tolua_PAC_dev_QT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  QT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'QT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: STUB */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_STUB00
static int tolua_PAC_dev_STUB00(lua_State* tolua_S)
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
   dev_stub* tolua_ret = (dev_stub*)  STUB();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"dev_stub");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'STUB'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DEVICE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_DEVICE00
static int tolua_PAC_dev_DEVICE00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int s_number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   device* tolua_ret = (device*)  DEVICE(s_number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DEVICE'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_get_value00
static int tolua_PAC_dev_dev_stub_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_set_value00
static int tolua_PAC_dev_dev_stub_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_on00
static int tolua_PAC_dev_dev_stub_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_off00
static int tolua_PAC_dev_dev_stub_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_set_state00
static int tolua_PAC_dev_dev_stub_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_get_state00
static int tolua_PAC_dev_dev_stub_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pause of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_pause00
static int tolua_PAC_dev_dev_stub_pause00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pause'", NULL);
#endif
  {
   self->pause();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pause'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: start of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_start00
static int tolua_PAC_dev_dev_stub_start00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'start'", NULL);
#endif
  {
   self->start();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'start'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_reset00
static int tolua_PAC_dev_dev_stub_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_quantity of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_get_quantity00
static int tolua_PAC_dev_dev_stub_get_quantity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_quantity'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_quantity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_quantity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_instance of class  dev_errors_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_errors_manager_get_instance00
static int tolua_PAC_dev_dev_errors_manager_get_instance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"dev_errors_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   dev_errors_manager* tolua_ret = (dev_errors_manager*)  dev_errors_manager::get_instance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"dev_errors_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_instance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  dev_errors_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_errors_manager_set_cmd00
static int tolua_PAC_dev_dev_errors_manager_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_errors_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_errors_manager* self = (dev_errors_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int cmd = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int object_type = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int object_number = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int object_alarm_number = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   self->set_cmd(cmd,object_type,object_number,object_alarm_number);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_set_cmd00
static int tolua_PAC_dev_tech_object_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_new00
static int tolua_PAC_dev_tech_object_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   tech_object* tolua_ret = (tech_object*)  Mtolua_new((tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_new00_local
static int tolua_PAC_dev_tech_object_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   tech_object* tolua_ret = (tech_object*)  Mtolua_new((tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_delete00
static int tolua_PAC_dev_tech_object_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_set_mode00
static int tolua_PAC_dev_tech_object_set_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int new_state = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_mode(mode,new_state);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_mode00
static int tolua_PAC_dev_tech_object_get_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_mode(mode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: exec_cmd of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_exec_cmd00
static int tolua_PAC_dev_tech_object_exec_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int cmd = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'exec_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->exec_cmd(cmd);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'exec_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_modes_count of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_modes_count00
static int tolua_PAC_dev_tech_object_get_modes_count00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_modes_count'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_modes_count();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_modes_count'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_par_float
static int tolua_get_tech_object_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par_float,"saved_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_par_float
static int tolua_set_tech_object_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par_float = *((saved_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rt_par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_rt_par_float
static int tolua_get_tech_object_rt_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->rt_par_float,"run_time_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rt_par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_rt_par_float
static int tolua_set_tech_object_rt_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"run_time_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rt_par_float = *((run_time_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_par_uint
static int tolua_get_tech_object_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_uint'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par_uint,"saved_params_u_int_4");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_par_uint
static int tolua_set_tech_object_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_uint'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_u_int_4",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par_uint = *((saved_params_u_int_4*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rt_par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_rt_par_uint
static int tolua_get_tech_object_rt_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_uint'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->rt_par_uint,"run_time_params_u_int_4");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rt_par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_rt_par_uint
static int tolua_set_tech_object_rt_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_uint'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"run_time_params_u_int_4",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rt_par_uint = *((run_time_params_u_int_4*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: timers of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_timers
static int tolua_get_tech_object_timers(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'timers'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->timers,"timer_manager");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: timers of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_timers
static int tolua_set_tech_object_timers(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'timers'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"timer_manager",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->timers = *((timer_manager*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_modes_manager of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_modes_manager00
static int tolua_PAC_dev_tech_object_get_modes_manager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_modes_manager'", NULL);
#endif
  {
   mode_manager* tolua_ret = (mode_manager*)  self->get_modes_manager();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"mode_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_modes_manager'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_idle of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_is_idle00
static int tolua_PAC_dev_tech_object_is_idle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_idle'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_idle();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_idle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_number of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_number00
static int tolua_PAC_dev_tech_object_get_number00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_number'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_number();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_number'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_err_msg of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_set_err_msg00
static int tolua_PAC_dev_tech_object_set_err_msg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* err_msg = ((const char*)  tolua_tostring(tolua_S,2,0));
  int mode = ((int)  tolua_tonumber(tolua_S,3,0));
  int new_mode = ((int)  tolua_tonumber(tolua_S,4,0));
  tech_object::ERR_MSG_TYPES type = ((tech_object::ERR_MSG_TYPES) (int)  tolua_tonumber(tolua_S,5,tech_object::ERR_CANT_ON));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_err_msg'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_err_msg(err_msg,mode,new_mode,type);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_err_msg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_TECH_OBJECT_MNGR */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_TECH_OBJECT_MNGR00
static int tolua_PAC_dev_G_TECH_OBJECT_MNGR00(lua_State* tolua_S)
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
   tech_object_manager* tolua_ret = (tech_object_manager*)  G_TECH_OBJECT_MNGR();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_TECH_OBJECT_MNGR'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_object_with_active_mode of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00
static int tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object_manager* self = (tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int start_idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int end_idx = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_object_with_active_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_object_with_active_mode(mode,start_idx,end_idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_object_with_active_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_tech_objects of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_get_tech_objects00
static int tolua_PAC_dev_tech_object_manager_get_tech_objects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object_manager* self = (tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_tech_objects'", NULL);
#endif
  {
   tech_object* tolua_ret = (tech_object*)  self->get_tech_objects(idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_tech_objects'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: print of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_print00
static int tolua_PAC_dev_tech_object_manager_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object_manager* self = (tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'print'", NULL);
#endif
  {
   self->print();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_mode of class  mode_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_manager_add_mode00
static int tolua_PAC_dev_mode_manager_add_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode_manager* self = (mode_manager*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_mode'", NULL);
#endif
  {
   mode* tolua_ret = (mode*)  self->add_mode(name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"mode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  mode_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_manager__geti00
static int tolua_PAC_dev_mode_manager__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode_manager* self = (mode_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   mode* tolua_ret = (mode*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"mode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_idle_time of class  mode_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_manager_get_idle_time00
static int tolua_PAC_dev_mode_manager_get_idle_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode_manager* self = (mode_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_idle_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_idle_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_idle_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluation_time of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_evaluation_time00
static int tolua_PAC_dev_mode_evaluation_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode* self = (mode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'evaluation_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->evaluation_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'evaluation_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_step of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_add_step00
static int tolua_PAC_dev_mode_add_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode* self = (mode*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  int next_step_n = ((int)  tolua_tonumber(tolua_S,3,0));
  unsigned int step_duration_par_n = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_step'", NULL);
#endif
  {
   step* tolua_ret = (step*)  self->add_step(name,next_step_n,step_duration_par_n);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"step");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_step_cooperate_time_par_n of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_set_step_cooperate_time_par_n00
static int tolua_PAC_dev_mode_set_step_cooperate_time_par_n00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode* self = (mode*)  tolua_tousertype(tolua_S,1,0);
  int step_cooperate_time_par_n = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_step_cooperate_time_par_n'", NULL);
#endif
  {
   self->set_step_cooperate_time_par_n(step_cooperate_time_par_n);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_step_cooperate_time_par_n'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: active_step of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_active_step00
static int tolua_PAC_dev_mode_active_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const mode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const mode* self = (const mode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'active_step'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->active_step();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'active_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: active_step_evaluation_time of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_active_step_evaluation_time00
static int tolua_PAC_dev_mode_active_step_evaluation_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const mode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const mode* self = (const mode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'active_step_evaluation_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->active_step_evaluation_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'active_step_evaluation_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode__geti00
static int tolua_PAC_dev_mode__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode* self = (mode*)  tolua_tousertype(tolua_S,1,0);
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   step* tolua_ret = (step*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"step");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: to_step of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_to_step00
static int tolua_PAC_dev_mode_to_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  mode* self = (mode*)  tolua_tousertype(tolua_S,1,0);
  unsigned int new_step = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'to_step'", NULL);
#endif
  {
   self->to_step(new_step);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'to_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: to_step of class  mode */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_mode_to_step01
static int tolua_PAC_dev_mode_to_step01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"mode",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  mode* self = (mode*)  tolua_tousertype(tolua_S,1,0);
  unsigned int new_step = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned long cooperative_time = ((unsigned long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'to_step'", NULL);
#endif
  {
   self->to_step(new_step,cooperative_time);
  }
 }
 return 0;
tolua_lerror:
 return tolua_PAC_dev_mode_to_step00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step__geti00
static int tolua_PAC_dev_step__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"step",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  step* self = (step*)  tolua_tousertype(tolua_S,1,0);
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   action* tolua_ret = (action*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"action");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_name of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step_get_name00
static int tolua_PAC_dev_step_get_name00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const step",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const step* self = (const step*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_name'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->get_name();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_name'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_dev of class  action */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_action_add_dev00
static int tolua_PAC_dev_action_add_dev00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"action",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  action* self = (action*)  tolua_tousertype(tolua_S,1,0);
  device* dev = ((device*)  tolua_tousertype(tolua_S,2,0));
  unsigned int group = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_dev'", NULL);
#endif
  {
   self->add_dev(dev,group);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_dev'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_dev of class  action */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_action_add_dev01
static int tolua_PAC_dev_action_add_dev01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"action",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  action* self = (action*)  tolua_tousertype(tolua_S,1,0);
  device* dev = ((device*)  tolua_tousertype(tolua_S,2,0));
  unsigned int group = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int subgroup = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_dev'", NULL);
#endif
  {
   self->add_dev(dev,group,subgroup);
  }
 }
 return 0;
tolua_lerror:
 return tolua_PAC_dev_action_add_dev00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_DEVICE_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_DEVICE_MANAGER00
static int tolua_PAC_dev_G_DEVICE_MANAGER00(lua_State* tolua_S)
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
   device_manager* tolua_ret = (device_manager*)  G_DEVICE_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_DEVICE_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_TECH_OBJECTS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_TECH_OBJECTS00
static int tolua_PAC_dev_G_TECH_OBJECTS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   tech_object* tolua_ret = (tech_object*)  G_TECH_OBJECTS(idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_TECH_OBJECTS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: print of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_print00
static int tolua_PAC_dev_device_manager_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const device_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const device_manager* self = (const device_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'print'", NULL);
#endif
  {
   self->print();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_wago_device of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_add_wago_device00
static int tolua_PAC_dev_device_manager_add_wago_device00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device_manager* self = (device_manager*)  tolua_tousertype(tolua_S,1,0);
  int dev_type = ((int)  tolua_tonumber(tolua_S,2,0));
  int dev_sub_type = ((int)  tolua_tonumber(tolua_S,3,0));
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,4,0));
  char* descr = ((char*)  tolua_tostring(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_wago_device'", NULL);
#endif
  {
   wago_device* tolua_ret = (wago_device*)  self->add_wago_device(dev_type,dev_sub_type,dev_name,descr);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"wago_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_wago_device'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_device of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_get_device00
static int tolua_PAC_dev_device_manager_get_device00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device_manager* self = (device_manager*)  tolua_tousertype(tolua_S,1,0);
  int dev_type = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_device'", NULL);
#endif
  {
   device* tolua_ret = (device*)  self->get_device(dev_type,dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_device'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  wago_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_wago_device_init00
static int tolua_PAC_dev_wago_device_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"wago_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  wago_device* self = (wago_device*)  tolua_tousertype(tolua_S,1,0);
  int DO_count = ((int)  tolua_tonumber(tolua_S,2,0));
  int DI_count = ((int)  tolua_tonumber(tolua_S,3,0));
  int AO_count = ((int)  tolua_tonumber(tolua_S,4,0));
  int AI_count = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init'", NULL);
#endif
  {
   self->init(DO_count,DI_count,AO_count,AI_count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_channel of class  wago_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_wago_device_init_channel00
static int tolua_PAC_dev_wago_device_init_channel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"wago_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  wago_device* self = (wago_device*)  tolua_tousertype(tolua_S,1,0);
  int type = ((int)  tolua_tonumber(tolua_S,2,0));
  int ch_inex = ((int)  tolua_tonumber(tolua_S,3,0));
  int node = ((int)  tolua_tonumber(tolua_S,4,0));
  int offset = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_channel'", NULL);
#endif
  {
   self->init_channel(type,ch_inex,node,offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_channel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  timer_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_manager__geti00
static int tolua_PAC_dev_timer_manager__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer_manager* self = (timer_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   timer* tolua_ret = (timer*)  self->operator[](index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"timer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: start of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_start00
static int tolua_PAC_dev_timer_start00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'start'", NULL);
#endif
  {
   self->start();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'start'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_reset00
static int tolua_PAC_dev_timer_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pause of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_pause00
static int tolua_PAC_dev_timer_pause00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pause'", NULL);
#endif
  {
   self->pause();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pause'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_time_up of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_is_time_up00
static int tolua_PAC_dev_timer_is_time_up00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_time_up'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_time_up();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_time_up'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_work_time of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_get_work_time00
static int tolua_PAC_dev_timer_get_work_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_work_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_work_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_work_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_countdown_time of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_set_countdown_time00
static int tolua_PAC_dev_timer_set_countdown_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
  unsigned long new_countdown_time = ((unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_countdown_time'", NULL);
#endif
  {
   self->set_countdown_time(new_countdown_time);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_countdown_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_countdown_time of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_get_countdown_time00
static int tolua_PAC_dev_timer_get_countdown_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_countdown_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_countdown_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_countdown_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_get_state00
static int tolua_PAC_dev_timer_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   timer::STATE tolua_ret = (timer::STATE)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_new00
static int tolua_PAC_dev_timer_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   timer* tolua_ret = (timer*)  Mtolua_new((timer)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"timer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_new00_local
static int tolua_PAC_dev_timer_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   timer* tolua_ret = (timer*)  Mtolua_new((timer)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"timer");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_save00
static int tolua_PAC_dev_saved_params_float_save00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save(idx,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save_all of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_save_all00
static int tolua_PAC_dev_saved_params_float_save_all00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save_all'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save_all();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save_all'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float__seti00
static int tolua_PAC_dev_saved_params_float__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float tolua_value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float__geti00
static int tolua_PAC_dev_saved_params_float__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   float tolua_ret = (float)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_reset_to_000
static int tolua_PAC_dev_saved_params_float_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_save00
static int tolua_PAC_dev_saved_params_u_int_4_save00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save(idx,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save_all of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_save_all00
static int tolua_PAC_dev_saved_params_u_int_4_save_all00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save_all'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save_all();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save_all'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4__seti00
static int tolua_PAC_dev_saved_params_u_int_4__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int tolua_value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4__geti00
static int tolua_PAC_dev_saved_params_u_int_4__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_reset_to_000
static int tolua_PAC_dev_saved_params_u_int_4_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4__seti00
static int tolua_PAC_dev_run_time_params_u_int_4__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_u_int_4* self = (run_time_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int tolua_value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4__geti00
static int tolua_PAC_dev_run_time_params_u_int_4__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_u_int_4* self = (run_time_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_reset_to_000
static int tolua_PAC_dev_run_time_params_u_int_4_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_u_int_4* self = (run_time_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float__seti00
static int tolua_PAC_dev_run_time_params_float__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_float* self = (run_time_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float tolua_value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float__geti00
static int tolua_PAC_dev_run_time_params_float__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_float* self = (run_time_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   float tolua_ret = (float)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_reset_to_000
static int tolua_PAC_dev_run_time_params_float_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_float* self = (run_time_params_float*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  wago_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_wago_manager_init00
static int tolua_PAC_dev_wago_manager_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"wago_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  wago_manager* self = (wago_manager*)  tolua_tousertype(tolua_S,1,0);
  int nodes_count = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init'", NULL);
#endif
  {
   self->init(nodes_count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_node of class  wago_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_wago_manager_add_node00
static int tolua_PAC_dev_wago_manager_add_node00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"wago_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isstring(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,12,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,13,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  wago_manager* self = (wago_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int ntype = ((int)  tolua_tonumber(tolua_S,3,0));
  int address = ((int)  tolua_tonumber(tolua_S,4,0));
  char* IP_address = ((char*)  tolua_tostring(tolua_S,5,0));
  char* name = ((char*)  tolua_tostring(tolua_S,6,0));
  int DO_cnt = ((int)  tolua_tonumber(tolua_S,7,0));
  int DI_cnt = ((int)  tolua_tonumber(tolua_S,8,0));
  int AO_cnt = ((int)  tolua_tonumber(tolua_S,9,0));
  int AO_size = ((int)  tolua_tonumber(tolua_S,10,0));
  int AI_cnt = ((int)  tolua_tonumber(tolua_S,11,0));
  int AI_size = ((int)  tolua_tonumber(tolua_S,12,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_node'", NULL);
#endif
  {
   self->add_node(index,ntype,address,IP_address,name,DO_cnt,DI_cnt,AO_cnt,AO_size,AI_cnt,AI_size);
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

/* method: init_node_AO of class  wago_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_wago_manager_init_node_AO00
static int tolua_PAC_dev_wago_manager_init_node_AO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"wago_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  wago_manager* self = (wago_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int node_index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int AO_index = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_node_AO'", NULL);
#endif
  {
   self->init_node_AO(node_index,AO_index,type,offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_node_AO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_node_AI of class  wago_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_wago_manager_init_node_AI00
static int tolua_PAC_dev_wago_manager_init_node_AI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"wago_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  wago_manager* self = (wago_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int node_index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int AI_index = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_node_AI'", NULL);
#endif
  {
   self->init_node_AI(node_index,AI_index,type,offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_node_AI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_WAGO_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_WAGO_MANAGER00
static int tolua_PAC_dev_G_WAGO_MANAGER00(lua_State* tolua_S)
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
   wago_manager* tolua_ret = (wago_manager*)  G_WAGO_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"wago_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_WAGO_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_new00
static int tolua_PAC_dev_PID_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int n = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   PID* tolua_ret = (PID*)  Mtolua_new((PID)(n));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PID");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_new00_local
static int tolua_PAC_dev_PID_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int n = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   PID* tolua_ret = (PID*)  Mtolua_new((PID)(n));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_on00
static int tolua_PAC_dev_PID_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  char is_down_to_inaccel_mode = ((char)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on(is_down_to_inaccel_mode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_off00
static int tolua_PAC_dev_PID_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_reset00
static int tolua_PAC_dev_PID_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: eval of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_eval00
static int tolua_PAC_dev_PID_eval00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  float current_value = ((float)  tolua_tonumber(tolua_S,2,0));
  int delta_sign = ((int)  tolua_tonumber(tolua_S,3,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'eval'", NULL);
#endif
  {
   float tolua_ret = (float)  self->eval(current_value,delta_sign);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'eval'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_set00
static int tolua_PAC_dev_PID_set00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  float new_z = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set'", NULL);
#endif
  {
   self->set(new_z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_assignment of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_get_assignment00
static int tolua_PAC_dev_PID_get_assignment00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_assignment'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_assignment();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_assignment'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_param of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_init_param00
static int tolua_PAC_dev_PID_init_param00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  PID::PARAM par_n = ((PID::PARAM) (int)  tolua_tonumber(tolua_S,2,0));
  float val = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_param'", NULL);
#endif
  {
   self->init_param(par_n,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_param'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_work_param of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_init_work_param00
static int tolua_PAC_dev_PID_init_work_param00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  PID::WORK_PARAM par_n = ((PID::WORK_PARAM) (int)  tolua_tonumber(tolua_S,2,0));
  float val = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_work_param'", NULL);
#endif
  {
   self->init_work_param(par_n,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_work_param'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save_param of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_save_param00
static int tolua_PAC_dev_PID_save_param00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save_param'", NULL);
#endif
  {
   self->save_param();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save_param'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_used_par of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_set_used_par00
static int tolua_PAC_dev_PID_set_used_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  int par_n = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_used_par'", NULL);
#endif
  {
   self->set_used_par(par_n);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_used_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: print of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_print00
static int tolua_PAC_dev_PID_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'print'", NULL);
#endif
  {
   self->print();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_set_cmd00
static int tolua_PAC_dev_PID_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  PAC_info */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PAC_info_set_cmd00
static int tolua_PAC_dev_PAC_info_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PAC_info",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PAC_info* self = (PAC_info*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_PAC_INFO */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_PAC_INFO00
static int tolua_PAC_dev_G_PAC_INFO00(lua_State* tolua_S)
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
   PAC_info* tolua_ret = (PAC_info*)  G_PAC_INFO();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PAC_info");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_PAC_INFO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_sec */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_sec00
static int tolua_PAC_dev_get_sec00(lua_State* tolua_S)
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
   unsigned long tolua_ret = (unsigned long)  get_sec();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_sec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_millisec */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_millisec00
static int tolua_PAC_dev_get_millisec00(lua_State* tolua_S)
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
   unsigned long tolua_ret = (unsigned long)  get_millisec();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_millisec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_delta_millisec */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_delta_millisec00
static int tolua_PAC_dev_get_delta_millisec00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned long time1 = ((unsigned long)  tolua_tonumber(tolua_S,1,0));
  {
   unsigned long tolua_ret = (unsigned long)  get_delta_millisec(time1);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_delta_millisec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: sleep_ms */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_sleep_ms00
static int tolua_PAC_dev_sleep_ms00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned long ms = ((unsigned long)  tolua_tonumber(tolua_S,1,0));
  {
   sleep_ms(ms);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sleep_ms'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_new00
static int tolua_PAC_dev_cipline_tech_object_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   cipline_tech_object* tolua_ret = (cipline_tech_object*)  Mtolua_new((cipline_tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"cipline_tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_new00_local
static int tolua_PAC_dev_cipline_tech_object_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   cipline_tech_object* tolua_ret = (cipline_tech_object*)  Mtolua_new((cipline_tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"cipline_tech_object");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: blocked of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_blocked
static int tolua_get_cipline_tech_object_blocked(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'blocked'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->blocked);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: blocked of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_blocked
static int tolua_set_cipline_tech_object_blocked(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'blocked'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->blocked = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: opcip of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_opcip
static int tolua_get_cipline_tech_object_opcip(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'opcip'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->opcip);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: opcip of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_opcip
static int tolua_set_cipline_tech_object_opcip(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'opcip'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->opcip = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: curstep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_curstep
static int tolua_get_cipline_tech_object_curstep(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curstep'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->curstep);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: curstep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_curstep
static int tolua_set_cipline_tech_object_curstep(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curstep'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->curstep = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: state of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_state
static int tolua_get_cipline_tech_object_state(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'state'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->state);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: state of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_state
static int tolua_set_cipline_tech_object_state(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'state'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->state = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: curprg of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_curprg
static int tolua_get_cipline_tech_object_curprg(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curprg'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->curprg);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: curprg of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_curprg
static int tolua_set_cipline_tech_object_curprg(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curprg'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->curprg = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nmr of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_nmr
static int tolua_get_cipline_tech_object_nmr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nmr'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->nmr);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nmr of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_nmr
static int tolua_set_cipline_tech_object_nmr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nmr'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nmr = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: cip_in_error of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_cip_in_error
static int tolua_get_cipline_tech_object_cip_in_error(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cip_in_error'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->cip_in_error);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: cip_in_error of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_cip_in_error
static int tolua_set_cipline_tech_object_cip_in_error(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cip_in_error'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->cip_in_error = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: disable_tank_heating of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_disable_tank_heating
static int tolua_get_cipline_tech_object_disable_tank_heating(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'disable_tank_heating'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->disable_tank_heating);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: disable_tank_heating of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_disable_tank_heating
static int tolua_set_cipline_tech_object_disable_tank_heating(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'disable_tank_heating'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->disable_tank_heating = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V00 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V00_ptr
static int tolua_get_cipline_tech_object_V00_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V00'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V00,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V00 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V00_ptr
static int tolua_set_cipline_tech_object_V00_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V00'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V00 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V01 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V01_ptr
static int tolua_get_cipline_tech_object_V01_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V01'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V01,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V01 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V01_ptr
static int tolua_set_cipline_tech_object_V01_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V01'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V01 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V03 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V03_ptr
static int tolua_get_cipline_tech_object_V03_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V03'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V03,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V03 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V03_ptr
static int tolua_set_cipline_tech_object_V03_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V03'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V03 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V02 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V02_ptr
static int tolua_get_cipline_tech_object_V02_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V02'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V02,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V02 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V02_ptr
static int tolua_set_cipline_tech_object_V02_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V02'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V02 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V04 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V04_ptr
static int tolua_get_cipline_tech_object_V04_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V04'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V04,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V04 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V04_ptr
static int tolua_set_cipline_tech_object_V04_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V04'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V04 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V05 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V05_ptr
static int tolua_get_cipline_tech_object_V05_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V05'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V05,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V05 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V05_ptr
static int tolua_set_cipline_tech_object_V05_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V05'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V05 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V06 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V06_ptr
static int tolua_get_cipline_tech_object_V06_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V06'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V06,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V06 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V06_ptr
static int tolua_set_cipline_tech_object_V06_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V06'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V06 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V07 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V07_ptr
static int tolua_get_cipline_tech_object_V07_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V07'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V07,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V07 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V07_ptr
static int tolua_set_cipline_tech_object_V07_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V07'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V07 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V08 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V08_ptr
static int tolua_get_cipline_tech_object_V08_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V08'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V08,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V08 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V08_ptr
static int tolua_set_cipline_tech_object_V08_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V08'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V08 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V09 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V09_ptr
static int tolua_get_cipline_tech_object_V09_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V09'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V09,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V09 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V09_ptr
static int tolua_set_cipline_tech_object_V09_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V09'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V09 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V10 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V10_ptr
static int tolua_get_cipline_tech_object_V10_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V10'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V10,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V10 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V10_ptr
static int tolua_set_cipline_tech_object_V10_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V10'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V10 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V11 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V11_ptr
static int tolua_get_cipline_tech_object_V11_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V11'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V11,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V11 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V11_ptr
static int tolua_set_cipline_tech_object_V11_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V11'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V11 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V12 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V12_ptr
static int tolua_get_cipline_tech_object_V12_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V12'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V12,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V12 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V12_ptr
static int tolua_set_cipline_tech_object_V12_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V12'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V12 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V13 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V13_ptr
static int tolua_get_cipline_tech_object_V13_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V13'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V13,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V13 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V13_ptr
static int tolua_set_cipline_tech_object_V13_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V13'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V13 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: NP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_NP_ptr
static int tolua_get_cipline_tech_object_NP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NP'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->NP,"i_DO_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: NP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_NP_ptr
static int tolua_set_cipline_tech_object_NP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NP'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->NP = ((i_DO_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: NK of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_NK_ptr
static int tolua_get_cipline_tech_object_NK_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NK'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->NK,"i_DO_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: NK of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_NK_ptr
static int tolua_set_cipline_tech_object_NK_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NK'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->NK = ((i_DO_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: NS of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_NS_ptr
static int tolua_get_cipline_tech_object_NS_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NS'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->NS,"i_DO_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: NS of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_NS_ptr
static int tolua_set_cipline_tech_object_NS_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NS'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->NS = ((i_DO_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LL_ptr
static int tolua_get_cipline_tech_object_LL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LL_ptr
static int tolua_set_cipline_tech_object_LL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LM of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LM_ptr
static int tolua_get_cipline_tech_object_LM_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LM'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LM,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LM of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LM_ptr
static int tolua_set_cipline_tech_object_LM_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LM'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LM = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LH_ptr
static int tolua_get_cipline_tech_object_LH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LH_ptr
static int tolua_set_cipline_tech_object_LH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LWL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LWL_ptr
static int tolua_get_cipline_tech_object_LWL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LWL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LWL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LWL_ptr
static int tolua_set_cipline_tech_object_LWL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LWL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LWH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LWH_ptr
static int tolua_get_cipline_tech_object_LWH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LWH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LWH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LWH_ptr
static int tolua_set_cipline_tech_object_LWH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LWH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LSL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LSL_ptr
static int tolua_get_cipline_tech_object_LSL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LSL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LSL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LSL_ptr
static int tolua_set_cipline_tech_object_LSL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LSL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LSH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LSH_ptr
static int tolua_get_cipline_tech_object_LSH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LSH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LSH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LSH_ptr
static int tolua_set_cipline_tech_object_LSH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LSH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LKL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LKL_ptr
static int tolua_get_cipline_tech_object_LKL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LKL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LKL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LKL_ptr
static int tolua_set_cipline_tech_object_LKL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LKL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LKH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LKH_ptr
static int tolua_get_cipline_tech_object_LKH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LKH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LKH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LKH_ptr
static int tolua_set_cipline_tech_object_LKH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LKH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: TP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_TP_ptr
static int tolua_get_cipline_tech_object_TP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TP'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->TP,"i_AI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: TP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_TP_ptr
static int tolua_set_cipline_tech_object_TP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TP'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->TP = ((i_AI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: TR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_TR_ptr
static int tolua_get_cipline_tech_object_TR_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TR'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->TR,"i_AI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: TR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_TR_ptr
static int tolua_set_cipline_tech_object_TR_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TR'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->TR = ((i_AI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: Q of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_Q_ptr
static int tolua_get_cipline_tech_object_Q_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Q'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->Q,"i_AI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: Q of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_Q_ptr
static int tolua_set_cipline_tech_object_Q_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Q'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->Q = ((i_AI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ao of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ao_ptr
static int tolua_get_cipline_tech_object_ao_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ao'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->ao,"i_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ao of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ao_ptr
static int tolua_set_cipline_tech_object_ao_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ao'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ao = ((i_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: PUMPFREQ of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_PUMPFREQ_ptr
static int tolua_get_cipline_tech_object_PUMPFREQ_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PUMPFREQ'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->PUMPFREQ,"i_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: PUMPFREQ of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_PUMPFREQ_ptr
static int tolua_set_cipline_tech_object_PUMPFREQ_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PUMPFREQ'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->PUMPFREQ = ((i_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: FL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_FL_ptr
static int tolua_get_cipline_tech_object_FL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'FL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->FL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: FL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_FL_ptr
static int tolua_set_cipline_tech_object_FL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'FL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->FL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: cnt of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_cnt_ptr
static int tolua_get_cipline_tech_object_cnt_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cnt'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->cnt,"i_counter");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: cnt of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_cnt_ptr
static int tolua_set_cipline_tech_object_cnt_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cnt'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_counter",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->cnt = ((i_counter*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: msa_number of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_msa_number
static int tolua_get_cipline_tech_object_msa_number(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'msa_number'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->msa_number);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: msa_number of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_msa_number
static int tolua_set_cipline_tech_object_msa_number(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'msa_number'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->msa_number = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_par_float
static int tolua_get_cipline_tech_object_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par_float,"saved_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_par_float
static int tolua_set_cipline_tech_object_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par_float = *((saved_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rt_par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_rt_par_float
static int tolua_get_cipline_tech_object_rt_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->rt_par_float,"run_time_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rt_par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_rt_par_float
static int tolua_set_cipline_tech_object_rt_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"run_time_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rt_par_float = *((run_time_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_station_par of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_get_station_par00
static int tolua_PAC_dev_cipline_tech_object_get_station_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int parno = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_station_par'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_station_par(parno);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_station_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_station_par of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_station_par00
static int tolua_PAC_dev_cipline_tech_object_set_station_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int parno = ((int)  tolua_tonumber(tolua_S,2,0));
  float newval = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_station_par'", NULL);
#endif
  {
   self->set_station_par(parno,newval);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_station_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_cmd00
static int tolua_PAC_dev_cipline_tech_object_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  const char* val = ((const char*)  tolua_tostring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_cmd01
static int tolua_PAC_dev_cipline_tech_object_set_cmd01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_PAC_dev_cipline_tech_object_set_cmd00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: initline of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_initline00
static int tolua_PAC_dev_cipline_tech_object_initline00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'initline'", NULL);
#endif
  {
   self->initline();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'initline'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluate of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_evaluate00
static int tolua_PAC_dev_cipline_tech_object_evaluate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
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

/* function: G_RM_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_RM_MANAGER00
static int tolua_PAC_dev_G_RM_MANAGER00(lua_State* tolua_S)
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
   rm_manager* tolua_ret = (rm_manager*)  G_RM_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"rm_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_RM_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_rm_cmmctr of class  rm_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_rm_manager_add_rm_cmmctr00
static int tolua_PAC_dev_rm_manager_add_rm_cmmctr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"rm_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  rm_manager* self = (rm_manager*)  tolua_tousertype(tolua_S,1,0);
  char* name = ((char*)  tolua_tostring(tolua_S,2,0));
  char* IP_address = ((char*)  tolua_tostring(tolua_S,3,0));
  int remote_PAC_id = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_rm_cmmctr'", NULL);
#endif
  {
   self->add_rm_cmmctr(name,IP_address,remote_PAC_id);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_rm_cmmctr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_new00
static int tolua_PAC_dev_modbus_client_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int id = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  char* ip = ((char*)  tolua_tostring(tolua_S,3,0));
  unsigned int port = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned long exchangetimeout = ((unsigned long)  tolua_tonumber(tolua_S,5,0));
  {
   modbus_client* tolua_ret = (modbus_client*)  Mtolua_new((modbus_client)(id,ip,port,exchangetimeout));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"modbus_client");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_new00_local
static int tolua_PAC_dev_modbus_client_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int id = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  char* ip = ((char*)  tolua_tostring(tolua_S,3,0));
  unsigned int port = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned long exchangetimeout = ((unsigned long)  tolua_tonumber(tolua_S,5,0));
  {
   modbus_client* tolua_ret = (modbus_client*)  Mtolua_new((modbus_client)(id,ip,port,exchangetimeout));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"modbus_client");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_discrete_inputs of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_discrete_inputs00
static int tolua_PAC_dev_modbus_client_read_discrete_inputs00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int start_address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_discrete_inputs'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_discrete_inputs(start_address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_discrete_inputs'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_coils of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_coils00
static int tolua_PAC_dev_modbus_client_read_coils00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int start_address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_coils'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_coils(start_address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_coils'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_holding_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_holding_registers00
static int tolua_PAC_dev_modbus_client_read_holding_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_holding_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_holding_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_holding_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_input_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_input_registers00
static int tolua_PAC_dev_modbus_client_read_input_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_input_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_input_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_input_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: write_coil of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_write_coil00
static int tolua_PAC_dev_modbus_client_write_coil00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned char value = ((unsigned char)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'write_coil'", NULL);
#endif
  {
   int tolua_ret = (int)  self->write_coil(address,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_coil'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: force_multiply_coils of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_force_multiply_coils00
static int tolua_PAC_dev_modbus_client_force_multiply_coils00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'force_multiply_coils'", NULL);
#endif
  {
   int tolua_ret = (int)  self->force_multiply_coils(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'force_multiply_coils'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: write_multiply_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_write_multiply_registers00
static int tolua_PAC_dev_modbus_client_write_multiply_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'write_multiply_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->write_multiply_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_multiply_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: zero_output_buff of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_zero_output_buff00
static int tolua_PAC_dev_modbus_client_zero_output_buff00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'zero_output_buff'", NULL);
#endif
  {
   self->zero_output_buff();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'zero_output_buff'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int2 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int200
static int tolua_PAC_dev_modbus_client_set_int200(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  short value = ((short)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int2'", NULL);
#endif
  {
   self->set_int2(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int2'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int2 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int200
static int tolua_PAC_dev_modbus_client_get_int200(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int2'", NULL);
#endif
  {
   short tolua_ret = (short)  self->get_int2(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int2'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int4 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int400
static int tolua_PAC_dev_modbus_client_set_int400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int4'", NULL);
#endif
  {
   self->set_int4(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int4'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int4 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int400
static int tolua_PAC_dev_modbus_client_get_int400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int4'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_int4(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int4'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_float of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_float00
static int tolua_PAC_dev_modbus_client_set_float00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_float'", NULL);
#endif
  {
   self->set_float(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_float'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_float of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_float00
static int tolua_PAC_dev_modbus_client_get_float00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_float'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_float(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_float'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_bit00
static int tolua_PAC_dev_modbus_client_set_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_bit'", NULL);
#endif
  {
   self->set_bit(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_bit00
static int tolua_PAC_dev_modbus_client_get_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_bit'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_bit(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_id of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_id00
static int tolua_PAC_dev_modbus_client_get_id00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_id'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_id();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_id'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UnpackInt16 of class  ModbusServ */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ModbusServ_UnpackInt1600
static int tolua_PAC_dev_ModbusServ_UnpackInt1600(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ModbusServ",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned char* buf = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
  int offset = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   short int tolua_ret = (short int)  ModbusServ::UnpackInt16(buf,offset);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnpackInt16'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_PAC_dev_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"i_DI_device","i_DI_device","",NULL);
  tolua_beginmodule(tolua_S,"i_DI_device");
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_DI_device_get_state00);
   tolua_function(tolua_S,"is_active",tolua_PAC_dev_i_DI_device_is_active00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_DO_device","i_DO_device","i_DI_device",NULL);
  tolua_beginmodule(tolua_S,"i_DO_device");
   tolua_function(tolua_S,"on",tolua_PAC_dev_i_DO_device_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_DO_device_off00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_i_DO_device_set_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_AI_device","i_AI_device","",NULL);
  tolua_beginmodule(tolua_S,"i_AI_device");
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_i_AI_device_get_value00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_AO_device","i_AO_device","i_AI_device",NULL);
  tolua_beginmodule(tolua_S,"i_AO_device");
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_AO_device_off00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_i_AO_device_set_value00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_DO_AO_device","i_DO_AO_device","i_DO_device",NULL);
  tolua_beginmodule(tolua_S,"i_DO_AO_device");
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_DO_AO_device_get_state00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_i_DO_AO_device_set_state00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_i_DO_AO_device_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_DO_AO_device_off00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_i_DO_AO_device_set_state01);
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_i_DO_AO_device_get_value00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_DO_AO_device_off01);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_i_DO_AO_device_set_value00);
   tolua_variable(tolua_S,"__i_AO_device__",tolua_get_i_DO_AO_device___i_AO_device__,NULL);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_counter","i_counter","",NULL);
  tolua_beginmodule(tolua_S,"i_counter");
   tolua_function(tolua_S,"pause",tolua_PAC_dev_i_counter_pause00);
   tolua_function(tolua_S,"start",tolua_PAC_dev_i_counter_start00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_i_counter_reset00);
   tolua_function(tolua_S,"restart",tolua_PAC_dev_i_counter_restart00);
   tolua_function(tolua_S,"get_quantity",tolua_PAC_dev_i_counter_get_quantity00);
   tolua_function(tolua_S,"get_flow",tolua_PAC_dev_i_counter_get_flow00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_counter_get_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"device","device","",NULL);
  tolua_beginmodule(tolua_S,"device");
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_device_set_cmd00);
   tolua_function(tolua_S,"set_par",tolua_PAC_dev_device_set_par00);
   tolua_function(tolua_S,"set_rt_par",tolua_PAC_dev_device_set_rt_par00);
   tolua_function(tolua_S,"set_property",tolua_PAC_dev_device_set_property00);
   tolua_function(tolua_S,"set_descr",tolua_PAC_dev_device_set_descr00);
   tolua_constant(tolua_S,"DT_NONE",device::DT_NONE);
   tolua_constant(tolua_S,"DT_V",device::DT_V);
   tolua_constant(tolua_S,"DT_VC",device::DT_VC);
   tolua_constant(tolua_S,"DT_M",device::DT_M);
   tolua_constant(tolua_S,"DT_LS",device::DT_LS);
   tolua_constant(tolua_S,"DT_TE",device::DT_TE);
   tolua_constant(tolua_S,"DT_FS",device::DT_FS);
   tolua_constant(tolua_S,"DT_GS",device::DT_GS);
   tolua_constant(tolua_S,"DT_FQT",device::DT_FQT);
   tolua_constant(tolua_S,"DT_LT",device::DT_LT);
   tolua_constant(tolua_S,"DT_QT",device::DT_QT);
   tolua_constant(tolua_S,"DT_HA",device::DT_HA);
   tolua_constant(tolua_S,"DT_HL",device::DT_HL);
   tolua_constant(tolua_S,"DT_SB",device::DT_SB);
   tolua_constant(tolua_S,"DT_DI",device::DT_DI);
   tolua_constant(tolua_S,"DT_DO",device::DT_DO);
   tolua_constant(tolua_S,"DT_AI",device::DT_AI);
   tolua_constant(tolua_S,"DT_AO",device::DT_AO);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"V",tolua_PAC_dev_V00);
  tolua_function(tolua_S,"VC",tolua_PAC_dev_VC00);
  tolua_function(tolua_S,"M",tolua_PAC_dev_M00);
  tolua_function(tolua_S,"LS",tolua_PAC_dev_LS00);
  tolua_function(tolua_S,"FS",tolua_PAC_dev_FS00);
  tolua_function(tolua_S,"AI",tolua_PAC_dev_AI00);
  tolua_function(tolua_S,"AO",tolua_PAC_dev_AO00);
  tolua_function(tolua_S,"FQT",tolua_PAC_dev_FQT00);
  tolua_function(tolua_S,"TE",tolua_PAC_dev_TE00);
  tolua_function(tolua_S,"LT",tolua_PAC_dev_LT00);
  tolua_function(tolua_S,"GS",tolua_PAC_dev_GS00);
  tolua_function(tolua_S,"HA",tolua_PAC_dev_HA00);
  tolua_function(tolua_S,"HL",tolua_PAC_dev_HL00);
  tolua_function(tolua_S,"SB",tolua_PAC_dev_SB00);
  tolua_function(tolua_S,"DI",tolua_PAC_dev_DI00);
  tolua_function(tolua_S,"DO",tolua_PAC_dev_DO00);
  tolua_function(tolua_S,"QT",tolua_PAC_dev_QT00);
  tolua_function(tolua_S,"STUB",tolua_PAC_dev_STUB00);
  tolua_function(tolua_S,"DEVICE",tolua_PAC_dev_DEVICE00);
  tolua_cclass(tolua_S,"dev_stub","dev_stub","",NULL);
  tolua_beginmodule(tolua_S,"dev_stub");
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_dev_stub_get_value00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_dev_stub_set_value00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_dev_stub_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_dev_stub_off00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_dev_stub_set_state00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_dev_stub_get_state00);
   tolua_function(tolua_S,"pause",tolua_PAC_dev_dev_stub_pause00);
   tolua_function(tolua_S,"start",tolua_PAC_dev_dev_stub_start00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_dev_stub_reset00);
   tolua_function(tolua_S,"get_quantity",tolua_PAC_dev_dev_stub_get_quantity00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"dev_errors_manager","dev_errors_manager","",NULL);
  tolua_beginmodule(tolua_S,"dev_errors_manager");
   tolua_function(tolua_S,"get_instance",tolua_PAC_dev_dev_errors_manager_get_instance00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_dev_errors_manager_set_cmd00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"valve","valve","",NULL);
  tolua_beginmodule(tolua_S,"valve");
   tolua_constant(tolua_S,"V_LOWER_SEAT",valve::V_LOWER_SEAT);
   tolua_constant(tolua_S,"V_UPPER_SEAT",valve::V_UPPER_SEAT);
   tolua_constant(tolua_S,"V_ON",valve::V_ON);
   tolua_constant(tolua_S,"V_OFF",valve::V_OFF);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"tech_object","tech_object","",tolua_collect_tech_object);
  #else
  tolua_cclass(tolua_S,"tech_object","tech_object","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"tech_object");
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_tech_object_set_cmd00);
   tolua_function(tolua_S,"new",tolua_PAC_dev_tech_object_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_tech_object_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_tech_object_new00_local);
   tolua_function(tolua_S,"delete",tolua_PAC_dev_tech_object_delete00);
   tolua_function(tolua_S,"set_mode",tolua_PAC_dev_tech_object_set_mode00);
   tolua_function(tolua_S,"get_mode",tolua_PAC_dev_tech_object_get_mode00);
   tolua_function(tolua_S,"exec_cmd",tolua_PAC_dev_tech_object_exec_cmd00);
   tolua_function(tolua_S,"get_modes_count",tolua_PAC_dev_tech_object_get_modes_count00);
   tolua_variable(tolua_S,"par_float",tolua_get_tech_object_par_float,tolua_set_tech_object_par_float);
   tolua_variable(tolua_S,"rt_par_float",tolua_get_tech_object_rt_par_float,tolua_set_tech_object_rt_par_float);
   tolua_variable(tolua_S,"par_uint",tolua_get_tech_object_par_uint,tolua_set_tech_object_par_uint);
   tolua_variable(tolua_S,"rt_par_uint",tolua_get_tech_object_rt_par_uint,tolua_set_tech_object_rt_par_uint);
   tolua_variable(tolua_S,"timers",tolua_get_tech_object_timers,tolua_set_tech_object_timers);
   tolua_function(tolua_S,"get_modes_manager",tolua_PAC_dev_tech_object_get_modes_manager00);
   tolua_function(tolua_S,"is_idle",tolua_PAC_dev_tech_object_is_idle00);
   tolua_function(tolua_S,"get_number",tolua_PAC_dev_tech_object_get_number00);
   tolua_constant(tolua_S,"ERR_CANT_ON",tech_object::ERR_CANT_ON);
   tolua_constant(tolua_S,"ERR_ON_WITH_ERRORS",tech_object::ERR_ON_WITH_ERRORS);
   tolua_constant(tolua_S,"ERR_OFF",tech_object::ERR_OFF);
   tolua_constant(tolua_S,"ERR_OFF_AND_ON",tech_object::ERR_OFF_AND_ON);
   tolua_constant(tolua_S,"ERR_DURING_WORK",tech_object::ERR_DURING_WORK);
   tolua_constant(tolua_S,"ERR_ALARM",tech_object::ERR_ALARM);
   tolua_function(tolua_S,"set_err_msg",tolua_PAC_dev_tech_object_set_err_msg00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_TECH_OBJECT_MNGR",tolua_PAC_dev_G_TECH_OBJECT_MNGR00);
  tolua_cclass(tolua_S,"tech_object_manager","tech_object_manager","",NULL);
  tolua_beginmodule(tolua_S,"tech_object_manager");
   tolua_function(tolua_S,"get_object_with_active_mode",tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00);
   tolua_function(tolua_S,"get_tech_objects",tolua_PAC_dev_tech_object_manager_get_tech_objects00);
   tolua_function(tolua_S,"print",tolua_PAC_dev_tech_object_manager_print00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"mode_manager","mode_manager","",NULL);
  tolua_beginmodule(tolua_S,"mode_manager");
   tolua_function(tolua_S,"add_mode",tolua_PAC_dev_mode_manager_add_mode00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_mode_manager__geti00);
   tolua_function(tolua_S,"get_idle_time",tolua_PAC_dev_mode_manager_get_idle_time00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"mode","mode","",NULL);
  tolua_beginmodule(tolua_S,"mode");
   tolua_function(tolua_S,"evaluation_time",tolua_PAC_dev_mode_evaluation_time00);
   tolua_function(tolua_S,"add_step",tolua_PAC_dev_mode_add_step00);
   tolua_function(tolua_S,"set_step_cooperate_time_par_n",tolua_PAC_dev_mode_set_step_cooperate_time_par_n00);
   tolua_function(tolua_S,"active_step",tolua_PAC_dev_mode_active_step00);
   tolua_function(tolua_S,"active_step_evaluation_time",tolua_PAC_dev_mode_active_step_evaluation_time00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_mode__geti00);
   tolua_function(tolua_S,"to_step",tolua_PAC_dev_mode_to_step00);
   tolua_function(tolua_S,"to_step",tolua_PAC_dev_mode_to_step01);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"step","step","",NULL);
  tolua_beginmodule(tolua_S,"step");
   tolua_function(tolua_S,".geti",tolua_PAC_dev_step__geti00);
   tolua_function(tolua_S,"get_name",tolua_PAC_dev_step_get_name00);
   tolua_constant(tolua_S,"A_ON",step::A_ON);
   tolua_constant(tolua_S,"A_OFF",step::A_OFF);
   tolua_constant(tolua_S,"A_UPPER_SEATS_ON",step::A_UPPER_SEATS_ON);
   tolua_constant(tolua_S,"A_LOWER_SEATS_ON",step::A_LOWER_SEATS_ON);
   tolua_constant(tolua_S,"A_REQUIRED_FB",step::A_REQUIRED_FB);
   tolua_constant(tolua_S,"A_DO_DI",step::A_DO_DI);
   tolua_constant(tolua_S,"A_WASH",step::A_WASH);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"action","action","",NULL);
  tolua_beginmodule(tolua_S,"action");
   tolua_function(tolua_S,"add_dev",tolua_PAC_dev_action_add_dev00);
   tolua_function(tolua_S,"add_dev",tolua_PAC_dev_action_add_dev01);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_DEVICE_MANAGER",tolua_PAC_dev_G_DEVICE_MANAGER00);
  tolua_function(tolua_S,"G_TECH_OBJECTS",tolua_PAC_dev_G_TECH_OBJECTS00);
  tolua_cclass(tolua_S,"device_manager","device_manager","",NULL);
  tolua_beginmodule(tolua_S,"device_manager");
   tolua_function(tolua_S,"print",tolua_PAC_dev_device_manager_print00);
   tolua_function(tolua_S,"add_wago_device",tolua_PAC_dev_device_manager_add_wago_device00);
   tolua_function(tolua_S,"get_device",tolua_PAC_dev_device_manager_get_device00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"wago_device","wago_device","",NULL);
  tolua_beginmodule(tolua_S,"wago_device");
   tolua_function(tolua_S,"init",tolua_PAC_dev_wago_device_init00);
   tolua_function(tolua_S,"init_channel",tolua_PAC_dev_wago_device_init_channel00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"timer_manager","timer_manager","",NULL);
  tolua_beginmodule(tolua_S,"timer_manager");
   tolua_function(tolua_S,".geti",tolua_PAC_dev_timer_manager__geti00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"timer","timer","",tolua_collect_timer);
  #else
  tolua_cclass(tolua_S,"timer","timer","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"timer");
   tolua_constant(tolua_S,"S_STOP",timer::S_STOP);
   tolua_constant(tolua_S,"S_WORK",timer::S_WORK);
   tolua_constant(tolua_S,"S_PAUSE",timer::S_PAUSE);
   tolua_function(tolua_S,"start",tolua_PAC_dev_timer_start00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_timer_reset00);
   tolua_function(tolua_S,"pause",tolua_PAC_dev_timer_pause00);
   tolua_function(tolua_S,"is_time_up",tolua_PAC_dev_timer_is_time_up00);
   tolua_function(tolua_S,"get_work_time",tolua_PAC_dev_timer_get_work_time00);
   tolua_function(tolua_S,"set_countdown_time",tolua_PAC_dev_timer_set_countdown_time00);
   tolua_function(tolua_S,"get_countdown_time",tolua_PAC_dev_timer_get_countdown_time00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_timer_get_state00);
   tolua_function(tolua_S,"new",tolua_PAC_dev_timer_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_timer_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_timer_new00_local);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"saved_params_float","saved_params_float","",NULL);
  tolua_beginmodule(tolua_S,"saved_params_float");
   tolua_function(tolua_S,"save",tolua_PAC_dev_saved_params_float_save00);
   tolua_function(tolua_S,"save_all",tolua_PAC_dev_saved_params_float_save_all00);
   tolua_function(tolua_S,".seti",tolua_PAC_dev_saved_params_float__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_saved_params_float__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_saved_params_float_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"saved_params_u_int_4","saved_params_u_int_4","",NULL);
  tolua_beginmodule(tolua_S,"saved_params_u_int_4");
   tolua_function(tolua_S,"save",tolua_PAC_dev_saved_params_u_int_4_save00);
   tolua_function(tolua_S,"save_all",tolua_PAC_dev_saved_params_u_int_4_save_all00);
   tolua_function(tolua_S,".seti",tolua_PAC_dev_saved_params_u_int_4__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_saved_params_u_int_4__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_saved_params_u_int_4_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"run_time_params_u_int_4","run_time_params_u_int_4","",NULL);
  tolua_beginmodule(tolua_S,"run_time_params_u_int_4");
   tolua_function(tolua_S,".seti",tolua_PAC_dev_run_time_params_u_int_4__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_run_time_params_u_int_4__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_run_time_params_u_int_4_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"run_time_params_float","run_time_params_float","",NULL);
  tolua_beginmodule(tolua_S,"run_time_params_float");
   tolua_function(tolua_S,".seti",tolua_PAC_dev_run_time_params_float__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_run_time_params_float__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_run_time_params_float_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"wago_manager","wago_manager","",NULL);
  tolua_beginmodule(tolua_S,"wago_manager");
   tolua_function(tolua_S,"init",tolua_PAC_dev_wago_manager_init00);
   tolua_function(tolua_S,"add_node",tolua_PAC_dev_wago_manager_add_node00);
   tolua_function(tolua_S,"init_node_AO",tolua_PAC_dev_wago_manager_init_node_AO00);
   tolua_function(tolua_S,"init_node_AI",tolua_PAC_dev_wago_manager_init_node_AI00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_WAGO_MANAGER",tolua_PAC_dev_G_WAGO_MANAGER00);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"PID","PID","",tolua_collect_PID);
  #else
  tolua_cclass(tolua_S,"PID","PID","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"PID");
   tolua_constant(tolua_S,"P_k",PID::P_k);
   tolua_constant(tolua_S,"P_Ti",PID::P_Ti);
   tolua_constant(tolua_S,"P_Td",PID::P_Td);
   tolua_constant(tolua_S,"P_dt",PID::P_dt);
   tolua_constant(tolua_S,"P_max",PID::P_max);
   tolua_constant(tolua_S,"P_min",PID::P_min);
   tolua_constant(tolua_S,"P_acceleration_time",PID::P_acceleration_time);
   tolua_constant(tolua_S,"P_is_manual_mode",PID::P_is_manual_mode);
   tolua_constant(tolua_S,"P_U_manual",PID::P_U_manual);
   tolua_constant(tolua_S,"P_k2",PID::P_k2);
   tolua_constant(tolua_S,"P_Ti2",PID::P_Ti2);
   tolua_constant(tolua_S,"P_Td2",PID::P_Td2);
   tolua_constant(tolua_S,"WP_Z",PID::WP_Z);
   tolua_constant(tolua_S,"WP_U",PID::WP_U);
   tolua_function(tolua_S,"new",tolua_PAC_dev_PID_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_PID_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_PID_new00_local);
   tolua_function(tolua_S,"on",tolua_PAC_dev_PID_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_PID_off00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_PID_reset00);
   tolua_function(tolua_S,"eval",tolua_PAC_dev_PID_eval00);
   tolua_function(tolua_S,"set",tolua_PAC_dev_PID_set00);
   tolua_function(tolua_S,"get_assignment",tolua_PAC_dev_PID_get_assignment00);
   tolua_function(tolua_S,"init_param",tolua_PAC_dev_PID_init_param00);
   tolua_function(tolua_S,"init_work_param",tolua_PAC_dev_PID_init_work_param00);
   tolua_function(tolua_S,"save_param",tolua_PAC_dev_PID_save_param00);
   tolua_function(tolua_S,"set_used_par",tolua_PAC_dev_PID_set_used_par00);
   tolua_function(tolua_S,"print",tolua_PAC_dev_PID_print00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_PID_set_cmd00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"PAC_info","PAC_info","i_Lua_save_device",NULL);
  tolua_beginmodule(tolua_S,"PAC_info");
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_PAC_info_set_cmd00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_PAC_INFO",tolua_PAC_dev_G_PAC_INFO00);
  tolua_function(tolua_S,"get_sec",tolua_PAC_dev_get_sec00);
  tolua_function(tolua_S,"get_millisec",tolua_PAC_dev_get_millisec00);
  tolua_function(tolua_S,"get_delta_millisec",tolua_PAC_dev_get_delta_millisec00);
  tolua_function(tolua_S,"sleep_ms",tolua_PAC_dev_sleep_ms00);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"cipline_tech_object","cipline_tech_object","tech_object",tolua_collect_cipline_tech_object);
  #else
  tolua_cclass(tolua_S,"cipline_tech_object","cipline_tech_object","tech_object",NULL);
  #endif
  tolua_beginmodule(tolua_S,"cipline_tech_object");
   tolua_function(tolua_S,"new",tolua_PAC_dev_cipline_tech_object_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_cipline_tech_object_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_cipline_tech_object_new00_local);
   tolua_variable(tolua_S,"blocked",tolua_get_cipline_tech_object_blocked,tolua_set_cipline_tech_object_blocked);
   tolua_variable(tolua_S,"opcip",tolua_get_cipline_tech_object_opcip,tolua_set_cipline_tech_object_opcip);
   tolua_variable(tolua_S,"curstep",tolua_get_cipline_tech_object_curstep,tolua_set_cipline_tech_object_curstep);
   tolua_variable(tolua_S,"state",tolua_get_cipline_tech_object_state,tolua_set_cipline_tech_object_state);
   tolua_variable(tolua_S,"curprg",tolua_get_cipline_tech_object_curprg,tolua_set_cipline_tech_object_curprg);
   tolua_variable(tolua_S,"nmr",tolua_get_cipline_tech_object_nmr,tolua_set_cipline_tech_object_nmr);
   tolua_variable(tolua_S,"cip_in_error",tolua_get_cipline_tech_object_cip_in_error,tolua_set_cipline_tech_object_cip_in_error);
   tolua_variable(tolua_S,"disable_tank_heating",tolua_get_cipline_tech_object_disable_tank_heating,tolua_set_cipline_tech_object_disable_tank_heating);
   tolua_variable(tolua_S,"V00",tolua_get_cipline_tech_object_V00_ptr,tolua_set_cipline_tech_object_V00_ptr);
   tolua_variable(tolua_S,"V01",tolua_get_cipline_tech_object_V01_ptr,tolua_set_cipline_tech_object_V01_ptr);
   tolua_variable(tolua_S,"V03",tolua_get_cipline_tech_object_V03_ptr,tolua_set_cipline_tech_object_V03_ptr);
   tolua_variable(tolua_S,"V02",tolua_get_cipline_tech_object_V02_ptr,tolua_set_cipline_tech_object_V02_ptr);
   tolua_variable(tolua_S,"V04",tolua_get_cipline_tech_object_V04_ptr,tolua_set_cipline_tech_object_V04_ptr);
   tolua_variable(tolua_S,"V05",tolua_get_cipline_tech_object_V05_ptr,tolua_set_cipline_tech_object_V05_ptr);
   tolua_variable(tolua_S,"V06",tolua_get_cipline_tech_object_V06_ptr,tolua_set_cipline_tech_object_V06_ptr);
   tolua_variable(tolua_S,"V07",tolua_get_cipline_tech_object_V07_ptr,tolua_set_cipline_tech_object_V07_ptr);
   tolua_variable(tolua_S,"V08",tolua_get_cipline_tech_object_V08_ptr,tolua_set_cipline_tech_object_V08_ptr);
   tolua_variable(tolua_S,"V09",tolua_get_cipline_tech_object_V09_ptr,tolua_set_cipline_tech_object_V09_ptr);
   tolua_variable(tolua_S,"V10",tolua_get_cipline_tech_object_V10_ptr,tolua_set_cipline_tech_object_V10_ptr);
   tolua_variable(tolua_S,"V11",tolua_get_cipline_tech_object_V11_ptr,tolua_set_cipline_tech_object_V11_ptr);
   tolua_variable(tolua_S,"V12",tolua_get_cipline_tech_object_V12_ptr,tolua_set_cipline_tech_object_V12_ptr);
   tolua_variable(tolua_S,"V13",tolua_get_cipline_tech_object_V13_ptr,tolua_set_cipline_tech_object_V13_ptr);
   tolua_variable(tolua_S,"NP",tolua_get_cipline_tech_object_NP_ptr,tolua_set_cipline_tech_object_NP_ptr);
   tolua_variable(tolua_S,"NK",tolua_get_cipline_tech_object_NK_ptr,tolua_set_cipline_tech_object_NK_ptr);
   tolua_variable(tolua_S,"NS",tolua_get_cipline_tech_object_NS_ptr,tolua_set_cipline_tech_object_NS_ptr);
   tolua_variable(tolua_S,"LL",tolua_get_cipline_tech_object_LL_ptr,tolua_set_cipline_tech_object_LL_ptr);
   tolua_variable(tolua_S,"LM",tolua_get_cipline_tech_object_LM_ptr,tolua_set_cipline_tech_object_LM_ptr);
   tolua_variable(tolua_S,"LH",tolua_get_cipline_tech_object_LH_ptr,tolua_set_cipline_tech_object_LH_ptr);
   tolua_variable(tolua_S,"LWL",tolua_get_cipline_tech_object_LWL_ptr,tolua_set_cipline_tech_object_LWL_ptr);
   tolua_variable(tolua_S,"LWH",tolua_get_cipline_tech_object_LWH_ptr,tolua_set_cipline_tech_object_LWH_ptr);
   tolua_variable(tolua_S,"LSL",tolua_get_cipline_tech_object_LSL_ptr,tolua_set_cipline_tech_object_LSL_ptr);
   tolua_variable(tolua_S,"LSH",tolua_get_cipline_tech_object_LSH_ptr,tolua_set_cipline_tech_object_LSH_ptr);
   tolua_variable(tolua_S,"LKL",tolua_get_cipline_tech_object_LKL_ptr,tolua_set_cipline_tech_object_LKL_ptr);
   tolua_variable(tolua_S,"LKH",tolua_get_cipline_tech_object_LKH_ptr,tolua_set_cipline_tech_object_LKH_ptr);
   tolua_variable(tolua_S,"TP",tolua_get_cipline_tech_object_TP_ptr,tolua_set_cipline_tech_object_TP_ptr);
   tolua_variable(tolua_S,"TR",tolua_get_cipline_tech_object_TR_ptr,tolua_set_cipline_tech_object_TR_ptr);
   tolua_variable(tolua_S,"Q",tolua_get_cipline_tech_object_Q_ptr,tolua_set_cipline_tech_object_Q_ptr);
   tolua_variable(tolua_S,"ao",tolua_get_cipline_tech_object_ao_ptr,tolua_set_cipline_tech_object_ao_ptr);
   tolua_variable(tolua_S,"PUMPFREQ",tolua_get_cipline_tech_object_PUMPFREQ_ptr,tolua_set_cipline_tech_object_PUMPFREQ_ptr);
   tolua_variable(tolua_S,"FL",tolua_get_cipline_tech_object_FL_ptr,tolua_set_cipline_tech_object_FL_ptr);
   tolua_variable(tolua_S,"cnt",tolua_get_cipline_tech_object_cnt_ptr,tolua_set_cipline_tech_object_cnt_ptr);
   tolua_variable(tolua_S,"msa_number",tolua_get_cipline_tech_object_msa_number,tolua_set_cipline_tech_object_msa_number);
   tolua_variable(tolua_S,"par_float",tolua_get_cipline_tech_object_par_float,tolua_set_cipline_tech_object_par_float);
   tolua_variable(tolua_S,"rt_par_float",tolua_get_cipline_tech_object_rt_par_float,tolua_set_cipline_tech_object_rt_par_float);
   tolua_function(tolua_S,"get_station_par",tolua_PAC_dev_cipline_tech_object_get_station_par00);
   tolua_function(tolua_S,"set_station_par",tolua_PAC_dev_cipline_tech_object_set_station_par00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_cipline_tech_object_set_cmd00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_cipline_tech_object_set_cmd01);
   tolua_function(tolua_S,"initline",tolua_PAC_dev_cipline_tech_object_initline00);
   tolua_function(tolua_S,"evaluate",tolua_PAC_dev_cipline_tech_object_evaluate00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_RM_MANAGER",tolua_PAC_dev_G_RM_MANAGER00);
  tolua_cclass(tolua_S,"rm_manager","rm_manager","",NULL);
  tolua_beginmodule(tolua_S,"rm_manager");
   tolua_function(tolua_S,"add_rm_cmmctr",tolua_PAC_dev_rm_manager_add_rm_cmmctr00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"modbus_client","modbus_client","",tolua_collect_modbus_client);
  #else
  tolua_cclass(tolua_S,"modbus_client","modbus_client","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"modbus_client");
   tolua_function(tolua_S,"new",tolua_PAC_dev_modbus_client_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_modbus_client_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_modbus_client_new00_local);
   tolua_function(tolua_S,"read_discrete_inputs",tolua_PAC_dev_modbus_client_read_discrete_inputs00);
   tolua_function(tolua_S,"read_coils",tolua_PAC_dev_modbus_client_read_coils00);
   tolua_function(tolua_S,"read_holding_registers",tolua_PAC_dev_modbus_client_read_holding_registers00);
   tolua_function(tolua_S,"read_input_registers",tolua_PAC_dev_modbus_client_read_input_registers00);
   tolua_function(tolua_S,"write_coil",tolua_PAC_dev_modbus_client_write_coil00);
   tolua_function(tolua_S,"force_multiply_coils",tolua_PAC_dev_modbus_client_force_multiply_coils00);
   tolua_function(tolua_S,"write_multiply_registers",tolua_PAC_dev_modbus_client_write_multiply_registers00);
   tolua_function(tolua_S,"zero_output_buff",tolua_PAC_dev_modbus_client_zero_output_buff00);
   tolua_function(tolua_S,"set_int2",tolua_PAC_dev_modbus_client_set_int200);
   tolua_function(tolua_S,"get_int2",tolua_PAC_dev_modbus_client_get_int200);
   tolua_function(tolua_S,"set_int4",tolua_PAC_dev_modbus_client_set_int400);
   tolua_function(tolua_S,"get_int4",tolua_PAC_dev_modbus_client_get_int400);
   tolua_function(tolua_S,"set_float",tolua_PAC_dev_modbus_client_set_float00);
   tolua_function(tolua_S,"get_float",tolua_PAC_dev_modbus_client_get_float00);
   tolua_function(tolua_S,"set_bit",tolua_PAC_dev_modbus_client_set_bit00);
   tolua_function(tolua_S,"get_bit",tolua_PAC_dev_modbus_client_get_bit00);
   tolua_function(tolua_S,"get_id",tolua_PAC_dev_modbus_client_get_id00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"ModbusServ","ModbusServ","",NULL);
  tolua_beginmodule(tolua_S,"ModbusServ");
   tolua_function(tolua_S,"UnpackInt16",tolua_PAC_dev_ModbusServ_UnpackInt1600);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_PAC_dev (lua_State* tolua_S) {
 return tolua_PAC_dev_open(tolua_S);
};
#endif

