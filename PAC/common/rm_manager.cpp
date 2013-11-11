#include "rm_manager.h"

#ifdef WIN_OS
#include "w_rm_tech_def.h"
#endif // WIN_OS

rm_manager* rm_manager::instance = new rm_manager();
//-----------------------------------------------------------------------------
void rm_manager::add_rm_cmmctr( char* name, char* IP_address, int remote_PAC_id )
    {
    i_rm_cmmctr* cmmctr = nullptr;

#ifdef WIN_OS
    cmmctr = new win_rm_cmmctr( name, IP_address, remote_PAC_id );
#endif // WIN_OS

    if ( cmmctr ) 
        {
        cmmctrs.push_back( cmmctr );          
        }
    }
//-----------------------------------------------------------------------------
int rm_manager::evaluate()
    {
    for ( u_int i = 0; i < cmmctrs.size(); i++ )
        {
        cmmctrs[ i ]->evaluate();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void rm_manager::print() const
    {
    Print( "Remote PAC manager (%d) \n", cmmctrs.size() );
    for ( u_int  i = 0; i < cmmctrs.size(); i++ )
        {
        Print( "  %d. ", i + 1 );
        cmmctrs[ i ]->print();                
        }
    }
//-----------------------------------------------------------------------------
rm_manager* G_RM_MANAGER()
    {
    return rm_manager::get_instance();
    }
//-----------------------------------------------------------------------------
