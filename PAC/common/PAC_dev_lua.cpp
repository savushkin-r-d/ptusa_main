/*
** Lua binding: PAC_dev
** Generated automatically by tolua++-1.0.92 on 09/10/10 17:43:44.
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
#include "tolua_base.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_run_time_params_float (lua_State* tolua_S)
{
 run_time_params_float* self = (run_time_params_float*) tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}

static int tolua_collect_i_DO_device (lua_State* tolua_S)
{
 i_DO_device* self = (i_DO_device*) tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}

static int tolua_collect_saved_params_u_int_4 (lua_State* tolua_S)
{
 saved_params_u_int_4* self = (saved_params_u_int_4*) tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}

static int tolua_collect_saved_params_float (lua_State* tolua_S)
{
 saved_params_float* self = (saved_params_float*) tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}

static int tolua_collect_run_time_params_u_int_4 (lua_State* tolua_S)
{
 run_time_params_u_int_4* self = (run_time_params_u_int_4*) tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}

static int tolua_collect_i_DI_device (lua_State* tolua_S)
{
 i_DI_device* self = (i_DI_device*) tolua_tousertype(tolua_S,1,0);
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
 tolua_usertype(tolua_S,"i_DI_device");
 tolua_usertype(tolua_S,"saved_params_u_int_4");
 tolua_usertype(tolua_S,"saved_params_float");
 tolua_usertype(tolua_S,"i_counter");
 tolua_usertype(tolua_S,"i_AO_device");
 tolua_usertype(tolua_S,"run_time_params_float");
 tolua_usertype(tolua_S,"i_DO_device");
 tolua_usertype(tolua_S,"i_AI_device");
 tolua_usertype(tolua_S,"run_time_params_u_int_4");
 tolua_usertype(tolua_S,"tech_object_manager");
 tolua_usertype(tolua_S,"tech_object");
}

/* method: delete of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_delete00
static int tolua_PAC_dev_i_DI_device_delete00(lua_State* tolua_S)
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

/* method: get_state_now of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_get_state_now00
static int tolua_PAC_dev_i_DI_device_get_state_now00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state_now'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state_now();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state_now'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

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

/* method: set_dt of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_set_dt00
static int tolua_PAC_dev_i_DI_device_set_dt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DI_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DI_device* self = (i_DI_device*)  tolua_tousertype(tolua_S,1,0);
  unsigned int time = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_dt'", NULL);
#endif
  {
   self->set_dt(time);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_dt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_st_state of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_set_st_state00
static int tolua_PAC_dev_i_DI_device_set_st_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DI_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DI_device* self = (i_DI_device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_st_state'", NULL);
#endif
  {
   self->set_st_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_st_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_delete00
static int tolua_PAC_dev_i_DO_device_delete00(lua_State* tolua_S)
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
   int tolua_ret = (int)  self->set_state(new_state);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  V(number);
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

/* function: N */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_N00
static int tolua_PAC_dev_N00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  N(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'N'.",&tolua_err);
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  M(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  LS(number);
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  FS(number);
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  AI(number);
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_AO_device* tolua_ret = (i_AO_device*)  AO(number);
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

/* function: CTR */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_CTR00
static int tolua_PAC_dev_CTR00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_counter* tolua_ret = (i_counter*)  CTR(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_counter");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CTR'.",&tolua_err);
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
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  TE(number);
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

/* function: FE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FE00
static int tolua_PAC_dev_FE00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  FE(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FE'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_LE00
static int tolua_PAC_dev_LE00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  LE(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LE'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: FB */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FB00
static int tolua_PAC_dev_FB00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  FB(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FB'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: UPR */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_UPR00
static int tolua_PAC_dev_UPR00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  UPR(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UPR'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: QE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_QE00
static int tolua_PAC_dev_QE00(lua_State* tolua_S)
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
  int number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  QE(number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'QE'.",&tolua_err);
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
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,10,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,"tech_object"));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,4,1));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,5,3));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,6,5));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,7,5));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,8,5));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,9,5));
  {
   tech_object* tolua_ret = (tech_object*)  Mtolua_new((tech_object)(name,number,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
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
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,10,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,"tech_object"));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,4,1));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,5,3));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,6,5));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,7,5));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,8,5));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,9,5));
  {
   tech_object* tolua_ret = (tech_object*)  Mtolua_new((tech_object)(
       name,number,states_count,timers_count,par_float_count,
       runtime_par_float_count,par_uint_count,runtime_par_uint_count));
   
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

/* method: check_on_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_check_on_mode00
static int tolua_PAC_dev_tech_object_check_on_mode00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'check_on_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->check_on_mode(mode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'check_on_mode'.",&tolua_err);
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

/* method: init_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_init_mode00
static int tolua_PAC_dev_tech_object_init_mode00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_mode'", NULL);
#endif
  {
   self->init_mode(mode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluate of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_evaluate00
static int tolua_PAC_dev_tech_object_evaluate00(lua_State* tolua_S)
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

/* method: check_off_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_check_off_mode00
static int tolua_PAC_dev_tech_object_check_off_mode00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'check_off_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->check_off_mode(mode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'check_off_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: final_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_final_mode00
static int tolua_PAC_dev_tech_object_final_mode00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'final_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->final_mode(mode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'final_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_params of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_init_params00
static int tolua_PAC_dev_tech_object_init_params00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_params'", NULL);
#endif
  {
   int tolua_ret = (int)  self->init_params();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_params'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_runtime_params of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_init_runtime_params00
static int tolua_PAC_dev_tech_object_init_runtime_params00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_runtime_params'", NULL);
#endif
  {
   int tolua_ret = (int)  self->init_runtime_params();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_runtime_params'.",&tolua_err);
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

/* method: get_count of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_get_count00
static int tolua_PAC_dev_tech_object_manager_get_count00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object_manager* self = (const tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_count'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_count();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_count'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_new00
static int tolua_PAC_dev_saved_params_float_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   saved_params_float* tolua_ret = (saved_params_float*)  Mtolua_new((saved_params_float)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"saved_params_float");
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

/* method: new_local of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_new00_local
static int tolua_PAC_dev_saved_params_float_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   saved_params_float* tolua_ret = (saved_params_float*)  Mtolua_new((saved_params_float)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"saved_params_float");
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

/* method: delete of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_delete00
static int tolua_PAC_dev_saved_params_float_delete00(lua_State* tolua_S)
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

/* method: get_val of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_get_val00
static int tolua_PAC_dev_saved_params_float_get_val00(lua_State* tolua_S)
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
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_val'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_val(idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_val'.",&tolua_err);
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

/* method: new of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_new00
static int tolua_PAC_dev_saved_params_u_int_4_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   saved_params_u_int_4* tolua_ret = (saved_params_u_int_4*)  Mtolua_new((saved_params_u_int_4)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"saved_params_u_int_4");
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

/* method: new_local of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_new00_local
static int tolua_PAC_dev_saved_params_u_int_4_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   saved_params_u_int_4* tolua_ret = (saved_params_u_int_4*)  Mtolua_new((saved_params_u_int_4)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"saved_params_u_int_4");
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

/* method: delete of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_delete00
static int tolua_PAC_dev_saved_params_u_int_4_delete00(lua_State* tolua_S)
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

/* method: get_val of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_get_val00
static int tolua_PAC_dev_saved_params_u_int_4_get_val00(lua_State* tolua_S)
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
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_val'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_val(idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_val'.",&tolua_err);
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

/* method: new of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_new00
static int tolua_PAC_dev_run_time_params_u_int_4_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   run_time_params_u_int_4* tolua_ret = (run_time_params_u_int_4*)  Mtolua_new((run_time_params_u_int_4)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"run_time_params_u_int_4");
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

/* method: new_local of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_new00_local
static int tolua_PAC_dev_run_time_params_u_int_4_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   run_time_params_u_int_4* tolua_ret = (run_time_params_u_int_4*)  Mtolua_new((run_time_params_u_int_4)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"run_time_params_u_int_4");
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

/* method: delete of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_delete00
static int tolua_PAC_dev_run_time_params_u_int_4_delete00(lua_State* tolua_S)
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

/* method: get_val of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_get_val00
static int tolua_PAC_dev_run_time_params_u_int_4_get_val00(lua_State* tolua_S)
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
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_val'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_val(idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_val'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_save00
static int tolua_PAC_dev_run_time_params_u_int_4_save00(lua_State* tolua_S)
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

/* method: new of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_new00
static int tolua_PAC_dev_run_time_params_float_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   run_time_params_float* tolua_ret = (run_time_params_float*)  Mtolua_new((run_time_params_float)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"run_time_params_float");
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

/* method: new_local of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_new00_local
static int tolua_PAC_dev_run_time_params_float_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int count = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   run_time_params_float* tolua_ret = (run_time_params_float*)  Mtolua_new((run_time_params_float)(count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"run_time_params_float");
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

/* method: delete of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_delete00
static int tolua_PAC_dev_run_time_params_float_delete00(lua_State* tolua_S)
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

/* method: get_val of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_get_val00
static int tolua_PAC_dev_run_time_params_float_get_val00(lua_State* tolua_S)
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
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_val'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_val(idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_val'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_save00
static int tolua_PAC_dev_run_time_params_float_save00(lua_State* tolua_S)
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

/* Open function */
TOLUA_API int tolua_PAC_dev_open (lua_State* tolua_S)
{
tolua_open(tolua_S);
tolua_reg_types(tolua_S);
tolua_module(tolua_S,NULL,0);
tolua_beginmodule(tolua_S,NULL);
#ifdef __cplusplus
tolua_cclass(tolua_S,"i_DI_device","i_DI_device","",tolua_collect_i_DI_device);
#else
tolua_cclass(tolua_S,"i_DI_device","i_DI_device","",NULL);
#endif
tolua_beginmodule(tolua_S,"i_DI_device");
tolua_function(tolua_S,"delete",tolua_PAC_dev_i_DI_device_delete00);
tolua_function(tolua_S,"get_state_now",tolua_PAC_dev_i_DI_device_get_state_now00);
tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_DI_device_get_state00);
tolua_function(tolua_S,"set_dt",tolua_PAC_dev_i_DI_device_set_dt00);
tolua_function(tolua_S,"set_st_state",tolua_PAC_dev_i_DI_device_set_st_state00);
tolua_endmodule(tolua_S);
#ifdef __cplusplus
tolua_cclass(tolua_S,"i_DO_device","i_DO_device","i_DI_device",tolua_collect_i_DO_device);
#else
tolua_cclass(tolua_S,"i_DO_device","i_DO_device","i_DI_device",NULL);
#endif
tolua_beginmodule(tolua_S,"i_DO_device");
tolua_function(tolua_S,"delete",tolua_PAC_dev_i_DO_device_delete00);
tolua_function(tolua_S,"on",tolua_PAC_dev_i_DO_device_on00);
tolua_function(tolua_S,"off",tolua_PAC_dev_i_DO_device_off00);
tolua_function(tolua_S,"set_state",tolua_PAC_dev_i_DO_device_set_state00);
tolua_endmodule(tolua_S);
tolua_function(tolua_S,"V",tolua_PAC_dev_V00);
tolua_function(tolua_S,"N",tolua_PAC_dev_N00);
tolua_function(tolua_S,"M",tolua_PAC_dev_M00);
tolua_function(tolua_S,"LS",tolua_PAC_dev_LS00);
tolua_function(tolua_S,"FS",tolua_PAC_dev_FS00);
tolua_function(tolua_S,"AI",tolua_PAC_dev_AI00);
tolua_function(tolua_S,"AO",tolua_PAC_dev_AO00);
tolua_function(tolua_S,"CTR",tolua_PAC_dev_CTR00);
tolua_function(tolua_S,"TE",tolua_PAC_dev_TE00);
tolua_function(tolua_S,"FE",tolua_PAC_dev_FE00);
tolua_function(tolua_S,"LE",tolua_PAC_dev_LE00);
tolua_function(tolua_S,"FB",tolua_PAC_dev_FB00);
tolua_function(tolua_S,"UPR",tolua_PAC_dev_UPR00);
tolua_function(tolua_S,"QE",tolua_PAC_dev_QE00);
#ifdef __cplusplus
tolua_cclass(tolua_S,"tech_object","tech_object","",tolua_collect_tech_object);
#else
tolua_cclass(tolua_S,"tech_object","tech_object","",NULL);
#endif
tolua_beginmodule(tolua_S,"tech_object");
tolua_function(tolua_S,"new",tolua_PAC_dev_tech_object_new00);
tolua_function(tolua_S,"new_local",tolua_PAC_dev_tech_object_new00_local);
tolua_function(tolua_S,".call",tolua_PAC_dev_tech_object_new00_local);
tolua_function(tolua_S,"delete",tolua_PAC_dev_tech_object_delete00);
tolua_function(tolua_S,"set_mode",tolua_PAC_dev_tech_object_set_mode00);
tolua_function(tolua_S,"get_mode",tolua_PAC_dev_tech_object_get_mode00);
tolua_function(tolua_S,"check_on_mode",tolua_PAC_dev_tech_object_check_on_mode00);
tolua_function(tolua_S,"exec_cmd",tolua_PAC_dev_tech_object_exec_cmd00);
tolua_function(tolua_S,"init_mode",tolua_PAC_dev_tech_object_init_mode00);
tolua_function(tolua_S,"evaluate",tolua_PAC_dev_tech_object_evaluate00);
tolua_function(tolua_S,"check_off_mode",tolua_PAC_dev_tech_object_check_off_mode00);
tolua_function(tolua_S,"final_mode",tolua_PAC_dev_tech_object_final_mode00);
tolua_function(tolua_S,"init_params",tolua_PAC_dev_tech_object_init_params00);
tolua_function(tolua_S,"init_runtime_params",tolua_PAC_dev_tech_object_init_runtime_params00);
tolua_function(tolua_S,"get_number",tolua_PAC_dev_tech_object_get_number00);
tolua_function(tolua_S,"get_modes_count",tolua_PAC_dev_tech_object_get_modes_count00);
tolua_variable(tolua_S,"par_float",tolua_get_tech_object_par_float,tolua_set_tech_object_par_float);
tolua_variable(tolua_S,"rt_par_float",tolua_get_tech_object_rt_par_float,tolua_set_tech_object_rt_par_float);
tolua_variable(tolua_S,"par_uint",tolua_get_tech_object_par_uint,tolua_set_tech_object_par_uint);
tolua_variable(tolua_S,"rt_par_uint",tolua_get_tech_object_rt_par_uint,tolua_set_tech_object_rt_par_uint);
tolua_endmodule(tolua_S);
tolua_function(tolua_S,"G_TECH_OBJECT_MNGR",tolua_PAC_dev_G_TECH_OBJECT_MNGR00);
tolua_function(tolua_S,"G_TECH_OBJECTS",tolua_PAC_dev_G_TECH_OBJECTS00);
tolua_cclass(tolua_S,"tech_object_manager","tech_object_manager","",NULL);
tolua_beginmodule(tolua_S,"tech_object_manager");
tolua_function(tolua_S,"get_object_with_active_mode",tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00);
tolua_function(tolua_S,"get_tech_objects",tolua_PAC_dev_tech_object_manager_get_tech_objects00);
tolua_function(tolua_S,"get_count",tolua_PAC_dev_tech_object_manager_get_count00);
tolua_endmodule(tolua_S);
#ifdef __cplusplus
tolua_cclass(tolua_S,"saved_params_float","saved_params_float","",tolua_collect_saved_params_float);
#else
tolua_cclass(tolua_S,"saved_params_float","saved_params_float","",NULL);
#endif
tolua_beginmodule(tolua_S,"saved_params_float");
tolua_function(tolua_S,"new",tolua_PAC_dev_saved_params_float_new00);
tolua_function(tolua_S,"new_local",tolua_PAC_dev_saved_params_float_new00_local);
tolua_function(tolua_S,".call",tolua_PAC_dev_saved_params_float_new00_local);
tolua_function(tolua_S,"delete",tolua_PAC_dev_saved_params_float_delete00);
tolua_function(tolua_S,"get_val",tolua_PAC_dev_saved_params_float_get_val00);
tolua_function(tolua_S,"save",tolua_PAC_dev_saved_params_float_save00);
tolua_function(tolua_S,"save_all",tolua_PAC_dev_saved_params_float_save_all00);
tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_saved_params_float_reset_to_000);
tolua_endmodule(tolua_S);
#ifdef __cplusplus
tolua_cclass(tolua_S,"saved_params_u_int_4","saved_params_u_int_4","",tolua_collect_saved_params_u_int_4);
#else
tolua_cclass(tolua_S,"saved_params_u_int_4","saved_params_u_int_4","",NULL);
#endif
tolua_beginmodule(tolua_S,"saved_params_u_int_4");
tolua_function(tolua_S,"new",tolua_PAC_dev_saved_params_u_int_4_new00);
tolua_function(tolua_S,"new_local",tolua_PAC_dev_saved_params_u_int_4_new00_local);
tolua_function(tolua_S,".call",tolua_PAC_dev_saved_params_u_int_4_new00_local);
tolua_function(tolua_S,"delete",tolua_PAC_dev_saved_params_u_int_4_delete00);
tolua_function(tolua_S,"get_val",tolua_PAC_dev_saved_params_u_int_4_get_val00);
tolua_function(tolua_S,"save",tolua_PAC_dev_saved_params_u_int_4_save00);
tolua_function(tolua_S,"save_all",tolua_PAC_dev_saved_params_u_int_4_save_all00);
tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_saved_params_u_int_4_reset_to_000);
tolua_endmodule(tolua_S);
#ifdef __cplusplus
tolua_cclass(tolua_S,"run_time_params_u_int_4","run_time_params_u_int_4","",tolua_collect_run_time_params_u_int_4);
#else
tolua_cclass(tolua_S,"run_time_params_u_int_4","run_time_params_u_int_4","",NULL);
#endif
tolua_beginmodule(tolua_S,"run_time_params_u_int_4");
tolua_function(tolua_S,"new",tolua_PAC_dev_run_time_params_u_int_4_new00);
tolua_function(tolua_S,"new_local",tolua_PAC_dev_run_time_params_u_int_4_new00_local);
tolua_function(tolua_S,".call",tolua_PAC_dev_run_time_params_u_int_4_new00_local);
tolua_function(tolua_S,"delete",tolua_PAC_dev_run_time_params_u_int_4_delete00);
tolua_function(tolua_S,"get_val",tolua_PAC_dev_run_time_params_u_int_4_get_val00);
tolua_function(tolua_S,"save",tolua_PAC_dev_run_time_params_u_int_4_save00);
tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_run_time_params_u_int_4_reset_to_000);
tolua_endmodule(tolua_S);
#ifdef __cplusplus
tolua_cclass(tolua_S,"run_time_params_float","run_time_params_float","",tolua_collect_run_time_params_float);
#else
tolua_cclass(tolua_S,"run_time_params_float","run_time_params_float","",NULL);
#endif
tolua_beginmodule(tolua_S,"run_time_params_float");
tolua_function(tolua_S,"new",tolua_PAC_dev_run_time_params_float_new00);
tolua_function(tolua_S,"new_local",tolua_PAC_dev_run_time_params_float_new00_local);
tolua_function(tolua_S,".call",tolua_PAC_dev_run_time_params_float_new00_local);
tolua_function(tolua_S,"delete",tolua_PAC_dev_run_time_params_float_delete00);
tolua_function(tolua_S,"get_val",tolua_PAC_dev_run_time_params_float_get_val00);
tolua_function(tolua_S,"save",tolua_PAC_dev_run_time_params_float_save00);
tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_run_time_params_float_reset_to_000);
tolua_endmodule(tolua_S);
tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_PAC_dev (lua_State* tolua_S) {
 return tolua_PAC_dev_open(tolua_S);
};
#endif
