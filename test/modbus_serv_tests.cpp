#include "modbus_serv_tests.h"
#include "lua_manager.h"

extern int isMsa;

TEST( ModbusServ, ModbusService )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );
    const auto BUFSIZE = 100;
    u_char  buf[ BUFSIZE ] = { 0 };


    auto res = ModbusServ::ModbusService( BUFSIZE, buf, buf );
    // Ничего сервис не делает, так как первый байт - 0.
    EXPECT_EQ( res, 0 );
    
    isMsa = 0;
    buf[ 1 ] = 0x01;    // Read coils.

    const auto bad_read_coils_str = 
R"(function read_coils( n, start_idx, count )
    res[ #res + 1 ] = 0 --Добавляем новый элемент.
    return res
end)";
    ASSERT_EQ( 0, luaL_dostring( L, bad_read_coils_str ) );
    res = ModbusServ::ModbusService( BUFSIZE, buf, buf );
    EXPECT_EQ( res, 0 );

    const auto good_read_coils_str =
R"(function read_coils( n, start_idx, count )
    local res = {}
    for coil_n = start_idx, start_idx + count, 1 do
        --local cr_idx = ( n - 1 ) * 2 + 1
        local cr_idx = n

        res[ #res + 1 ] = 0 --Добавляем новый элемент.
        -- if coil_n == 0 then
        --     res[ #res ] = 1
        --elseif coil_n == 1 then
        --     res[ #res ] = 0
        --end
    end

    return res
end)";
    ASSERT_EQ( 0, luaL_dostring( L, good_read_coils_str ) );
    res = ModbusServ::ModbusService( BUFSIZE, buf, buf );
    EXPECT_EQ( res, 3 );


    G_LUA_MANAGER->free_Lua();
    }
