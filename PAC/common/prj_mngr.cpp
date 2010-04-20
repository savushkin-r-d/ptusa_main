#include <string.h>

#include "prj_mngr.h"
#include "wago.h"
#include "PAC_dev.h"

project_manager* project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::load_configuration( char *file_name )
    {
#ifdef DEBUG
    Print( "Reading description from %s....\n", file_name );
#endif // DEBUG

    if ( config_file_open( file_name ) != 0 )
    	{
#ifdef DEBUG
        Print( "project_manager:load_configuration(...) - file not found!\n" );            
#endif // DEBUG
        while ( 1 ) { }
    	}

    const *char SIGNATURE            = "WG {4B714C08-9602-4130-8563-4B51E08BB9D7}";
    const int   SIGNATURE_SIZE       = 42;    
    char        id[ SIGNATURE_SIZE ] = { 0 };
    
    config_file_read( id, SIGNATURE_SIZE - 1 );
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

    config_file_read( &editor_version, 2 );
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

    config_file_read( &file_version, 2 );
#ifdef DEBUG
    Print( "Editor version - %d.\n, editor_version" );
    Print( "File version - %d.\n, file_version" );
#endif // DEBUG

    //-Wago data.
    int wago_data_size = 0;
    char *tmp_buff;

    config_file_read( &wago_data_size, 2 );
    if ( wago_data_size )
    	{
        tmp_buff = new char[ wago_data_size ];
        config_file_read( tmp_buff, wago_data_size );
        wago_manager::get_instance()->load_from_stream( tmp_buff );
        delete [] tmp_buff;
    	}

    //-Devices data.
    int dev_data_size = 0;
    config_file_read( &dev_data_size, 2 );
    if ( dev_data_size )
        {
        tmp_buff = new char[ dev_data_size ];
        config_file_read( tmp_buff, dev_data_size );
        device_manager::get_instance()->load_from_stream( tmp_buff );
        delete [] tmp_buff;
        }

    config_file_close();
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


