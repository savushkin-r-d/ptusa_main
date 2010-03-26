#include "param_ex.h"

params_manager* params_manager::instance = 0;
NV_memory_manager *G_NV_MEMORY_MANAGER = new NV_memory_manager_7186();
//-----------------------------------------------------------------------------
params_manager::params_manager()
    {
    last_idx = 0;
    params_mem = G_NV_MEMORY_MANAGER->get_memory_block( 
        NV_memory_manager::MT_EEPROM, C_TOTAL_PARAMS_SIZE );

    CRC_mem = G_NV_MEMORY_MANAGER->get_memory_block(
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
int params_manager::init( unsigned int project_id /*= 65535 */ )
    {
    params_manager::project_id = project_id;

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
    char buff[ 2 ];
    CRC_mem->read( buff, 2, 0 );

    if ( solve_CRC() == *( ( int* ) buff ) )  
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
    Print( "Total memory used: %u [ %f% ]. \n", 
        last_idx, last_idx / C_TOTAL_PARAMS_SIZE * 100 );
#endif // DEBUG

    if ( -1 == loaded )
        {
        if ( custom_init_params_function != 0 ) 
            {
            ( *custom_init_params_function )();
            }

        if ( auto_init_params ) 
            {
#ifndef USE_NO_TANK_COMB_DEVICE
            int i;
            for ( i = 0; i < TTank::TankCnt; i++ )
                {
                g_tanks[ i ]->InitParams();
                }
            for ( i = 0; i< TMyComb::CombCnt; i++)
                {    
                g_combs[ i ]->InitParams();
                }
#endif // USE_NO_TANK_COMB_DEVICE

#ifdef USE_SIMPLE_DEV_ERRORS 
            g_dev_errors_manager->reset_errors_params();
#endif // USE_SIMPLE_DEV_ERRORS
            }

        if ( auto_init_work_params ) 
            {
#ifndef USE_NO_TANK_COMB_DEVICE
            for ( int i = 0; i < TTank::TankCnt; i++ )
                {
                g_tanks[ i ]->InitWorkParams();
                }
            for ( i = 0; i< TMyComb::CombCnt; i++ )
                {   
                g_combs[ i ]->InitWorkParams();
                }
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
        Getch();
        Print( "\n" );
#endif // KEY_CONFIRM
#endif
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
template < class type >
parameters<type>::parameters( int count, 
                             type *value /*= 0 */ ) : count( count ),
                             values( value )
    {
#ifdef DEBUG
    if ( 0 == count )
        {
        Print( "parameters(...) - error: count = 0!\n" );
        }
#endif // DEBUG
    if ( count > 0 && 0 == value )
        {
        values = new type[ count ];
        memset( values, 0, count * sizeof( type ) );
        }
    }
//-----------------------------------------------------------------------------
template < class type >
type& parameters<type>::operator[]( unsigned int index )
    {
    if ( index < count )
        {
        return values[ index ];
        }
#ifdef DEBUG
    else
        {
        Print( "parameters[] - error: index[ %u ] > count [ %u ]\n",
            index, count );
        }
#endif // DEBUG

    stub = 0;
    return stub;
    }
//-----------------------------------------------------------------------------
template < class type >
type parameters<type>::get_val( int idx )
    {
    return this->operator []( idx );
    }
//-----------------------------------------------------------------------------
template < class type >
int parameters<type>::parse_cmd( char *buff )
    {
    int par_n = *( ( u_int_4* ) buff );
    this[ par_n ] = *( ( type* ) ( buff + sizeof( u_int_4 ) ) );

    return sizeof( u_int_4 ) + sizeof( type );
    }
//-----------------------------------------------------------------------------
template < class type > u_int parameters<type>::get_count() const
    {
    return count;
    }
//-----------------------------------------------------------------------------
template < class type > type* parameters<type>::get_values()
    {
    return values;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
run_time_params_float::run_time_params_float( int count 
                                             ):parameters < float >( count ),
                                             array_device < float >( 1, 
                                             "RT_PARAM_F", 
                                             count, 
                                             i_complex_device::ARRAY_DEV_FLOAT )
    {
    }
//-----------------------------------------------------------------------------
float run_time_params_float::get_val( int idx ) const
    {
    return parameters< float >::get_val( idx );
    }
//-----------------------------------------------------------------------------
int run_time_params_float::parse_cmd( char *buff )
    {
    int res = parameters< float >::parse_cmd( buff );
#ifdef DEBUG
    Print( "Set val work param float[ %lu ] = %f\n",
        *( ( u_int_4* ) buff ), 
        *( ( float* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif //DEBUG
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
run_time_params_ulong::run_time_params_ulong( 
    int count  ) : parameters < u_int_4 >( count ),
    array_device < u_int_4 >( 1, "RT_PARAM_UL", count, 
    i_complex_device::ARRAY_DEV_ULONG )
    {

    }
//-----------------------------------------------------------------------------
u_int_4 run_time_params_ulong::get_val( int idx )
    {
    return parameters< u_int_4 >::get_val( idx );
    }
//-----------------------------------------------------------------------------
int run_time_params_ulong::parse_cmd( char *buff )
    {
    int res = parameters< u_int_4 >::parse_cmd( buff );
#ifdef DEBUG
    Print( "Set val work param ulong[ %lu ] = %lu\n",
        *( ( u_int_4* ) buff ), 
        *( ( u_int_4* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif //DEBUG
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < class type > saved_params<type>::
saved_params( int count ) : parameters < type >( 
    count,
    ( type* ) params_manager::get_instance()->get_params_data( 
    count * sizeof( type ), start_pos ) )
    {
    }
//-----------------------------------------------------------------------------
template < class type >
int saved_params<type>::parse_cmd( char *buff )
    {
    int res = parameters< type >::parse_cmd( buff );

    u_int_4 idx = *( ( u_int_4* ) buff );

    params_manager::get_instance()->save( 
        start_pos + idx, sizeof( u_int_4* ) );

    return res;
    }
//-----------------------------------------------------------------------------
template < class type >
int saved_params< type >::save( u_int idx, type value )
    {
    int res = 1;
    if ( idx < get_count() )
        {
        get_values()[ idx ] = value;

        params_manager::get_instance()->save( 
            start_pos + idx * sizeof( type ), sizeof( type ) );
        res = 0;
        }
#ifdef DEBUG
    else
        {
        Print( "parameters:save - index[ %u ] > count [ %u ]\n",
            idx, get_count() );
        }
#endif // DEBUG
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
saved_params_ulong::
saved_params_ulong( int count ) :
saved_params < u_int_4 >( count ),
array_device < u_int_4 >( 1, "S_PARAM_UL", count, 
                         i_complex_device::ARRAY_DEV_ULONG )
    {
    }
//-----------------------------------------------------------------------------
u_int_4 saved_params_ulong::get_val( int idx )
    {
    return saved_params< u_int_4 >::get_val( idx );
    }
//-----------------------------------------------------------------------------
int saved_params_ulong::parse_cmd( char *buff )
    {
    int res = saved_params< u_int_4 >::parse_cmd( buff );
#ifdef DEBUG
    Print( "Set val saved param ulong[ %lu ] = %lu\n",
        *( ( u_int_4* ) buff ), 
        *( ( u_int_4* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif //DEBUG
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
saved_params_float::
saved_params_float( int count ):
saved_params < float >( count ),
array_device < float >( 1, "S_PARAM_F", count, 
                         i_complex_device::ARRAY_DEV_FLOAT )
    {

    }
//-----------------------------------------------------------------------------
float saved_params_float::get_val( int idx )
    {
    return saved_params< float >::get_val( idx );
    }
//-----------------------------------------------------------------------------
int saved_params_float::parse_cmd( char *buff )
    {
    int res = saved_params< float >::parse_cmd( buff );
#ifdef DEBUG
    Print( "Set val saved param float[ %lu ] = %f\n",
        *( ( u_int_4* ) buff ), 
        *( ( float* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif //DEBUG
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int params_test::make_test()
    {
    params_manager::get_instance()->init( 2 );

    saved_params_ulong test1( 10 );
    test1.save( 0, 5120 );        
    test1.save( 1, 120 );        
    test1.save( 2, 130 );

    saved_params_float test( 10 );
    test.save( 0, 512 );        
    test.save( 1, 12 );        
    test.save( 2, 13 );

    params_manager::get_instance()->init( 2 );

    if ( 
        test1[ 0 ] != 5120 || 
        test1[ 1 ] != 120 ||
        test1[ 2 ] != 130 ||
        test[ 0 ] != 512 || 
        test[ 1 ] != 12 ||
        test[ 2 ] != 13 )
        {
#ifdef DEBUG
        Print( "Error passing test!\n" );
        Print( "test[ 0 ] = %f\n", test[ 0 ] );
        Print( "test[ 1 ] = %f\n", test[ 1 ] );
        Print( "test[ 2 ] = %f\n", test[ 2 ] );
        Getch();
#endif // DEBUG
        return 1;
        }

#ifdef DEBUG
    Print( "Passing test - ok!\n" );
    Print( "test[ 0 ] = %f\n", test[ 0 ] );
    Print( "test[ 1 ] = %f\n", test[ 1 ] );
    Print( "test[ 2 ] = %f\n", test[ 2 ] );
    Getch();
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------