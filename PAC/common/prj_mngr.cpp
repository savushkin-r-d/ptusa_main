#include <string.h>

#include "prj_mngr.h"
#include "wago.h"
#include "PAC_dev.h"
#include "param_ex.h"

smart_ptr< project_manager > project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::proc_main_params( int argc, char *argv[] )
    {
      if ( argc > 1 )
        {
        if ( strcmp( argv[ 1 ], "rcrc" ) == 0 )
            {
#ifdef DEBUG
            Print( "\nReset CRC..." );
#endif
            params_manager::get_instance()->reset_CRC();
#ifdef DEBUG
            Print( " Ok!\n" );
#endif
            }

        }
    return 0;
    }
//-----------------------------------------------------------------------------
int project_manager::load_configuration( const char *file_name )
    {
#ifdef DEBUG
    Print( "Reading description from %s....\n", file_name );
#endif // DEBUG

    if ( cfg_file->fopen( file_name ) <= 0 )
        {
#ifdef DEBUG
        Print( "project_manager:load_configuration(...) - file not found!\n" );            
#endif // DEBUG
        while ( 1 ) { }
        }

    const char *SIGNATURE            = "WG {4B714C08-9602-4130-8563-4B51E08BB9D7}";
    const int   SIGNATURE_SIZE       = 42;    
    char        id[ SIGNATURE_SIZE ] = { 0 };

    cfg_file->fread( id, SIGNATURE_SIZE - 1 );
    cfg_file->fget_line();

    if ( strcmp( id, SIGNATURE ) != 0 )
        {
#ifdef DEBUG
        Print( "project_manager:load_configuration(...) - not correct file format!\n" );            
#endif // DEBUG
        while ( 1 ) { }
        }

    //-Editor editor_version.
    const int CURRENT_EDITOR_VERSION = 12;
    int       editor_version = 0;

    sscanf( cfg_file->fget_line(), "%d", &editor_version );
    if ( editor_version < CURRENT_EDITOR_VERSION )
        {
#ifdef DEBUG
        Print( "project_manager:load_configuration(...) - not correct editor editor_version - %d, must be %d!\n",
            editor_version, CURRENT_EDITOR_VERSION );
#endif // DEBUG
        while ( 1 ) { }
        }

    //-File editor_version.
    int file_version = 0;

    sscanf( cfg_file->fget_line(), "%d", &file_version );
#ifdef DEBUG
    Print( "Editor version - %d.\n", editor_version );
    Print( "File version - %d.\n", file_version );
#endif // DEBUG

    cfg_file->fget_line();

    //-Wago data.
    wago_manager::get_instance()->load_from_cfg_file( cfg_file );

    //-Devices data.
    device_manager::get_instance()->load_from_cfg_file( cfg_file );

    cfg_file->fclose();

    G_DEVICE_CMMCTR->add_device(
        new complex_device( 0, "GLB", device::C_DEVICE_TYPE_CNT, 0 ) );

    for ( int i = 0; i < device::C_DEVICE_TYPE_CNT; i++ )
        {
        int dev_cnt = G_DEVICE_MANAGER->dev_types_ranges[ i ].end_pos -
            G_DEVICE_MANAGER->dev_types_ranges[ i ].start_pos + 1;

        if ( G_DEVICE_MANAGER->dev_types_ranges[ i ].start_pos == -1 )
            {
            dev_cnt = 0;
            }

        ( ( complex_device* ) G_DEVICE_CMMCTR->dev[ 0 ] )->sub_dev[ i ] =
            new complex_device( 0, device::DEV_NAMES[ i ], dev_cnt,
            device::DEV_TYPES[ i ] );

        complex_device* complex_dev =
            ( ( complex_device* ) ( ( complex_device* ) G_DEVICE_CMMCTR->dev[ 0 ] )->sub_dev[ i ] );

        if ( dev_cnt )
            {
            int pos = 0;
            for ( int j = G_DEVICE_MANAGER->dev_types_ranges[ i ].start_pos;
                j <= G_DEVICE_MANAGER->dev_types_ranges[ i ].end_pos; j++ )
                {
                complex_dev->sub_dev[ pos++ ] =
                    G_DEVICE_MANAGER->project_devices[ j ];
                }
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
project_manager* project_manager::get_instance()
    {
    return instance;
    }
//-----------------------------------------------------------------------------
void project_manager::set_instance( project_manager* new_instance )
    {
    instance = new_instance;
    }
//-----------------------------------------------------------------------------
