#include "smart_ptr.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include    "lua.h"
#include    "lauxlib.h"
#include    "lualib.h"

#ifdef  __cplusplus
    };
#endif

#include    "tolua++.h"
//-----------------------------------------------------------------------------
TOLUA_API int tolua_PAC_dev_open ( lua_State* tolua_S );

TOLUA_API int tolua_IOT_dev_open(lua_State* tolua_S);

#ifdef RFID
TOLUA_API int tolua_rfid_reader_open( lua_State* tolua_S );
#endif

#ifdef OPCUA
TOLUA_API int tolua_OPC_UA_open(lua_State* tolua_S);
#endif
//-----------------------------------------------------------------------------
#if !defined RM_PAC
const int SYS_FILE_CNT = 3;
const int FILE_CNT     = 8;
#else
const int SYS_FILE_CNT = 4;
const int FILE_CNT     = 10;
#endif // RM_PAC
//-----------------------------------------------------------------------------
const int FILES_VERSION[ FILE_CNT ] =
    {
    1, //"sys.io.lua",
    1, //"sys.devices.lua",
    7, //"sys.objects.plua",
#if defined RM_PAC
    1, //"sys.rm_PACS.lua",
#endif // defined RM_PAC

    1, //"main.io.plua",
    1, //"main.objects.plua",
    1, //"main.modbus_srv.lua",
    1, //"main.profibus.lua",
    1, //"main.restrictions.lua"
#if defined RM_PAC
    1, //"main.rm_PACS.lua"
#endif // defined RM_PAC
    };
//-----------------------------------------------------------------------------
int check_file(const char* file_name, char* err_str);
//-----------------------------------------------------------------------------
class lua_manager
    {
    public:
        void no_print_stack_traceback()
            {
            is_print_stack_traceback = false;
            }

        static lua_manager* get_instance();

        int init( lua_State* L, const char* script_name, const char* dir = "", const char* sys_dir = "" );

        ~lua_manager();

        int void_exec_lua_method( const char *object_name,
            const char *function_name, const char *c_function_name ) const;

        const char* char_no_param_exec_lua_method( const char *object_name,
            const char *function_name, const char *c_function_name ) const;

        const char* char_exec_lua_method( const char *object_name,
            const char *function_name, int param, const char *c_function_name ) const;

        int int_exec_lua_method( const char *object_name,
            const char *function_name, int param,
            const char *c_function_name ) const;

        int int_2_exec_lua_method( const char* object_name,
            const char* function_name, int param1, int param2,
            const char* c_function_name ) const;

        int int_no_param_exec_lua_method( const char *object_name,
            const char *function_name, const char *c_function_name ) const;

        void* user_object_exec_lua_method( const char *object_name,
            const char *function_name, int param,
            const char *c_function_name ) const;

        int exec_Lua_str( const char *Lua_str, const char *error_str,
            bool is_print_error_msg = true ) const;

        lua_State * get_Lua() const;

        int reload_script( int script_n, const char* script_function_name,
            char *res_str, int max_res_str_length );

    private:
        lua_manager() : L( 0 ), err_func( 0 ), is_free_lua( 0 )
            {
            }

        static int error_trace( lua_State * L );

        static auto_smart_ptr< lua_manager > instance;

        int exec_lua_method( const char *object_name,
            const char *function_name, int param, int is_use_param = 1,
            int is_use_lua_return_value = 1 ) const;

        int exec_lua_method_var( const char* object_name, const char* function_name,
            int is_use_lua_return_value = 0, int cnt = 0, ... ) const;

        static bool is_print_stack_traceback;

        int err_func;
        lua_State * L;

        int is_free_lua;

        enum CONSTANTS
            {
            MAX_ERRORS = 50,
            };
    };
//-----------------------------------------------------------------------------
#define G_LUA_MANAGER lua_manager::get_instance()
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
