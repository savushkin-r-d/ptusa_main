#include "param_ex.h"
#include "lua_manager.h"
#include "device/manager.h"

auto_smart_ptr< params_manager > params_manager::instance = 0;
char params_manager::is_init = 0;

#ifdef USE_SIMPLE_DEV_ERRORS
#include "g_errors.h"
#endif // USE_SIMPLE_DEV_ERRORS

#ifndef USE_NO_TANK_COMB_DEVICE
#include "tech_def.h"
#endif //USE_NO_TANK_COMB_DEVICE

#include "PAC_info.h"
#include "g_errors.h"

#include "log.h"
//-----------------------------------------------------------------------------
params_manager::params_manager(): par( 0 ), project_id( 0 )
    {
    last_idx = 0;
    CRC_mem = NV_memory_manager::get_instance()->get_memory_block(
        NV_memory_manager::MT_NVRAM, C_SYS_MEM_SIZE );

    params_mem = NV_memory_manager::get_instance()->get_memory_block(
        NV_memory_manager::MT_EEPROM, C_TOTAL_PARAMS_SIZE );

    memset( params, 0, C_TOTAL_PARAMS_SIZE );
    }
//-----------------------------------------------------------------------------
u_int_2 params_manager::solve_CRC()
    {
    int     idx;
    char    Flag;

    u_int_2 CRC = 65535;
    unsigned int datlen = C_TOTAL_PARAMS_SIZE;
    int bufidx = 0;

    while ( datlen > 0 )
        {
        CRC = CRC ^ params[ bufidx ];
        for ( idx = 0; idx <= 7; idx++ )
            {
            Flag = CRC & 1;
            CRC = CRC >> 1;
            if ( Flag ) CRC = CRC ^ 0x0A001;
            }
        datlen--;
        bufidx++;
        }
    char* p = ( char* ) &project_id;
    CRC = CRC ^ p[ 0 ];
    for ( idx = 0; idx <= 7; idx++ )
        {
        Flag = CRC & 1;
        CRC = CRC >> 1;
        if ( Flag ) CRC = CRC ^ 0x0A001;
        }
    CRC = CRC ^ p[ 1 ];
    for ( idx = 0; idx <= 7; idx++ )
        {
        Flag = CRC & 1;
        CRC = CRC >> 1;
        if ( Flag ) CRC = CRC ^ 0x0A001;
        }

    return CRC;
    }
//-----------------------------------------------------------------------------
void params_manager::reset_params_size()
    {
    unsigned char buff[ 4 ] = { 0 };
    CRC_mem->write( ( char* )buff, 4, C_LAST_IDX_OFFSET );
    }
//-----------------------------------------------------------------------------
int params_manager::init( unsigned int project_id )
    {
    params_manager::project_id = project_id;

    memset( params, 0, C_TOTAL_PARAMS_SIZE );
    params_mem->read( params, C_TOTAL_PARAMS_SIZE, 0 );

    return 0;
    }
//-----------------------------------------------------------------------------
void params_manager::final_init( int auto_init_params /*= 1*/,
                                int auto_init_work_params /*= 1*/,
                                void ( *custom_init_params_function )() /*= 0 */ )
    {
    sprintf( G_LOG->msg, "Total memory used: %u of %u bytes[ %.2f%c ].",
            last_idx, C_TOTAL_PARAMS_SIZE,
            100. * last_idx / C_TOTAL_PARAMS_SIZE, '%' );
    G_LOG->write_log( i_log::P_DEBUG );

    //Проверка на изменение количества параметров.
    unsigned char buff[ 4 ] = { 0 };
    CRC_mem->read( ( char* ) buff, 4, C_LAST_IDX_OFFSET );
    u_int* last_idx_ = ( u_int* ) buff;
    if ( *last_idx_ != last_idx )
        {
        sprintf( G_LOG->msg,
            "Total params size has changed (%d != %d), reinitialization.",
            last_idx, *last_idx_ );
        G_LOG->write_log( i_log::P_NOTICE );

        char *buff = ( char* ) &last_idx;   //Запись количества параметров.
        CRC_mem->write( buff, 4, C_LAST_IDX_OFFSET );

        reset_to_default( custom_init_params_function, auto_init_params,
            auto_init_work_params );
        }
    }
//-----------------------------------------------------------------------------
void params_manager::reset_to_default( void( *custom_init_params_function )( ),
    int auto_init_params, int auto_init_work_params )
    {
    memset( params, 0, C_TOTAL_PARAMS_SIZE );

    if ( custom_init_params_function != 0 )
        {
        ( *custom_init_params_function )( );
        }

    PAC_info::get_instance()->reset_params();

    if ( auto_init_params )
        {
#ifndef USE_NO_TANK_COMB_DEVICE
        tech_object_manager::get_instance()->init_params();
#endif // USE_NO_TANK_COMB_DEVICE

        G_ERRORS_MANAGER->reset_errors_params();

        G_DEVICE_MANAGER()->init_params();
        }

    if ( auto_init_work_params )
        {
#ifndef USE_NO_TANK_COMB_DEVICE
        tech_object_manager::get_instance()->init_runtime_params();
#endif // USE_NO_TANK_COMB_DEVICE
        }

    par[ 0 ][ P_IS_RESET_PARAMS ] = 0;

    save();
#ifdef KEY_CONFIRM
    printf( "Press any key to continue..." );
    get_char();
    printf( "\n" );
#endif // KEY_CONFIRM
        }
//-----------------------------------------------------------------------------
void params_manager::save( int start_pos, int count )
    {
    if ( 0 == count )
        {
        count = C_TOTAL_PARAMS_SIZE;
        }

    params_mem->write( params + start_pos, count, start_pos );
    }
//-----------------------------------------------------------------------------
char* params_manager::get_params_data( int size, int &start_pos )
    {
    char *res = nullptr;

    if ( last_idx + size > params_mem->get_size() )
        {
        if ( G_DEBUG )
            {
            printf( "params_manager::get_params_data() - is not enough memory ( %d + %d < %d ) !\n",
                last_idx, size, params_mem->get_size() );
            }
        return 0;
        }

    res = params + last_idx;
    start_pos = last_idx;
    last_idx += size;

    return res;
    }
//-----------------------------------------------------------------------------
params_manager* params_manager::get_instance()
    {    
    if ( 0 == is_init )
        {
        is_init = 1;
        instance = new params_manager();

        instance->par = new saved_params_u_int_4( P_COUNT );
        }
    return instance;
    }
//-----------------------------------------------------------------------------
params_manager::~params_manager()
    {
    if ( CRC_mem )
        {
        delete params_mem;
        params_mem = nullptr;
        }
    if ( CRC_mem )
        {
        delete CRC_mem;
        CRC_mem = nullptr;
        }

    delete par;
    par = nullptr;
    }
//-----------------------------------------------------------------------------
int params_manager::save_params_as_Lua_str( char* str )
    {
    int res = 0;
    //res += G_DEVICE_MANAGER()->save_params_as_Lua_str( str );
    res += G_TECH_OBJECT_MNGR()->save_params_as_Lua_str( str + res );

    return res;
    }
//-----------------------------------------------------------------------------
int params_manager::restore_params_from_server_backup( char *backup_str )
    {
    static bool is_init = false;
    if ( false == is_init )
        {
        const char *extra_cmd = "function params ( par_info )\n"
            "\n"
            "local cmd\n"
            "--if par_info == nil then return\n"
            "\n"
            "for index, value in pairs( par_info.values ) do\n"
            "    cmd = 'sys.'..par_info.object..':set_param('..par_info.par_id..','..\n"
            "        ( index - 1 )..','..value..')'\n"
            "    assert( loadstring( cmd ) )()\n"
            "    end\n"
            "end\n";

        lua_manager::get_instance()->exec_Lua_str( extra_cmd,
            "params_manager::restore_params_from_server_backup - init block");

        is_init = true;
        }


    int res = lua_manager::get_instance()->exec_Lua_str( backup_str,
        "params_manager::restore_params_from_server_backup ");

    if ( 0 == res )
        {
        par[ 0 ][ P_IS_RESET_PARAMS ] = 0;
        par->save_all();
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int params_test::make_test()
    {
    if ( G_DEBUG )
        {
        printf( "Start params test.\n" );
        }

    const u_int POJECT_ID = 2;
    params_manager::get_instance()->init( POJECT_ID );

    saved_params_u_int_4 test1( 10 );
    //test1.save( 0, 5120 );
    //test1.save( 1, 120 );
    //test1.save( 2, 130 );

    saved_params_float test( 10 );
    //test.save( 0, 512 );
    //test.save( 1, 12 );
    //test.save( 2, 13 );

    params_manager::get_instance()->init( POJECT_ID );
    params_manager::get_instance()->final_init();

    if (
        test1[ 0 ] != 5120 ||
        test1[ 1 ] != 120 ||
        test1[ 2 ] != 130 ||
        test[ 0 ] != 512 ||
        test[ 1 ] != 12 ||
        test[ 2 ] != 13 )
        {
        if ( G_DEBUG )
            {
            printf( "Error passing params test!\n" );
            printf( "test[ 0 ] = %f\n", test[ 0 ] );
            printf( "test[ 1 ] = %f\n", test[ 1 ] );
            printf( "test[ 2 ] = %f\n", test[ 2 ] );

            printf( "test1[ 0 ] = %lu\n", ( u_long ) test1[ 0 ] );
            printf( "test1[ 1 ] = %lu\n", ( u_long ) test1[ 1 ] );
            printf( "test1[ 2 ] = %lu\n", ( u_long ) test1[ 2 ] );
            get_char();
            }
        return 1;
        }

    if ( G_DEBUG )
        {
        printf( "Passing params test - ok!\n" );

#ifdef KEY_CONFIRM
        printf( "Press any key to continue..." );
        get_char();
        printf( "\n" );
#endif // KEY_CONFIRM
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
