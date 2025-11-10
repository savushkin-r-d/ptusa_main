/*
** Lua binding: network_settings
** Generated for network settings manager
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int tolua_network_settings_open(lua_State* tolua_S);

#include "network_settings.h"

#ifdef WIN_OS
#pragma warning(disable: 4800) // Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#endif // WIN_OS

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "network_settings_manager");
    tolua_usertype(tolua_S, "port_forward_rule");
}

/* method: add_port_forward_rule of class network_settings_manager */
#ifndef TOLUA_DISABLE_tolua_network_settings_network_settings_manager_add_port_forward_rule00
static int tolua_network_settings_network_settings_manager_add_port_forward_rule00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "network_settings_manager", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isstring(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
        !tolua_isstring(tolua_S, 6, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 7, &tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        network_settings_manager* self = (network_settings_manager*)tolua_tousertype(tolua_S, 1, 0);
        unsigned int command_id = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
        const char* device_name = ((const char*)tolua_tostring(tolua_S, 3, 0));
        unsigned int internal_port = ((unsigned int)tolua_tonumber(tolua_S, 4, 0));
        unsigned int external_port = ((unsigned int)tolua_tonumber(tolua_S, 5, 0));
        const char* device_ip = ((const char*)tolua_tostring(tolua_S, 6, 0));
#ifndef TOLUA_RELEASE
        if (!self) tolua_error(tolua_S, "invalid 'self' in function 'add_port_forward_rule'", NULL);
#endif
        {
            self->add_port_forward_rule(command_id, device_name, internal_port, external_port, device_ip);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'add_port_forward_rule'.", &tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: process_command of class network_settings_manager */
#ifndef TOLUA_DISABLE_tolua_network_settings_network_settings_manager_process_command00
static int tolua_network_settings_network_settings_manager_process_command00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "network_settings_manager", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isboolean(tolua_S, 3, 1, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        network_settings_manager* self = (network_settings_manager*)tolua_tousertype(tolua_S, 1, 0);
        unsigned int command_id = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
        bool open = ((bool)tolua_toboolean(tolua_S, 3, true));
#ifndef TOLUA_RELEASE
        if (!self) tolua_error(tolua_S, "invalid 'self' in function 'process_command'", NULL);
#endif
        {
            bool tolua_ret = (bool)self->process_command(command_id, open);
            tolua_pushboolean(tolua_S, (bool)tolua_ret);
        }
    }
    return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'process_command'.", &tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: close_all_ports of class network_settings_manager */
#ifndef TOLUA_DISABLE_tolua_network_settings_network_settings_manager_close_all_ports00
static int tolua_network_settings_network_settings_manager_close_all_ports00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "network_settings_manager", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        network_settings_manager* self = (network_settings_manager*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
        if (!self) tolua_error(tolua_S, "invalid 'self' in function 'close_all_ports'", NULL);
#endif
        {
            self->close_all_ports();
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'close_all_ports'.", &tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_NETWORK_SETTINGS_MANAGER */
#ifndef TOLUA_DISABLE_tolua_network_settings_G_NETWORK_SETTINGS_MANAGER00
static int tolua_network_settings_G_NETWORK_SETTINGS_MANAGER00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isnoobj(tolua_S, 1, &tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        {
            network_settings_manager* tolua_ret = (network_settings_manager*)G_NETWORK_SETTINGS_MANAGER();
            tolua_pushusertype(tolua_S, (void*)tolua_ret, "network_settings_manager");
        }
    }
    return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'G_NETWORK_SETTINGS_MANAGER'.", &tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_network_settings_open(lua_State* tolua_S)
{
    tolua_open(tolua_S);
    tolua_reg_types(tolua_S);
    tolua_module(tolua_S, NULL, 0);
    tolua_beginmodule(tolua_S, NULL);
    tolua_cclass(tolua_S, "network_settings_manager", "network_settings_manager", "", NULL);
    tolua_beginmodule(tolua_S, "network_settings_manager");
    tolua_function(tolua_S, "add_port_forward_rule", tolua_network_settings_network_settings_manager_add_port_forward_rule00);
    tolua_function(tolua_S, "process_command", tolua_network_settings_network_settings_manager_process_command00);
    tolua_function(tolua_S, "close_all_ports", tolua_network_settings_network_settings_manager_close_all_ports00);
    tolua_endmodule(tolua_S);
    tolua_function(tolua_S, "G_NETWORK_SETTINGS_MANAGER", tolua_network_settings_G_NETWORK_SETTINGS_MANAGER00);
    tolua_endmodule(tolua_S);
    return 1;
}

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
TOLUA_API int luaopen_network_settings(lua_State* tolua_S) {
    return tolua_network_settings_open(tolua_S);
};
#endif
