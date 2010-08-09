#include "param_ex.h"

params_manager* params_manager::instance = 0;

#ifdef USE_SIMPLE_DEV_ERRORS
#include "errors.h"

extern dev_errors_manager *g_dev_errors_manager; 
#endif // USE_SIMPLE_DEV_ERRORS

#ifndef USE_NO_TANK_COMB_DEVICE
#include "tech_def.h"
#endif //USE_NO_TANK_COMB_DEVICE

//-----------------------------------------------------------------------------
params_manager::params_manager()
    {
    last_idx = 0;
    params_mem = NV_memory_manager::get_instance()->get_memory_block( 
        NV_memory_manager::MT_EEPROM, C_TOTAL_PARAMS_SIZE );

    CRC_mem = NV_memory_manager::get_instance()->get_memory_block(
        NV_memory_manager::MT_NVRAM, 2 );

    memset( params, 0, C_TOTAL_PARAMS_SIZE );
    }
//-----------------------------------------------------------------------------
unsigned int params_manager::solve_CRC()
    {
    int     idx;
    char    Flag;

    unsigned int CRC = 65535;
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
int params_manager::init( unsigned int project_id )
    {
    params_manager::project_id = project_id;

    memset( params, 0, C_TOTAL_PARAMS_SIZE );
    params_mem->read( params, C_TOTAL_PARAMS_SIZE, 0 );

    int chk = check_CRC();
    if ( 0 == chk ) 
        {
#ifdef DEBUG
        Print( "PARAMS OK. PARAMS LOADED.\n\r" );
#endif
        loaded = 1;
        } 
    else 
        {
#ifdef DEBUG
        Print( "Project id = %u. PARAMS CRC CHECK FAILED: Trying to reinitialize...\n\r", 
            project_id );
#endif
        loaded = -1;
        }
    return chk;
    }
//-----------------------------------------------------------------------------
int params_manager::check_CRC()
    {
    unsigned char buff[ 2 ];
    CRC_mem->read( ( char* ) buff, 2, 0 );

    u_int read_CRC = 256 * buff[ 1 ] + buff[ 0 ];

    if ( solve_CRC() == read_CRC )
        {
        return 0;
        }

    return 1;
    }
//-----------------------------------------------------------------------------
void params_manager::final_init( int auto_init_params /*= 1*/, 
                                int auto_init_work_params /*= 1*/, 
                                void ( *custom_init_params_function )() /*= 0 */ )
    {
#ifdef DEBUG
    Print( "Total memory used: %u of %u bytes[ %.2f%c ]. \n",
        last_idx, C_TOTAL_PARAMS_SIZE, 
        100. * last_idx / C_TOTAL_PARAMS_SIZE, '%' );
#endif // DEBUG

    if ( -1 == loaded )
        {
        memset( params, 0, C_TOTAL_PARAMS_SIZE );

        if ( custom_init_params_function != 0 ) 
            {
            ( *custom_init_params_function )();
            }

        if ( auto_init_params ) 
            {
#ifndef USE_NO_TANK_COMB_DEVICE
            tech_object_manager::get_instance()->init_params();
#endif // USE_NO_TANK_COMB_DEVICE

#ifdef USE_SIMPLE_DEV_ERRORS 
            g_dev_errors_manager->reset_errors_params();
#endif // USE_SIMPLE_DEV_ERRORS
            }

        if ( auto_init_work_params ) 
            {
#ifndef USE_NO_TANK_COMB_DEVICE
            tech_object_manager::get_instance()->init_runtime_params();
#endif // USE_NO_TANK_COMB_DEVICE 
            }

        save();

#ifdef DEBUG
        if ( check_CRC() == 0 )
            Print( "PARAMS OK: PARAMS SUCCESFULLY REINITIALIZED.\n\r" );
        else
            Print( "PARAMS: FATAL ERROR.\n\r" );
#ifdef KEY_CONFIRM
        Print( "Press any key to continue..." );
        get_char();
        Print( "\n" );
#endif // KEY_CONFIRM
#endif // DEBUG
        }
    else
        {
#ifdef DEBUG
        if ( check_CRC() == 0 )
            Print( "PARAMS OK: DON'T NEED REINITIALIZING.\n\r" );
        else
            Print( "PARAMS: FATAL ERROR.\n\r" );
#ifdef KEY_CONFIRM
        Print( "Press any key to continue..." );
        get_char();
        Print( "\n" );
#endif // KEY_CONFIRM
#endif // DEBUG
        }
    }
//-----------------------------------------------------------------------------
void params_manager::make_CRC()
    {
    unsigned int CRC = solve_CRC();
    char *buff = ( char* ) &CRC;

    CRC_mem->write( buff, 2, 0 );
    }
//-----------------------------------------------------------------------------
void params_manager::save( int start_pos, int count )
    {        
    if ( 0 == count )
        {
        count = C_TOTAL_PARAMS_SIZE;
        }

    params_mem->write( params + start_pos, count, start_pos );

    make_CRC();
    }
//-----------------------------------------------------------------------------
void params_manager::reset_CRC()
    {
    char buff[ 2 ] = { 0 };
    CRC_mem->write( buff, 2, 0 );
    }
//-----------------------------------------------------------------------------
char* params_manager::get_params_data( int size, int &start_pos )
    {
    char *res = 0;

    if ( last_idx + size > params_mem->get_size() )
        {
#ifdef DEBUG
        Print( "params_manager::get_params_data() - is not enough memory ( %d + %d < %d ) !\n",
            last_idx, size, params_mem->get_size() );
#endif // DEBUG
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
    if ( 0 == instance )
        {
        instance = new params_manager;
        }
    return instance;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int params_test::make_test()
    {
#ifdef DEBUG
    Print( "Start params test.\n" );
#endif // DEBUG

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
#ifdef DEBUG
        Print( "Error passing params test!\n" );
        Print( "test[ 0 ] = %f\n", test[ 0 ] );
        Print( "test[ 1 ] = %f\n", test[ 1 ] );
        Print( "test[ 2 ] = %f\n", test[ 2 ] );

        Print( "test1[ 0 ] = %lu\n", ( u_long ) test1[ 0 ] );
        Print( "test1[ 1 ] = %lu\n", ( u_long ) test1[ 1 ] );
        Print( "test1[ 2 ] = %lu\n", ( u_long ) test1[ 2 ] );
        get_char();
#endif // DEBUG
        return 1;
        }

#ifdef DEBUG
        Print( "Error passing params test!\n" );
        Print( "test[ 0 ] = %f\n", test[ 0 ] );
        Print( "test[ 1 ] = %f\n", test[ 1 ] );
        Print( "test[ 2 ] = %f\n", test[ 2 ] );

        Print( "test1[ 0 ] = %lu\n", ( u_long ) test1[ 0 ] );
        Print( "test1[ 1 ] = %lu\n", ( u_long ) test1[ 1 ] );
        Print( "test1[ 2 ] = %lu\n", ( u_long ) test1[ 2 ] );
        get_char();

    Print( "Passing params test - ok!\n" );

#ifdef KEY_CONFIRM
    Print( "Press any key to continue..." );
    get_char();
    Print( "\n" );
#endif // KEY_CONFIRM

#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
