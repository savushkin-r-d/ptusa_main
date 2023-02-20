#include "lua_manager_tests.h"

using namespace ::testing;

/*
	TEST METHOD DEFENITION:
	void no_print_stack_traceback()
*/

TEST(lua_manager_test, no_print_stack_traceback)
{
    lua_manager::no_print_stack_traceback();
}

/*
    TEST METHOD DEFENITION:
	static lua_manager* get_instance();
*/

TEST(lua_manager_test, get_instance)
{
	lua_manager* p = 0;

	EXPECT_NE(p, lua_manager::get_instance());
}

/*
	TEST METHOD DEFENITION:
	int init(lua_State* L, const char* script_name, const char* dir = "", const char* sys_dir = "");

	HOOKED:
	LUALIB_API lua_State *(luaL_newstate) (void)
	LUA_API int lua_gc (lua_State *L, int what, int data)
	LUALIB_API void luaL_openlibs (lua_State *L)
	LUALIB_API int (luaL_loadstring) (lua_State *L, const char *s)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUALIB_API int luaL_loadfile (lua_State *L, const char *filename)
	TOLUA_API int tolua_PAC_dev_open (lua_State* tolua_S)
	TOLUA_API int tolua_IOT_dev_open (lua_State* tolua_S)
    LUA_API void lua_close (lua_State *L)
    LUA_API const char *lua_tolstring (lua_State *L, int idx, size_t *len)
    LUA_API void lua_settop (lua_State *L, int idx)
*/

TEST_F(LuaManagerTest, init_success)
{
    char* res = 0;
	mock_project_manager* prj_mock = new mock_project_manager();
	mock_params_manager* par_mock = new mock_params_manager();
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, get_params_data(_, _))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(res));

    mock_tech_object_manager* tech_mock = new mock_tech_object_manager(); //require par_mock
    mock_tcp_communicator* tcp_mock = new mock_tcp_communicator();
	test_project_manager::replaceEntity(prj_mock);
	mock_tech_object_manager::set_instance(tech_mock);
    test_tcp_communicator::replaceEntity(tcp_mock);

	EXPECT_CALL(*par_mock, init(_));

	EXPECT_CALL(*prj_mock, lua_load_configuration())
		.WillOnce(Return(0));

	EXPECT_CALL(*tech_mock, init_objects())
		.WillOnce(Return(0));

    EXPECT_CALL(*tcp_mock, reg_service(_, _))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(nullptr));

    EXPECT_CALL(*par_mock, final_init(_, _, _));

	EXPECT_EQ(0, G_LUA_MANAGER->init(0, "", "", ""));

    set_file_counter(0);
	test_project_manager::removeObject();
	test_params_manager::removeObject();
	delete tech_mock;
    test_tcp_communicator::removeObject();
}

TEST_F(LuaManagerTest, init_creating_lua_context_failure)
{
    lua_State *s = NULL;
    subhook_t hook_lua_create_context =
        subhook_new((void *) luaL_newstate, (void *) mock_luaL_newstate_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_create_context);

    EXPECT_EQ(1, G_LUA_MANAGER->init(NULL, "", "", ""));

    EXPECT_EQ(s, G_LUA_MANAGER->get_Lua());

    subhook_remove(hook_lua_create_context);
    subhook_free(hook_lua_create_context);
}

TEST_F(LuaManagerTest, init_check_file_failure)
{
    subhook_t hook_check_file =
        subhook_new((void *) check_file, (void *) mock_check_file_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_check_file);

    EXPECT_EQ(1, G_LUA_MANAGER->init(NULL, "", "", ""));

    subhook_remove(hook_check_file);
    subhook_free(hook_check_file);
}

TEST_F(LuaManagerTest, init_check_file_version_failure)
{
    //get lua_state
    lua_State* state = new lua_State;
    set_file_counter(1);

    // also checking of set lua_state
    EXPECT_EQ(1, G_LUA_MANAGER->init(state, "", "", ""));

    set_file_counter(0);
    delete state;
}

TEST_F(LuaManagerTest, init_system_scripts_execution_failure)
{
    subhook_t hook_luaL_loadfile =
        subhook_new((void *)luaL_loadfile, (void *)mock_luaL_loadfile_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_luaL_loadfile);

    EXPECT_EQ(1, G_LUA_MANAGER->init(NULL, "", "", ""));

    subhook_remove(hook_luaL_loadfile);
    subhook_free(hook_luaL_loadfile);
}

TEST_F(LuaManagerTest, init_lua_load_configuration_failure)
{
    lua_State *s = 0;
    mock_project_manager* prj_mock = new mock_project_manager();
    mock_params_manager* par_mock = new mock_params_manager();
    test_project_manager::replaceEntity(prj_mock);
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, init(_));

    EXPECT_CALL(*prj_mock, lua_load_configuration())
        .WillOnce(Return(42));

    EXPECT_EQ(42, G_LUA_MANAGER->init(0, "no_such_script", "", ""));

    EXPECT_NE(s, G_LUA_MANAGER->get_Lua());

    set_file_counter(0);
    test_project_manager::removeObject();
    test_params_manager::removeObject();
}

TEST_F(LuaManagerTest, init_luaL_loadfile_failure)
{
    subhook_t hook_luaL_loadfile =
        subhook_new((void *)luaL_loadfile, (void *)mock_luaL_loadfile_failure_2, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_luaL_loadfile);
    mock_project_manager* prj_mock = new mock_project_manager();
    mock_params_manager* par_mock = new mock_params_manager();
    test_project_manager::replaceEntity(prj_mock);
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, init(_));

    EXPECT_CALL(*prj_mock, lua_load_configuration())
        .WillOnce(Return(0));

    EXPECT_EQ(1, G_LUA_MANAGER->init(NULL, "", "", ""));

    subhook_remove(hook_luaL_loadfile);
    subhook_free(hook_luaL_loadfile);
    set_file_counter(0);
    test_project_manager::removeObject();
    test_params_manager::removeObject();
}

TEST_F(LuaManagerTest, init_lua_pcall_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    const int EXTRA_CALLS_COUNT = 1;
    set_lua_pcall_success_calls_before_failure(FILE_CNT + EXTRA_CALLS_COUNT);

    mock_project_manager* prj_mock = new mock_project_manager();
    mock_params_manager* par_mock = new mock_params_manager();
    test_project_manager::replaceEntity(prj_mock);
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, init(_));

    EXPECT_CALL(*prj_mock, lua_load_configuration())
        .WillOnce(Return(0));

    EXPECT_EQ(1, G_LUA_MANAGER->init(NULL, "", "", ""));

    set_file_counter(0);
    test_project_manager::removeObject();
    test_params_manager::removeObject();
    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

TEST_F(LuaManagerTest, init_init_objects_failure)
{
    char* res = 0;
    mock_project_manager* prj_mock = new mock_project_manager();
    mock_params_manager* par_mock = new mock_params_manager();
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, get_params_data(_, _))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(res));

    mock_tech_object_manager* tech_mock = new mock_tech_object_manager(); //require par_mock
    test_project_manager::replaceEntity(prj_mock);
    mock_tech_object_manager::set_instance(tech_mock);

    EXPECT_CALL(*par_mock, init(_));

    EXPECT_CALL(*prj_mock, lua_load_configuration())
        .WillOnce(Return(0));

    EXPECT_CALL(*tech_mock, init_objects())
        .WillOnce(Return(42));

    EXPECT_EQ(42, G_LUA_MANAGER->init(0, "", "", ""));

    set_file_counter(0);
    test_project_manager::removeObject();
    test_params_manager::removeObject();
    delete tech_mock;
}

TEST_F(LuaManagerTest, init_PAC_name_rus_failure)
{
    char* res = 0;
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    const int EXTRA_CALLS_COUNT = 3;
    set_lua_pcall_success_calls_before_failure(FILE_CNT + EXTRA_CALLS_COUNT);

    mock_project_manager* prj_mock = new mock_project_manager();
    mock_params_manager* par_mock = new mock_params_manager();
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, get_params_data(_, _))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(res));

    mock_tech_object_manager* tech_mock = new mock_tech_object_manager(); //require par_mock
    test_project_manager::replaceEntity(prj_mock);
    mock_tech_object_manager::set_instance(tech_mock);

    EXPECT_CALL(*par_mock, init(_));

    EXPECT_CALL(*prj_mock, lua_load_configuration())
        .WillOnce(Return(0));

    EXPECT_CALL(*tech_mock, init_objects())
        .WillOnce(Return(0));

    EXPECT_EQ(1, G_LUA_MANAGER->init(0, "", "", ""));

    set_file_counter(0);
    test_project_manager::removeObject();
    test_params_manager::removeObject();
    delete tech_mock;
    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

TEST_F(LuaManagerTest, init_PAC_name_eng_failure)
{
    char* res = 0;
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    const int EXTRA_CALLS_COUNT = 3;
    set_lua_pcall_success_calls_before_failure(FILE_CNT + EXTRA_CALLS_COUNT);

    mock_project_manager* prj_mock = new mock_project_manager();
    mock_params_manager* par_mock = new mock_params_manager();
    test_params_manager::replaceEntity(par_mock);

    EXPECT_CALL(*par_mock, get_params_data(_, _))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(res));

    mock_tech_object_manager* tech_mock = new mock_tech_object_manager(); //require par_mock
    test_project_manager::replaceEntity(prj_mock);
    mock_tech_object_manager::set_instance(tech_mock);

    EXPECT_CALL(*par_mock, init(_));

    EXPECT_CALL(*prj_mock, lua_load_configuration())
        .WillOnce(Return(0));

    EXPECT_CALL(*tech_mock, init_objects())
        .WillOnce(Return(0));

    EXPECT_EQ(1, G_LUA_MANAGER->init(0, "", "", ""));

    set_file_counter(0);
    test_project_manager::removeObject();
    test_params_manager::removeObject();
    delete tech_mock;
    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
	TEST METHOD DEFENITION:
	int void_exec_lua_method(const char *object_name,
		const char *function_name, const char *c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
*/

TEST_F(LuaManagerTest, void_exec_lua_method_success)
{
    EXPECT_EQ(0, G_LUA_MANAGER->void_exec_lua_method(
                 "test_lua_object_str",
                 "test_lua_func_str",
                 "test_lua_c_func_str"));
}

TEST_F(LuaManagerTest, void_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall = subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_EQ(1, G_LUA_MANAGER->void_exec_lua_method(
                 "test_lua_object_str",
                 "test_lua_func_str",
                 "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	const char* char_no_param_exec_lua_method(const char *object_name,
		const char *function_name, const char *c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	TOLUA_API const char* tolua_tostring (lua_State* L, int narg, const char* def)
*/

TEST_F(LuaManagerTest, char_no_param_exec_lua_method_success)
{
	EXPECT_STREQ("Test passed", G_LUA_MANAGER->char_no_param_exec_lua_method(
                                "test_lua_object_str",
                                "test_lua_func_str",
                                "test_lua_c_func_str"));
}

TEST_F(LuaManagerTest, char_no_param_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_STREQ((char*) 0, G_LUA_MANAGER->char_no_param_exec_lua_method(
                            "test_lua_object_str",
                            "test_lua_func_str",
                            "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	const char* char_exec_lua_method(const char *object_name,
		const char *function_name, int param, const char *c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
	TOLUA_API const char* tolua_tostring (lua_State* L, int narg, const char* def)
*/

TEST_F(LuaManagerTest, char_exec_lua_method_success)
{
	EXPECT_STREQ("Test passed", G_LUA_MANAGER->char_exec_lua_method(
                                "test_lua_object_str",
                                "test_lua_func_str",
                                42,
                                "test_lua_c_func_str"));
}

TEST_F(LuaManagerTest, char_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_STREQ((char*)0, G_LUA_MANAGER->char_exec_lua_method(
                           "test_lua_object_str",
                           "test_lua_func_str",
                           42,
                           "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	int int_exec_lua_method(const char *object_name,
		const char *function_name, int param,
		const char *c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
	TOLUA_API lua_Number tolua_tonumber (lua_State* L, int narg, lua_Number def)
*/

TEST_F(LuaManagerTest, int_exec_lua_method_success)
{
	EXPECT_EQ(0, G_LUA_MANAGER->int_exec_lua_method(
                 "test_lua_object_str",
                 "test_lua_func_str",
                 42,
                 "test_lua_c_func_str"));
}

TEST_F(LuaManagerTest, int_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_EQ(-1, G_LUA_MANAGER->int_exec_lua_method(
                  "test_lua_object_str",
                  "test_lua_func_str",
                  42,
                  "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	int int_2_exec_lua_method(const char* object_name,
		const char* function_name, int param1, int param2,
		const char* c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
	TOLUA_API lua_Number tolua_tonumber (lua_State* L, int narg, lua_Number def)
*/

TEST_F(LuaManagerTest, int_2_exec_lua_method_success)
{
	EXPECT_EQ(0, G_LUA_MANAGER->int_2_exec_lua_method(
                 "test_lua_object_str",
                 "test_lua_func_str",
                 0,
                 42,
                 "test_lua_c_func_str"));
}

TEST_F(LuaManagerTest, int_2_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_EQ(-1, G_LUA_MANAGER->int_2_exec_lua_method(
                  "test_lua_object_str",
                  "test_lua_func_str",
                  0,
                  42,
                  "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	int int_no_param_exec_lua_method(const char *object_name,
		const char *function_name, const char *c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
	TOLUA_API lua_Number tolua_tonumber (lua_State* L, int narg, lua_Number def)
*/

TEST_F(LuaManagerTest, int_no_param_exec_lua_method_success)
{
	EXPECT_EQ(0, G_LUA_MANAGER->int_no_param_exec_lua_method(
                 "test_lua_object_str",
                 "test_lua_func_str",
                 "test_lua_c_func_str"));
}

TEST_F(LuaManagerTest, int_no_param_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_EQ(1, G_LUA_MANAGER->int_no_param_exec_lua_method(
                 "test_lua_object_str",
                 "test_lua_func_str",
                 "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	void* user_object_exec_lua_method(const char *object_name,
		const char *function_name, int param,
		const char *c_function_name) const;

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
	TOLUA_API void* tolua_tousertype (lua_State* L, int narg, void* def)
*/

TEST_F(LuaManagerTest, user_object_exec_lua_method_success)
{
	EXPECT_STREQ("Test passed", (char*) (G_LUA_MANAGER->user_object_exec_lua_method(
                                "test_lua_object_str",
                                "test_lua_func_str",
                                42,
                                "test_lua_c_func_str")));
}

TEST_F(LuaManagerTest, user_object_exec_lua_method_failure)
{
    subhook_t hook_lua_pcall = subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_EQ(NULL, G_LUA_MANAGER->user_object_exec_lua_method(
                    "test_lua_object_str",
                    "test_lua_func_str",
                    42,
                    "test_lua_c_func_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	int lua_manager::exec_Lua_str( const char *Lua_str, const char *error_str,
								  bool is_print_error_msg = true ) const

	HOOKED:
	LUA_API int (luaL_loadstring) (lua_State *L, const char *s)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
*/

TEST_F(LuaManagerTest, exec_Lua_str_success)
{
	EXPECT_EQ(0, G_LUA_MANAGER->exec_Lua_str("test_lua_str", "error_test_str"));
}

TEST_F(LuaManagerTest, exec_Lua_str_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    EXPECT_EQ(1, G_LUA_MANAGER->exec_Lua_str("test_lua_str", "error_test_str"));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

/*
    TEST METHOD DEFENITION:
	int reload_script(int script_n, const char* script_function_name,
		char *res_str, int max_res_str_length);

	HOOKED:
	LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
	LUA_API int lua_gettop (lua_State *L)
	LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
	LUA_API void lua_remove (lua_State *L, int idx)
	LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
	LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
	TOLUA_API void* tolua_tousertype (lua_State* L, int narg, void* def)
*/

TEST_F(LuaManagerTest, reload_script_lua_state_null_failure)
{
    // set lua_state to NULL
    lua_State *s = NULL;
    subhook_t hook_lua_create_context =
        subhook_new((void *)luaL_newstate, (void *)mock_luaL_newstate_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_create_context);

    EXPECT_EQ(1, G_LUA_MANAGER->init(NULL, "", "", ""));
    EXPECT_EQ(s, G_LUA_MANAGER->get_Lua());

    // test_reload_script
    char ans[100];
    EXPECT_EQ(1, G_LUA_MANAGER->reload_script(INT_MAX, "test_lua_func_str", ans, sizeof(ans)));

    subhook_remove(hook_lua_create_context);
    subhook_free(hook_lua_create_context);
}

TEST_F(LuaManagerTest, reload_script_success)
{
    lua_State* state = new lua_State;
    set_file_counter(1);
    // easiest way to set lua_state is
    // to shutdown with wrong script version (G->LOG CRITIC message here)
    EXPECT_EQ(1, G_LUA_MANAGER->init(state, "", "", ""));

    // file_serial_number = any from 0 to lua_manager::FILE_CNT
    const int file_serial_number = 0;
    set_file_counter(file_serial_number);
    char ans[100];

	EXPECT_EQ(0, G_LUA_MANAGER->reload_script(
                 file_serial_number, "test_lua_func_str", ans, sizeof(ans)));
}

TEST_F(LuaManagerTest, reload_script_exceeded_script_number_failure)
{
    char ans[100];
    EXPECT_EQ(1, G_LUA_MANAGER->reload_script(INT_MAX, "test_lua_func_str", ans, sizeof(ans)));
}

TEST_F(LuaManagerTest, reload_script_check_file_failure)
{
    subhook_t hook_check_file =
        subhook_new((void *)check_file, (void *)mock_check_file_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_check_file);

    // file_serial_number = any from 0 to lua_manager::FILE_CNT
    const int file_serial_number = FILE_CNT - 1;
    set_file_counter(file_serial_number);
    char ans[100];

    EXPECT_EQ(1, G_LUA_MANAGER->reload_script(
                 file_serial_number, "test_lua_func_str", ans, sizeof(ans)));

    subhook_remove(hook_check_file);
    subhook_free(hook_check_file);
}

TEST_F(LuaManagerTest, reload_script_check_file_version_failure)
{
    subhook_t hook_check_file =
        subhook_new((void *)check_file, (void *)mock_check_file_failure_2, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_check_file);

    // file_serial_number = any from 0 to lua_manager::FILE_CNT
    const int file_serial_number = 0;
    set_file_counter(file_serial_number);
    char ans[100];

    EXPECT_EQ(1, G_LUA_MANAGER->reload_script(
                 file_serial_number, "test_lua_func_str", ans, sizeof(ans)));

    subhook_remove(hook_check_file);
    subhook_free(hook_check_file);
}

TEST_F(LuaManagerTest, reload_script_luaL_dofile_failure)
{
    subhook_t hook_lua_pcall =
        subhook_new((void *)lua_pcall, (void *)mock_lua_pcall_failure, SUBHOOK_64BIT_OFFSET);
    subhook_install(hook_lua_pcall);
    set_lua_pcall_success_calls_before_failure(0);

    // file_serial_number = any from 0 to lua_manager::FILE_CNT
    const int file_serial_number = 0;
    set_file_counter(file_serial_number);
    char ans[100];

    EXPECT_EQ(1, G_LUA_MANAGER->reload_script(
                 file_serial_number, "test_lua_func_str", ans, sizeof(ans)));

    subhook_remove(hook_lua_pcall);
    subhook_free(hook_lua_pcall);
}

TEST( lua_manager, error_trace )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );
    lua_manager::use_print_stack_traceback();
    
    EXPECT_EQ( 0, luaL_dostring( L, "t = {}" ) );
    EXPECT_EQ( 0, luaL_dostring( L, "t.no_exist1 = 1" ) );
    EXPECT_EQ( 0, luaL_dostring( L, "t.no_exist2 = {}" ) );
    EXPECT_EQ( 0, luaL_dostring( L, "t.no_exist3 = function(a, b) return a + b end" ) );

    EXPECT_NE( 0, G_LUA_MANAGER->void_exec_lua_method( "t", "no_exist1", "error_trace") );
    EXPECT_NE( 0, G_LUA_MANAGER->void_exec_lua_method( "t", "no_exist2", "error_trace" ) );
    EXPECT_NE( 0, G_LUA_MANAGER->void_exec_lua_method( "t", "no_exist3", "error_trace" ) );
    G_LUA_MANAGER->free_Lua();
    }
