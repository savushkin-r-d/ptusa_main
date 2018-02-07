#include "PAC_dev.h"
#include "tech_def.h"

#include "errors.h"
#include "lua_manager.h"

auto_smart_ptr < device_manager > device_manager::instance;

std::vector<valve*> valve::to_switch_off;

std::vector<valve_bottom_mix_proof*> valve_bottom_mix_proof::to_switch_off;

const char device::DEV_NAMES[][ 5 ] =
    {
    "V",       ///< Клапан.
    "VC",      ///< Управляемый клапан.
    "M",       ///< Двигатель.
    "LS",      ///< Уровень (есть/нет).
    "TE",      ///< Температура.
    "FS",      ///< Расход (есть/нет).
    "GS",      ///< Датчик положения.
    "FQT",     ///< Счетчик.
    "LT",      ///< Уровень (значение).
    "QT",      ///< Концентрация.

    "HA",      ///< Аварийная звуковая сигнализация.
    "HL",      ///< Аварийная световая сигнализация.
    "SB",      ///< Кнопка.
    "DI",      ///< Дискретный входной сигнал.
    "DO",      ///< Дискретный выходной сигнал.
    "AI",      ///< Аналоговый входной сигнал.
    "AO",      ///< Аналоговый выходной сигнал.
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int par_device::save_device ( char *str )
    {
    str[ 0 ] = 0;

    if ( par == 0 )
        {
        return 0;
        }

    int res = 0;
    for ( u_int i = 0; i < par->get_count(); i++ )
        {
        if ( par_name[ i ] )
            {
            res += sprintf( str + res, "%s=", par_name[ i ] );

            float val =  par[ 0 ][ i + 1 ];
            if ( 0. == val )
                {
                res += sprintf( str + res, "0, " );
                }
            else
                {
                double tmp;
                if ( modf( val, &tmp ) == 0 )
                    {
                    res += sprintf( str + res, "%d, ", ( int ) val );
                    }
                else
                    {
                    res += sprintf( str + res, "%.2f, ", val );
                    }
                }
            }
        }

    return res;
    }
//-----------------------------------------------------------------------------
int par_device::set_cmd( const char *name, double val )
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            if ( strcmp( par_name[ i ], name ) == 0 )
                {
                par->save( i + 1, ( float ) val );
                if ( G_DEBUG )
                    {
                    printf( "par_device::set_cmd() - name = %s, val = %f.\n",
                        name, val );
                    }
                return 0;
                }
            }
        }

    if ( G_DEBUG )
        {
        printf( "par_device::set_cmd() - name = %s wasn't found.\n",
            name );
        }
    return 1;
    }
//-----------------------------------------------------------------------------
void par_device::set_par( u_int idx, u_int offset, float value )
    {
    if ( par )
        {
        par[ 0 ][ offset + idx ] = value;
        }
    }
//-----------------------------------------------------------------------------
par_device::par_device ( u_int par_cnt ) : par ( 0 ),
    par_name ( 0 )
    {
    if ( par_cnt )
        {
        par_name = new char*[ par_cnt ];
        for ( u_int i = 0; i < par_cnt; i++ )
            {
            par_name[ i ] = 0;
            }

        par = new saved_params_float ( par_cnt );
        }
    }
//-----------------------------------------------------------------------------
par_device::~par_device()
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            delete par_name[ i ];
            par_name[ i ] = 0;
            }

        delete par_name;
        par_name = 0;

        delete par;
        par = 0;
        }
    }
//-----------------------------------------------------------------------------
float par_device::get_par( u_int idx, u_int offset )
    {
    if ( par )
        {
        return par[ 0 ][ offset + idx ];
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void par_device::set_par_name( u_int idx, u_int offset, const char* name )
    {
    if ( par )
        {
        if ( offset + idx <= par->get_count() && ( offset + idx > 0 ) )
            {
            if ( strlen( name ) > C_MAX_PAR_NAME_LENGTH )
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name( u_int idx, u_int offset, const char* name ) - "
                        "name length (%d) > param C_MAX_PAR_NAME_LENGTH (%d).",
                        strlen( name ), C_MAX_PAR_NAME_LENGTH );
                    }

                return;
                }

            if ( 0 == par_name[ offset + idx - 1 ] )
                {
                par_name[ offset + idx - 1 ] = new char[ strlen( name ) + 1 ];
                strcpy( par_name[ offset + idx - 1 ], name );
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                        "param (%d %d) already has name (%s).",
                        offset, idx, par_name[ offset + idx - 1 ] );
                    }
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                    "offset (%d) + idx (%d) > param count ( %d ).",
                    offset, idx, par->get_count() );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void device::set_descr( const char *new_description )
    {
    delete[] description;

    //Копирование с учетом нуль-символа.
    int len = strlen( new_description ) + 1;
    description = new char[ len ];
    strcpy( description, new_description );
    }
//-----------------------------------------------------------------------------
void device::print() const
    {
    printf( "%s\t", name );
    }
//-----------------------------------------------------------------------------
void device::off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int device::save_device( char *buff, const char *prefix )
    {
    int res = sprintf( buff, "%s%s={M=%d, ",
        prefix,  name, is_manual_mode );

    if ( type != DT_AO )
        {
        res += sprintf( buff + res, "ST=%d, ", get_state() );
        }

    if ( type != DT_V &&

        type != DT_LS &&
        type != DT_FS &&
        type != DT_GS &&

        type != DT_HA &&
        type != DT_HL &&
        type != DT_SB &&

        type != DT_DI &&
        type != DT_DO )
        {
        if ( get_value() == 0 )
            {
            res += sprintf( buff + res, "V=0, " );
            }
        else
            {
            res += sprintf( buff + res, "V=%.2f, ", get_value() );
            }
        }

    res += save_device_ex( buff + res );
    res += par_device::save_device( buff + res );

    res -= 2;
    buff[ res ] = 0; //Убираем лишнюю последнюю запятую и пробел.//

    res += sprintf( buff + res, "},\n" );

    return res;
    }
//-----------------------------------------------------------------------------
#ifdef RM_PAC
int device::rm_save_device_state( char *buff, const char *prefix )
    {
    int res = 0;

    if ( type != DT_AO &&
        type != DT_TE )
        {
        res += sprintf( buff, "%s.%s.ST=%d\n", prefix,  name, get_state() );
        }

    if ( type != DT_V &&

        type != DT_LS &&
        type != DT_FS &&
        type != DT_GS &&

        type != DT_HA &&
        type != DT_HL &&
        type != DT_SB &&

        type != DT_DI &&
        type != DT_DO )
        {
        if ( get_value() == 0 )
            {
            res += sprintf( buff + res, "%s.%s.V=0\n", prefix, name );
            }
        else
            {
            res += sprintf( buff + res, "%s.%s.V=%.2f\n",
                prefix, name, get_value() );
            }
        }

    return res;
    }
#endif // RM_PAC
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, char *val )
    {
    if ( G_DEBUG )
        {
        printf( "%s\t device::set_cmd() - prop = %s, idx = %d, val = %s\n",
            name, prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        printf( "%s\t device::set_cmd() - prop = %s, idx = %d, val = %f\n",
            name, prop, idx, val );
        }

    switch ( prop[ 0 ] )
        {
        case 'S':
            direct_set_state( ( int ) val );
            break;

        case 'V':
            direct_set_value( ( float ) val );
            break;

        case 'M':
            is_manual_mode = val == 0. ? false : true;
            break;

        case 'P': //Параметры.
            par_device::set_cmd( prop, val );
            break;

        default:
            if ( G_DEBUG )
                {
                printf( "Error device::set_cmd() - prop = %s, val = %f\n",
                    prop, val );
                }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
device::device( const char *dev_name, DEVICE_TYPE type, DEVICE_SUB_TYPE sub_type,
               u_int par_cnt ) :
par_device( par_cnt ),
    type( type ),
    sub_type( sub_type ),
    is_manual_mode( false )
    {
    if ( dev_name )
        {
        strcpy( this->name, dev_name );
        }
    else
        {
        strcpy( this->name, "?" );
        }

    description = new char[ 1 ];
    description[ 0 ] = 0;
    }
//-----------------------------------------------------------------------------
device::~device()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int DO1::get_state()
    {
    return get_DO( DO_INDEX );
    }
//-----------------------------------------------------------------------------
void DO1::direct_on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DO1::direct_off()
    {
    set_DO( DO_INDEX, 0 );
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_V( const char *dev_name )
    {
    return get_device( device::DT_V, dev_name );
    }
//-----------------------------------------------------------------------------
device_manager* device_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new device_manager();
        }
    return instance;
    }
//-----------------------------------------------------------------------------
device* device_manager::get_device( int dev_type,
                                   const char *dev_name )
    {
    int dev_n = get_device_n( ( device::DEVICE_TYPE ) dev_type, dev_name );

    if ( dev_n >= 0 )
        {
        try
            {
            return project_devices[ dev_n ];
            }
        catch (...)
            {
            return &stub;
            }
        }
    else
        {
        if ( G_DEBUG )
            {
            printf( "Error - " );
            if ( dev_type <= device::C_DEVICE_TYPE_CNT )
                {
                printf( "%3s ", device::DEV_NAMES[ dev_type ] );
                }
            else
                {
                printf( "unknown " );
                }

            printf( "\"%s\" not found!\n", dev_name );
            }
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
void device_manager::print() const
    {
    printf( "Device manager [%d]:\n", project_devices.size() );
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        printf( "    %3i. ", i + 1 );
        printf( "%-8s %s",
            project_devices[ i ]->get_name(), project_devices[ i ]->get_description() );
        printf( "\n" );
        }
    }
//-----------------------------------------------------------------------------
device_manager::device_manager(): project_devices( 0 )
    {
    G_DEVICE_CMMCTR->add_device( this );
    }
//-----------------------------------------------------------------------------
device_manager::~device_manager()
    {
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        delete project_devices[ i ];
        }

#ifndef __BORLANDC__
    project_devices.clear();
#endif
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_VC( const char *dev_name )
    {
    return get_device( device::DT_VC, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* device_manager::get_M( const char *dev_name )
    {
    return get_device( device::DT_M, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_LS( const char *dev_name )
    {
    return get_device( device::DT_LS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FS( const char *dev_name )
    {
    return get_device( device::DT_FS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_GS( const char *dev_name )
    {
    return get_device( device::DT_GS, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_AI( const char *dev_name )
    {
    return get_device( device::DT_AI, dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_AO( const char *dev_name )
    {
    return get_device( device::DT_AO, dev_name );
    }
//-----------------------------------------------------------------------------
i_counter* device_manager::get_FQT( const char *dev_name )
    {
    int res = get_device_n( device::DT_FQT, dev_name );
    if ( res > -1 )
        {
        device *res_ctr = project_devices.at( res );
        switch ( res_ctr->get_sub_type() )
            {
            case device::DST_FQT_F:
                return ( counter_f* )res_ctr;

            case device::DST_FQT_F_OK:
                return ( counter_f_ok* )res_ctr;

            case device::DST_FQT:
                return ( counter* )res_ctr;

            default:
                break;
            }
        }

    if ( G_DEBUG )
        {
        printf( "Error - \"%s\" not found!\n", dev_name );
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_TE( const char *dev_name )
    {
    return get_device( device::DT_TE, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_LT( const char *dev_name )
    {
    return get_device( device::DT_LT, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_DI( const char *dev_name )
    {
    return get_device( device::DT_DI, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_SB( const char *dev_name )
    {
    return get_device( device::DT_SB, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_DO( const char *dev_name )
    {
    return get_device( device::DT_DO, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HA( const char *dev_name )
    {
    return get_device( device::DT_HA, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HL( const char *dev_name )
    {
    return get_device( device::DT_HL, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_PT( const char *dev_name )
    {
    return get_device( device::DT_PT, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_QT( const char *dev_name )
    {
    return get_device( device::DT_QT, dev_name );
    }
//-----------------------------------------------------------------------------
wages* device_manager::get_WT( const char *dev_name )
    {
    return (wages*)get_device( device::DT_WT, dev_name );
    }
//-----------------------------------------------------------------------------
wago_device* device_manager::add_wago_device( int dev_type, int dev_sub_type,
                                             const char* dev_name, char * descr )
    {
    static char is_first_device[ device::C_DEVICE_TYPE_CNT ] = { 0 };

    device* new_device = 0;
    wago_device* new_wago_device = 0;

    switch ( dev_type )
        {
        case device::DT_V:
            {
            switch ( dev_sub_type )
                {
                case device::DST_V_DO1:
                    new_device      = new valve_DO1( dev_name );
                    new_wago_device = ( valve_DO1* ) new_device;
                    break;

                case device::DST_V_DO2:
                    new_device      = new valve_DO2( dev_name );
                    new_wago_device = ( valve_DO2* ) new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_OFF:
                    new_device      = new valve_DO1_DI1_off( dev_name );
                    new_wago_device = ( valve_DO1_DI1_off* ) new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_ON:
                    new_device      = new valve_DO1_DI1_on( dev_name );
                    new_wago_device = ( valve_DO1_DI1_on* ) new_device;
                    break;

                case device::DST_V_DO1_DI2:
                    new_device      = new valve_DO1_DI2( dev_name );
                    new_wago_device = ( valve_DO1_DI2* ) new_device;
                    break;

                case device::DST_V_DO2_DI2:
                    new_device      = new valve_DO2_DI2( dev_name );
                    new_wago_device = ( valve_DO2_DI2* ) new_device;
                    break;

                case device::DST_V_MIXPROOF:
                    new_device      = new valve_mix_proof( dev_name );
                    new_wago_device = ( valve_mix_proof* ) new_device;
                    break;

                case device::DST_V_AS_MIXPROOF:
                    new_device      = new valve_AS_mix_proof( dev_name );
                    new_wago_device = ( valve_AS_mix_proof* ) new_device;
                    break;

                case device::DST_V_BOTTOM_MIXPROOF:
                    new_device      = new valve_bottom_mix_proof( dev_name );
                    new_wago_device = ( valve_bottom_mix_proof* ) new_device;
                    break;

                case device::DST_V_AS_DO1_DI2:
                    new_device      = new valve_AS_DO1_DI2( dev_name );
                    new_wago_device = ( valve_AS_DO1_DI2* ) new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown V device subtype %d!\n", dev_sub_type );
                        }
                    new_device      = new dev_stub();
                    break;
                }
            break;
            }

        case device::DT_VC:
            new_device      = new analog_valve( dev_name );
            new_wago_device = ( analog_valve* ) new_device;
            break;

        case device::DT_M:
            new_device      = new motor( dev_name,
                ( device::DEVICE_SUB_TYPE ) dev_sub_type );
            new_wago_device = ( motor* ) new_device;
            break;

        case device::DT_LS:
            new_device      = new level_s( dev_name,
                ( device::DEVICE_SUB_TYPE ) dev_sub_type );
            new_wago_device = ( level_s* ) new_device;
            break;

        case device::DT_TE:
            new_device      = new temperature_e( dev_name );
            new_wago_device = ( temperature_e* ) new_device;
            break;

        case device::DT_FS:
            new_device      = new flow_s( dev_name );
            new_wago_device = ( flow_s* ) new_device;
            break;

        case device::DT_FQT:
            switch ( dev_sub_type )
                {
                case device::DST_FQT:
                    new_device      = new counter( dev_name );
                    new_wago_device = ( counter* ) new_device;
                    break;

                case device::DST_FQT_F:
                    new_device      = new counter_f( dev_name );
                    new_wago_device = ( counter_f* ) new_device;
                    break;

                case device::DST_FQT_F_OK:
                    new_device = new counter_f_ok( dev_name );
                    new_wago_device = (counter_f_ok*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown FQT device subtype %d!\n", dev_sub_type );
                        }
                    new_device      = new dev_stub();
                    break;
                }

            break;

        case device::DT_AO:
            switch (dev_sub_type)
                {
                case device::DST_NONE:
                case device::DST_AO:
                    new_device      = new analog_output( dev_name );
                    new_wago_device = ( analog_output* ) new_device;
                    break;
                case device::DST_AO_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_AO, device::DST_AO_VIRT );
                    break;
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown AO device subtype %d!\n", dev_sub_type );
                        }
                    new_device      = new dev_stub();
                    break;
                }
            break;

        case device::DT_LT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_LT:
                    new_device = new level_e( dev_name );
                    new_wago_device = ( level_e* ) new_device;
                    break;

                case device::DST_LT_CYL:
                    new_device = new level_e_cyl( dev_name );
                    new_wago_device = ( level_e_cyl* ) new_device;
                    break;

                case device::DST_LT_CONE:
                    new_device = new level_e_cone( dev_name );
                    new_wago_device = (level_e_cone*)new_device;
                    break;

                case device::DST_LT_TRUNC:
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown LT device subtype %d!\n", dev_sub_type );
                        }
                    new_device = new dev_stub();
                    break;
                }
            break;

        case device::DT_DI:
            switch (dev_sub_type)
                {
                case device::DST_NONE:
                case device::DST_DI:
                    new_device      = new DI_signal( dev_name );
                    new_wago_device = ( DI_signal* ) new_device;
                    break;
                case device::DST_DI_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_DI, device::DST_DI_VIRT );
                    break;
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown DI device subtype %d!\n", dev_sub_type );
                        }
                    new_device      = new dev_stub();
                    break;
                }
            break;

        case device::DT_DO:
            switch (dev_sub_type)
                {
                case device::DST_NONE:
                case device::DST_DO:
                    new_device      = new DO_signal( dev_name );
                    new_wago_device = ( DO_signal* ) new_device;
                    break;
                case device::DST_DO_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_DO, device::DST_DO_VIRT );
                    break;
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown DO device subtype %d!\n", dev_sub_type );
                        }
                    new_device      = new dev_stub();
                    break;
                }
            break;

        case device::DT_PT:
            new_device      = new pressure_e( dev_name );
            new_wago_device = ( pressure_e* ) new_device;
            break;

        case device::DT_QT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_QT:
                    new_device = new concentration_e( dev_name, device::DST_QT );
                    new_wago_device = (concentration_e*)new_device;
                    break;

                case device::DST_QT_OK:
                    new_device = new concentration_e_ok( dev_name );
                    new_wago_device = (concentration_e_ok*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown QT device subtype %d!\n", dev_sub_type );
                        }
                    new_device = new dev_stub();
                    break;
                }
            break;

        case device::DT_AI:
            switch (dev_sub_type)
                {
                case device::DST_NONE:
                case device::DST_AI:
                    new_device      = new analog_input( dev_name );
                    new_wago_device = ( analog_input* ) new_device;
                    break;
                case device::DST_AI_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_AI, device::DST_AI_VIRT );
                    break;
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown AI device subtype %d!\n", dev_sub_type );
                        }
                    new_device      = new dev_stub();
                    break;
                }
            break;

        case device::DT_HA:
            new_device      = new siren( dev_name );
            new_wago_device = ( siren* ) new_device;
            break;

        case device::DT_HL:
            new_device      = new lamp( dev_name );
            new_wago_device = ( lamp* ) new_device;
            break;

        case device::DT_SB:
            new_device      = new button( dev_name );
            new_wago_device = ( button* ) new_device;
            break;

        case device::DT_GS:
            new_device      = new state_s( dev_name );
            new_wago_device = ( state_s* ) new_device;
            break;

        case device::DT_WT:
            new_device      = new wages( dev_name );
            new_wago_device = ( wages* ) new_device;
            break;

        default:
            if ( G_DEBUG )
                {
                printf( "Unknown device type %d!\n", dev_type );
                }
            new_device = new dev_stub();
            break;
        }

    // Ошибки.
    G_ERRORS_MANAGER->add_error( new tech_dev_error( new_device ) );

    u_int new_dev_index = project_devices.size();
    project_devices.push_back( new_device );
    new_device->set_serial_n( new_dev_index );

    if ( dev_type < device::C_DEVICE_TYPE_CNT )
        {
        if ( 0 == is_first_device[ dev_type ] )
            {
            dev_types_ranges[ dev_type ].start_pos = new_dev_index;
            is_first_device[ dev_type ] = 1;
            }
        dev_types_ranges[ dev_type ].end_pos = new_dev_index;
        }

    return new_wago_device;
    }
//-----------------------------------------------------------------------------
int device_manager::init_params()
    {
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_params", "device_manager::init_params()" );

    return 0;
    }
//-----------------------------------------------------------------------------
int device_manager::save_device( char *buff )
    {
    int res = sprintf( buff, "t=\n" );

    res += sprintf( buff + res, "\t{\n" );

    for ( u_int i = 0; i < project_devices.size(); i++)
        {
        res += project_devices[ i ]->save_device( buff + res, "\t" );
        }

    res += sprintf( buff + res, "\t}\n" );
    return res;
    }
//-----------------------------------------------------------------------------
#ifdef RM_PAC
int device_manager::rm_save_device_state( char *buff )
    {
    int res =
        sprintf( buff, "assert( loadstring( \"%s=%s or{}\"))()\n t=%s\n",
        G_CMMCTR->get_host_name_eng(), G_CMMCTR->get_host_name_eng(),
        G_CMMCTR->get_host_name_eng() );

    for ( u_int i = 0; i < project_devices.size(); i++)
        {
        res +=
            project_devices[ i ]->rm_save_device_state( buff + res, "t" );
        }

    return res;
    }
//-----------------------------------------------------------------------------
int device_manager::rm_save_device( char *buff )
    {
    int answer_size = sprintf( buff, "%s=\n",  G_CMMCTR->get_host_name_eng() );

    answer_size += sprintf( buff + answer_size, "\t{\n" );

    for ( u_int i = 0; i < project_devices.size(); i++)
        {
        answer_size += project_devices[ i ]->save_device( buff + answer_size, "\t" );
        }

    answer_size += sprintf( buff + answer_size, "\t}\n" );

    return answer_size;
    }

#endif // RM_PAC
//-----------------------------------------------------------------------------
int device_manager::get_device_n( device::DEVICE_TYPE dev_type, const char *dev_name )
    {
    int l = -1;
    int u = -1;

    if ( ( int ) dev_type < ( int ) device::C_DEVICE_TYPE_CNT )
        {
        l = dev_types_ranges[ dev_type ].start_pos;
        u = dev_types_ranges[ dev_type ].end_pos;
        }

    if ( -1 == l ) return -1; // Нет устройств.

    while ( l <= u )
        {
        int i = ( l + u ) / 2;

        if ( strcmp( dev_name, project_devices[ i ]->get_name() ) == 0 )
            {
            return i;
            }

        if ( strcmp( dev_name, project_devices[ i ]->get_name() ) > 0 )
            {
            l = i + 1;
            }
        else
            {
            u = i - 1;
            }
        }

    return -1;
    }
//-----------------------------------------------------------------------------
int device_manager::init_rt_params()
    {
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_rt_params", "device_manager::init_rt_params()" );

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void i_counter::restart()
    {
    reset();
    start();
    }
//-----------------------------------------------------------------------------
i_counter::~i_counter()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool i_DI_device::is_active()
    {
    return get_state() == 0 ? 0 : 1;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void i_DO_device::on()
    {
    if ( !get_manual_mode() )
        {
        direct_on();
        }
    }
//-----------------------------------------------------------------------------
void i_DO_device::instant_off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
void i_DO_device::set_state( int new_state )
    {
    if ( !get_manual_mode() )
        {
        direct_set_state( new_state );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Установка текущего состояния устройства с учетом ручного режима.
void i_AO_device::set_value( float new_value )
    {
    if ( !get_manual_mode() )
        {
        direct_set_value( new_value );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float dev_stub::get_value()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_value( float new_value )
    {
    }
//-----------------------------------------------------------------------------
int dev_stub::get_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_off()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_state( int new_state )
    {
    }
//-----------------------------------------------------------------------------
u_int_4 dev_stub::get_serial_n() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::print() const
    {
    printf( "virtual device" );
    }
//-----------------------------------------------------------------------------
void dev_stub::pause()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::start()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::reset()
    {
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_quantity()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float dev_stub::get_flow()
    {
    return 0.;
    }
//-----------------------------------------------------------------------------
void dev_stub::abs_reset()
    {
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_abs_quantity()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float counter::get_value()
    {
    return ( float ) get_quantity();
    }
//-----------------------------------------------------------------------------
void counter::direct_set_value( float new_value )
    {
    value = ( u_int ) new_value;
    }
//-----------------------------------------------------------------------------
int counter::get_state()
    {
    return state;
    }
//-----------------------------------------------------------------------------
void counter::direct_on()
    {
    start();
    }
//-----------------------------------------------------------------------------
void counter::direct_off()
    {
    reset();
    }
//-----------------------------------------------------------------------------
void counter::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ( STATES ) new_state;
#else
    switch ( new_state )
        {
        case S_STOP:
            state = S_STOP;
            reset();
            break;

        case S_WORK:
            start();
            break;

        case S_PAUSE:
            pause();
            break;
        }
#endif
    }
//-----------------------------------------------------------------------------
void counter::print() const
    {
    device::print();
    //wago_device::print();
    }
//-----------------------------------------------------------------------------
void counter::pause()
    {
    get_quantity(); // Пересчитываем значение счетчика.

    state = S_PAUSE;
    }
//-----------------------------------------------------------------------------
void counter::start()
    {
    if ( S_STOP == state || S_PAUSE == state )
        {
        if ( S_STOP == state )
            {
            value = 0;
            }

        state = S_WORK;
        last_read_value = *( ( u_int_2* ) get_AI_data( AI_Q_INDEX ) );
        }
    }
//-----------------------------------------------------------------------------
void counter::reset()
    {
    value = 0;
    }
//-----------------------------------------------------------------------------
void counter::abs_reset()
    {
    abs_value = 0;
    }
//-----------------------------------------------------------------------------
u_int counter::get_abs_quantity()
    {
#ifndef DEBUG_NO_WAGO_MODULES
    u_int current = *( (u_int_2*)get_AI_data( AI_Q_INDEX ) );

    if ( is_first_read_abs )
        {
        if ( current != 0 )
            {
            abs_last_read_value = current;
            is_first_read_abs = false;
            }
        }
    else
        {
        u_int delta;
        if ( current < abs_last_read_value )
            {
            delta = MAX_VAL - abs_last_read_value + current;
            }
        else
            {
            delta = current - abs_last_read_value;
            }

        abs_last_read_value = current;
        if ( delta > 0 )
            {
            abs_value += delta;
            }
        }          
#endif // NO_WAGO_MODULES

    return abs_value;
    }
//-----------------------------------------------------------------------------
u_int counter::get_quantity()
    {
#ifndef DEBUG_NO_WAGO_MODULES
    if ( S_WORK == state )
        {
        u_int current = *( (u_int_2*)get_AI_data( AI_Q_INDEX ) );

        if ( is_first_read )
            {
            if ( current != 0 )
                {
                last_read_value = current;
                is_first_read = false;
                }
            }
        else
            {
            u_int delta;
            if ( current < last_read_value )
                {
                delta = MAX_VAL - last_read_value + current;
                }
            else
                {
                delta = current - last_read_value;
                }

            last_read_value = current;
            if ( delta > 0 )
                {
                value += delta;
                }
            }
        }
#endif // NO_WAGO_MODULES

    return value;
    }
//-----------------------------------------------------------------------------
counter::counter( const char *dev_name, DEVICE_SUB_TYPE sub_type,
                     int extra_par_cnt ):
    device( dev_name, DT_FQT, DST_FQT, extra_par_cnt ),
    wago_device( dev_name ),
    state( S_WORK ),
    value( 0 ),
    last_read_value( 0 ),
    abs_value( 0 ),
    abs_last_read_value( 0 ),
    is_first_read( true ),
    is_first_read_abs( true )
    {
    }
//-----------------------------------------------------------------------------
int counter::set_cmd(const char *prop, u_int idx, double val)
    {
    switch ( prop[ 0 ] )
        {
        case 'A': //ABS_V
            abs_value = ( u_int ) val;
            break;

        default:
            return device::set_cmd( prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter_f::counter_f( const char *dev_name ) :
    counter( dev_name, DST_FQT_F, ADDITIONAL_PARAMS_COUNT ),
    flow_value( 0 )
    {
    set_par_name( P_MIN_FLOW, 0, "P_MIN_FLOW" );
    set_par_name( P_MAX_FLOW, 0, "P_MAX_FLOW" );
    set_par_name( P_CZ, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT" );
    }
//-----------------------------------------------------------------------------
int counter_f::get_state()
    {
    if ( !motors.empty() )
        {
        char           is_pump_working         = 0;
        static u_int_4 start_pump_working_time = 0;
        static u_int_4 counter_prev_value      = 0;

        for ( u_int i = 0; i < motors.size(); i++ )
            {
            if ( motors[ i ]->get_state() == 1 )
                {
                is_pump_working = 1;
                if ( 0 == start_pump_working_time )
                    {
                    start_pump_working_time = get_millisec();
                    counter_prev_value      = get_quantity();
                    }
                }
            }

        if ( 0 == is_pump_working )
            {
            // Насос не работает.
            start_pump_working_time = 0;
            }
        else
            {
            // Насос работает.
            if ( state == S_PAUSE )
                {
                start_pump_working_time = get_millisec();
                }
            else          // Работа.
                {
                state = S_WORK;

                if ( get_delta_millisec( start_pump_working_time ) > get_par( P_DT, 0 ) )
                    {
                    // Проверяем счетчик на ошибку - он должен изменить свои показания.
                    if ( get_quantity() == counter_prev_value )
                        {
                        state = S_ERROR;
                        }
                    else
                        {
                        start_pump_working_time = get_millisec();
                        counter_prev_value      = get_quantity();
                        }
                    }
                }
            }
        }// if ( motors.size() > 0

    if ( get_flow() == -1. )
        {
        return S_LOW_ERR;
        }
    if ( get_flow() == -2. )
        {
        return S_HI_ERR;
        }

    return state;
    }
//-----------------------------------------------------------------------------
float counter_f::get_flow()
    {
    return get_par( P_CZ, 0 ) +
#ifdef DEBUG_NO_WAGO_MODULES
        flow_value;
#else
        get_AI( AI_FLOW_INDEX, get_par( P_MIN_FLOW, 0 ), get_par( P_MAX_FLOW, 0 ) );
#endif // NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void counter_f::set_property( const char* field, device* dev )
    {
    if ( field && field[ 0 ] == 'M' ) //Связанные насосы.
        {
        motors.push_back( dev );
        }
    else
        {
        counter::set_property( field, dev );
        }
    }
//-----------------------------------------------------------------------------
int counter_f::set_cmd(const char *prop, u_int idx, double val)
    {
    switch ( prop[ 0 ] )
        {
        case 'F':
            flow_value = ( float ) val;
            break;

        default:
            return counter::set_cmd( prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int counter_f::save_device_ex( char *buff )
    {
    int res = counter::save_device_ex( buff );

    res += sprintf( buff + res, "F=%.2f, ",
        get_flow() );

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter_f_ok::counter_f_ok( const char *dev_name ) : counter_f( dev_name )
    {
    sub_type = DST_FQT_F_OK;
    }
//-----------------------------------------------------------------------------
int counter_f_ok::save_device_ex( char *buff )
    {
    int res = counter_f::save_device_ex( buff );

#ifdef DEBUG_NO_WAGO_MODULES
    res += sprintf( buff + res, "OK=1, " );
#else
    res += sprintf( buff + res, "OK=%d, ", get_DI( DI_INDEX ) );
#endif //DEBUG_NO_WAGO_MODULES

    return res;
    }
//-----------------------------------------------------------------------------
int counter_f_ok::get_state()
    {
#ifndef DEBUG_NO_WAGO_MODULES
    int i = get_DI( DI_INDEX );

    return i == 1 ? counter_f::get_state() : S_ERROR;
#else
    return counter_f::get_state();
#endif
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float digital_wago_device::get_value()
    {
    return ( float ) get_state();
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_set_value( float new_value )
    {
    direct_set_state( ( int ) new_value );
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = new_state;
#else
    if ( new_state )
        {
        direct_on();
        }
    else direct_off();
#endif //DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void digital_wago_device::print() const
    {
    device::print();
    //wago_device::print();
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES
int digital_wago_device::get_state()
    {
    return state;
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_on()
    {
    state = 1;
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_off()
    {
    state = 0;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
digital_wago_device::digital_wago_device( const char *dev_name, device::DEVICE_TYPE type,
                                         device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    device( dev_name, type, sub_type, par_cnt ),
    wago_device( dev_name )
#ifdef DEBUG_NO_WAGO_MODULES
    , state( 0 )
#endif // DEBUG_NO_WAGO_MODULES
    {
    }
//-----------------------------------------------------------------------------
digital_wago_device::~digital_wago_device()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int DO2::get_state()
    {
    int b1 = get_DO( DO_INDEX_1 );
    int b2 = get_DO( DO_INDEX_2 );
    if ( b1 == b2 ) return -1;
    return b2;
    }
//-----------------------------------------------------------------------------
void DO2::direct_on()
    {
    set_DO( DO_INDEX_1, 0 );
    set_DO( DO_INDEX_2, 1 );
    }
//-----------------------------------------------------------------------------
void DO2::direct_off()
    {
    set_DO( DO_INDEX_1, 1 );
    set_DO( DO_INDEX_2, 0 );
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve::valve( bool is_on_fb, bool is_off_fb, const char *dev_name,
             device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type ) :
digital_wago_device( dev_name, type, sub_type, ADDITIONAL_PARAMS_COUNT ),
    is_on_fb( is_on_fb ),
    is_off_fb( is_off_fb ),
    on_fb( true ),
    off_fb( true ),
    was_on_auto( false ),
    start_switch_time( get_millisec() )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    set_par_name( P_FB,  0, "P_FB" );
    }
//-----------------------------------------------------------------------------
valve::valve( const char *dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type ) :
    digital_wago_device( dev_name, type, sub_type, 0 ),
    is_on_fb( false ),
    is_off_fb( false ),
    was_on_auto( false ),
    start_switch_time( get_millisec() )
    {
    }
//-----------------------------------------------------------------------------
int valve::save_device_ex( char *buff )
    {
    int res = 0;
    if ( is_on_fb )
        {
        res += sprintf( buff, "FB_ON_ST=%d, ", get_on_fb_value() );
        }

    if ( is_off_fb )
        {
        res += sprintf( buff + res, "FB_OFF_ST=%d, ", get_off_fb_value() );
        }
    return res;
    }
//-----------------------------------------------------------------------------
int valve::get_state()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    return digital_wago_device::get_state();
#else

    switch ( get_valve_state() )
        {
        case V_LOWER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_LOWER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_LOWER_SEAT_FB_OFF;
                }

            return VX_LOWER_SEAT;

        case V_UPPER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_UPPER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_UPPER_SEAT_FB_OFF;
                }

            return VX_UPPER_SEAT;

        case V_ON:
            if ( is_off_fb || is_on_fb ) //Обратная связь есть.
                {
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF ) //Обратная связь отключена.
                    {
                    //start_switch_time = get_millisec();

                    if ( get_manual_mode() )
                        {
                        return VX_ON_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_ON_FB_OFF;
                        }
                    }
                else //Обратная связь включена.
                    {
                    if ( get_manual_mode() ) //Ручной режим включен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_ON_FB_OK_MANUAL;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_ON_FB_ERR_MANUAL;
                                }
                            else
                                {
                                return VX_ON_FB_OK_MANUAL;
                                }
                            }
                        } // if ( get_manual_mode() )
                    else  //Ручной режим отключен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_ON_FB_OK;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_ON_FB_ERR;
                                }
                            else
                                {
                                return VX_ON_FB_OK;
                                }
                            }
                        }
                    }
                }//if ( is_off_fb || is_on_fb ) //Обратная связь есть.
            else //Обратной связи нет.
                {
                if ( get_manual_mode() )
                    {
                    return VX_ON_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_ON_FB_OK;
                    }
                }
            break;

        case V_OFF:
            if ( is_off_fb || is_on_fb ) //Обратная связь есть.
                {
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF ) //Обратная связь отключена.
                    {
                    //start_switch_time = get_millisec();

                    if ( get_manual_mode() )
                        {
                        return VX_OFF_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_OFF_FB_OFF;
                        }
                    }
                else //Обратная связь включена.
                    {
                    if ( get_manual_mode() ) //Ручной режим включен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_OFF_FB_OK_MANUAL;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_OFF_FB_ERR_MANUAL;
                                }
                            else
                                {
                                return VX_OFF_FB_OK_MANUAL;
                                }
                            }
                        } // if ( get_manual_mode() )
                    else  //Ручной режим отключен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_OFF_FB_OK;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_OFF_FB_ERR;
                                }
                            else
                                {
                                return VX_OFF_FB_OK;
                                }
                            }
                        }
                    }
                }//if ( is_off_fb || is_on_fb ) //Обратная связь есть.
            else //Обратной связи нет.
                {
                if ( get_manual_mode() )
                    {
                    return VX_OFF_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_OFF_FB_OK;
                    }
                }
            break;
        }

    return VX_UNKNOWN;

#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES
int valve::set_cmd( const char *prop, u_int idx, double val )
    {
    printf( "valve::set_cmd() - prop = %s, idx = %d, val = %f\n",
        prop, idx, val );

    switch ( prop[ 0 ] )
        {
        case 'F':
            if ( strcmp( prop, "FB_ON_ST" ) == 0 )
                {
                on_fb = val == .0;
                }
            else
                {
                off_fb = val == .0;
                }
            break;

        default:
            device::set_cmd( prop, idx, val );
        }

    return 0;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
void valve::evaluate()
    {
    if ( to_switch_off.empty() )
        {
        return;
        }

    u_int delay = G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ];

    for( std::vector< valve* >::iterator iter = to_switch_off.begin();
            iter != to_switch_off.end(); iter++ )
        {
        valve* v = *iter;

        if ( v->is_switching_off &&
            get_delta_millisec( v->start_off_time ) > delay )
            {
            if ( !v->get_manual_mode() )
                {
                v->direct_off();
                }

            v->is_switching_off = false;
            v->was_on_auto = false;
            }
        }

    to_switch_off.erase(
        std::remove_if( to_switch_off.begin(), to_switch_off.end(),
        is_switching_off_finished ), to_switch_off.end() );
    }
//-----------------------------------------------------------------------------
void valve::off()
    {
    if ( false == was_on_auto ||                //Если был включен вручную.
        get_valve_state() == V_UPPER_SEAT ||
        get_valve_state() == V_LOWER_SEAT )
        {
        if ( !get_manual_mode() )
            {
            direct_off();
            }

        return;
        }

    if ( false == is_switching_off )
        {
        is_switching_off = true;
        start_off_time = get_millisec();

        to_switch_off.push_back( this );
        }
    }
//-----------------------------------------------------------------------------
void valve::on()
    {
    was_on_auto = true;
    is_switching_off = false;
    digital_wago_device::on();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1_DI1_off::valve_DO1_DI1_off( const char *dev_name ) :
    valve( false, true, dev_name, DT_V, DST_V_DO1_DI1_FB_OFF )
    {
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES
void valve_DO1_DI1_off::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_off::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES
void valve_DO1_DI1_on::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_on::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_DO1_DI2::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI2::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_DO2_DI2::direct_on()
    {
    int o = get_DO( DO_INDEX_1 );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_1, 1 );
        set_DO( DO_INDEX_2, 0 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2::direct_off()
    {
    int o = get_DO( DO_INDEX_2 );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_1, 0 );
        set_DO( DO_INDEX_2, 1 );
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void valve_mix_proof::open_upper_seat()
    {
    direct_set_state( V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_lower_seat()
    {
    direct_set_state( V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ( char ) new_state;
#else
    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            direct_off();

            int u = get_DO( DO_INDEX_U );
            if( 0 == u )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_U, 1 );
                }

            break;
            }

        case V_LOWER_SEAT:
            {
            direct_off();

            int l = get_DO( DO_INDEX_L );
            if ( 0 == l )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_L, 1 );
                }
            break;
            }

        default:
            direct_on();
            break;
        }
#endif //DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_mix_proof::direct_on()
    {
    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_off()
    {
    VALVE_STATE st = get_valve_state();
    bool was_seat = st == V_LOWER_SEAT || st == V_UPPER_SEAT;

    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( o != 0 || was_seat )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Определение завершения отключения клапана с задержкой.
bool valve_bottom_mix_proof::is_switching_off_finished(
    valve_bottom_mix_proof *v )
    {
    //Если открыли клапан раньше завершения закрытия, то его можно удалять из
    //вектора.
    if ( v->get_valve_state() == V_ON )
        {
        return true;
        }

    //Если сняли флаг закрытия, то удаляем из вектора
    if (!v->is_closing_mini)
        {
        return true;
        }

    u_int delay = G_PAC_INFO()->par[ PAC_info::P_V_BOTTOM_OFF_DELAY_TIME ];

    //Если завершилось время задержки, выключаем мини клапан перед удалением
    //клапана из вектора.
    if ( get_delta_millisec( v->start_off_time ) > delay )
        {
        v->set_DO( DO_INDEX_MINI_V, 0 );
        v->is_closing_mini = 0;
        return true;
        }

    return false;
    };
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::evaluate()
    {
    if ( to_switch_off.empty() )
        {
        return;
        }

    to_switch_off.erase(
        std::remove_if( to_switch_off.begin(), to_switch_off.end(),
        is_switching_off_finished ), to_switch_off.end() );
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES
void valve_bottom_mix_proof::direct_on()
    {
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );

        set_DO( DO_INDEX_MINI_V, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::direct_off()
    {
    VALVE_STATE st = get_valve_state();
    bool was_seat = st == V_LOWER_SEAT;
    bool was_mini = st == V_UPPER_SEAT;
    int o = get_DO( DO_INDEX );

    if ( was_seat )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_L, 0 );
        }

    if ( was_mini && !is_closing_mini )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_MINI_V, 0 );
        }

    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        start_off_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        is_closing_mini = 1;

        to_switch_off.push_back( this );
        }
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void DI1::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void DI1::direct_off()
    {
    }
//-----------------------------------------------------------------------------
int DI1::get_state()
    {
    u_int_4 dt = ( u_int_4 ) get_par( P_DT, 0 );

    if ( dt > 0 )
        {
        if ( current_state != get_DI( DI_INDEX ) )
            {
            if ( get_delta_millisec( time ) > dt )
                {
                current_state = get_DI( DI_INDEX );
                time = get_millisec();
                }
            }
        else
            {
            time = get_millisec();
            }
        }
    else current_state = get_DI( DI_INDEX );

    return current_state;
    }
//-----------------------------------------------------------------------------
#else
int DI1::get_state()
    {
    u_int_4 dt = ( u_int_4 ) get_par( P_DT, 0 );

    if ( dt > 0 )
        {
        if ( current_state != digital_wago_device::get_state() )
            {
            if ( get_delta_millisec( time ) > dt  )
                {
                current_state = digital_wago_device::get_state();
                time = get_millisec();
                }
            }
        else
            {
            time = get_millisec();
            }
        }
    else current_state = digital_wago_device::get_state();

    return current_state;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AI1::AI1( const char *dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt,
    u_int *start_par_idx ) : analog_wago_device( dev_name, type, sub_type,
        par_cnt + ADDITIONAL_PARAM_COUNT )
#ifdef DEBUG_NO_WAGO_MODULES
        ,st( 1 )
#endif
    {
    if ( start_par_idx )
        {
        *start_par_idx = ADDITIONAL_PARAM_COUNT;
        }

    set_par_name( P_ZERO_ADJUST_COEFF, 0, "P_CZ" );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

float AI1::get_value()
    {
    return get_par( P_ZERO_ADJUST_COEFF, 0 ) + analog_wago_device::get_value();
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float AI1::get_value()
    {
    return get_par( P_ZERO_ADJUST_COEFF, 0 ) +
        get_AI( C_AI_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AI1::direct_set_value( float new_value )
    {
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wages::wages( const char *dev_name ) : analog_wago_device(
    dev_name, DT_WT, DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    set_par_name( P_NOMINAL_W,  0, "P_NOMINAL_W" );
    set_par_name( P_RKP, 0, "P_RKP");
    set_par_name( P_C0, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT");
    weight = 0;
    filter_time = get_millisec();
    }
//-----------------------------------------------------------------------------
void wages::tare()
    {
    set_par(P_C0, 0, -weight);
    return;
    }

float wages::get_weight()
    {
    if (get_delta_millisec(filter_time) > 500)
        {
        filter_time = get_millisec();
        float rkp = get_par(P_RKP, 0);
        if (0 == rkp) return -1001;
        float uref = get_AI(C_AI_Uref);
        if (0 == uref) return -1002;
        float filterval = get_par(P_DT, 0);
        float now_weight = get_AI(C_AI_Ud) / rkp / uref * get_par(P_NOMINAL_W, 0);
        if (fabs(now_weight - weight) > filterval)
            {
            weight = now_weight;
            }
        if (filterval >= 1)
            {
            weight = ceilf(weight);
            }
        }
    return weight + get_par(P_C0, 0);
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

float wages::get_value()
    {
    return weight + get_par(P_C0, 0);
    }
//-----------------------------------------------------------------------------
void wages::direct_set_value( float new_value )
    {
    weight = new_value;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float wages::get_value()
    {
    return get_weight();
    }

void wages::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case S_TARE:
            tare();
            break;
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float AO1::get_value()
    {
    return get_AO( AO_INDEX, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
void AO1::direct_set_value( float new_value )
    {
    set_AO( AO_INDEX, new_value, get_min_value(), get_max_value() );
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float level_e::get_max_val()
    {
    return 100;
    }
//-----------------------------------------------------------------------------
float level_e::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float level_e_cyl::get_max_val()
    {
    return 100;
    }
//-----------------------------------------------------------------------------
float level_e_cyl::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int level_e_cyl::save_device_ex( char *buff )
    {
    int res = 0;

    float v = get_par( P_R, start_param_idx );
    v = (float) M_PI * v * v *  AI1::get_value() *
        get_par( P_MAX_P, start_param_idx ) / 9.81f;

    res = sprintf( buff, "CLEVEL=%.2f, ", v );

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float level_e_cone::get_max_val()
    {
    return 100;
    }
//-----------------------------------------------------------------------------
float level_e_cone::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int level_e_cone::save_device_ex( char *buff )
    {
    int res = 0;

    float r = get_par( P_R, start_param_idx );
    float tg_a = 0;
    if ( get_par( P_H_CONE, start_param_idx ) > 0 )
        {
        tg_a = r / get_par( P_H_CONE, start_param_idx );
        }

    float h_cone = get_par( P_H_CONE, start_param_idx );
    float h_curr = get_par( P_MAX_P, start_param_idx ) * AI1::get_value() / 9.81f;

    float v = 0;
    if ( h_curr <= h_cone )
        {
        v = (float) M_PI * h_curr * tg_a * h_curr * tg_a * h_curr / 3;
        }
    else
        {
        v = (float) M_PI * r * r * ( h_curr - h_cone * 2 / 3 );
        }
    
    res = sprintf( buff, "CLEVEL=%.2f, ", v );

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_DO1::direct_on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_off()
    {
    set_DO( DO_INDEX, 0 );
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float motor::get_value()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    return freq;
#else
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        return get_AO( AO_INDEX, C_MIN_VALUE, C_MAX_VALUE );
        }

    return 0;
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_set_value( float value )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    freq = value;
#else
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        set_AO( AO_INDEX, value, C_MIN_VALUE, C_MAX_VALUE );
        }
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    if ( -1 == new_state )
        {
        state = ( char ) -1;
        return;
        }
#endif // DEBUG_NO_WAGO_MODULES

    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
        {
        if ( new_state == 2 )
            {
#ifdef DEBUG_NO_WAGO_MODULES
            state = 2;
#else
            // Включение прямого пуска.
            int o = get_DO( DO_INDEX );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX, 1 );
                }

            // Включение реверса.
            o = get_DO( DO_INDEX_REVERSE );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_REVERSE, 1 );
                }
#endif // DEBUG_NO_WAGO_MODULES

            return;
            }
        }

    if ( sub_type == device::DST_M_REV_2 ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( new_state == 2 )
            {
#ifdef DEBUG_NO_WAGO_MODULES
            state = 2;
#else
            // Выключение прямого пуска.
            int o = get_DO( DO_INDEX );
            if ( 1 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX, 0 );
                }

            // Включение реверса.
            o = get_DO( DO_INDEX_REVERSE );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_REVERSE, 1 );
                }
#endif // DEBUG_NO_WAGO_MODULES

            return;
            }
        }

    if ( new_state )
        {
        direct_on();
        }
    else
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int motor::get_state()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    return state;
#else
    int o = get_DO( DO_INDEX );

    if ( sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        int err = get_DI( DI_INDEX_ERROR );

        if ( 1 == err )
            {
            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                {
                return -1;
                }
            }
        else
            {
            start_switch_time = get_millisec();
            }

        int ro = get_DO( DO_INDEX_REVERSE );
        if ( 1 == ro )
            {
            return 2;
            }

        if ( 1 == o )
            {
            return 1;
            }

        return 0;
        }

    int i = get_DI( DI_INDEX );

    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 )
        {
        int ro = get_DO( DO_INDEX_REVERSE );

        if ( 0 == ro && 0 == o && 0 == i )
            {
            start_switch_time = get_millisec();
            return 0;
            }

        if ( 0 == ro && 1 == o && 1 == i )
            {
            start_switch_time = get_millisec();
            return 1;
            }

        if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
            {
            if ( 1 == ro && 1 == o && 1 == i )
                {
                start_switch_time = get_millisec();
                return 2;
                }
            }

        if ( sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 )
            {
            if ( 1 == ro && 0 == o && 1 == i )
                {
                start_switch_time = get_millisec();
                return 2;
                }
            }
        }
    else
        {
        if ( o == i )
            {
            start_switch_time = get_millisec();
            return i;
            }
        }

    if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
        {
        return -1;
        }
    else
        {
        return i;
        }
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_on()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = 1;
#else
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        // Выключение реверса.
        int o = get_DO( DO_INDEX_REVERSE );
        if ( 0 != o )
            {
            start_switch_time = get_millisec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }

    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_off()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = 0;
#else
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }

    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        // Отключение реверса.
        o = get_DO( DO_INDEX_REVERSE );
        if ( o != 0 )
            {
            start_switch_time = get_millisec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }
#endif // DEBUG_NO_WAGO_MODULES

    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
int motor::save_device_ex( char *buff )
    {
    int res = 0;
#ifdef DEBUG_NO_WAGO_MODULES
    res = sprintf( buff, "R=0, " );
#else
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( sub_type == device::M_REV_2_ERROR ||
            sub_type == device::DST_M_REV_FREQ_2_ERROR )
            {
            res = sprintf( buff, "R=%d, ERR=%d, ",
                get_DO( DO_INDEX_REVERSE ), get_DI( DI_INDEX_ERROR ) );
            }
        else
            {
            res = sprintf( buff, "R=%d,", get_DO( DO_INDEX_REVERSE ) );
            }
        }
#endif //DEBUG_NO_WAGO_MODULES
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool level_s::is_active()
    {
    switch ( sub_type )
        {
        case DST_LS_MIN:
            return get_state() == 0 ? 0 : 1;
            break;

        case DST_LS_MAX:
            return get_state() == 0 ? 1 : 0;
            break;

        default:
            return get_state() == 0 ? 0 : 1;
        }
    }
//-----------------------------------------------------------------------------
level_s::level_s( const char *dev_name, device::DEVICE_SUB_TYPE sub_type ):
    DI1( dev_name, DT_LS, sub_type, 0,
        sub_type == DEVICE_SUB_TYPE::DST_LS_MAX ? 1 : 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float pressure_e::get_max_val()
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float pressure_e::get_min_val()
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float concentration_e::get_max_val()
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float concentration_e::get_min_val()
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float analog_input::get_max_val()
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float analog_input::get_min_val()
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void analog_wago_device::direct_set_state( int new_state )
    {
    direct_set_value( ( float ) new_state );
    }
//-----------------------------------------------------------------------------
int analog_wago_device::get_state()
    {
    return ( int ) get_value();
    }
//-----------------------------------------------------------------------------
void analog_wago_device::print() const
    {
    device::print();
    //wago_device::print();
    }
//-----------------------------------------------------------------------------
void analog_wago_device::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void analog_wago_device::direct_off()
    {
    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

float analog_wago_device::get_value()
    {
    return value;
    }
//-----------------------------------------------------------------------------
void analog_wago_device::direct_set_value( float new_value )
    {
    value = new_value;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int timer::save( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int timer::load( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int timer::get_saved_size() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
timer::timer(): last_time( 0 ),
    work_time( 0 ),
    state( S_STOP ),
    countdown_time( 0 )
    {
    }
//-----------------------------------------------------------------------------
void timer::start()
    {
    if ( S_STOP == state )
        {
        work_time = 0;
        }

    if ( S_PAUSE == state || S_STOP == state )
        {
        state = S_WORK;
        last_time = get_millisec();
        }
    }
//-----------------------------------------------------------------------------
void timer::reset()
    {
    state = S_STOP;
    work_time = 0;
    }
//-----------------------------------------------------------------------------
void timer::pause()
    {
    if ( S_WORK == state )
        {
        work_time += get_delta_millisec( last_time );
        }
    state = S_PAUSE;
    }
//-----------------------------------------------------------------------------
bool timer::is_time_up() const
    {
    if ( S_WORK == state )
        {
        u_int time = work_time + get_delta_millisec( last_time );
        if (  time <= countdown_time )
            {
            return 0;
            }
        else
            {
            return 1;
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
u_long timer::get_work_time() const
    {
    if (S_WORK == state)
        {
        return work_time + get_delta_millisec( last_time );
        }
    else
        {
        return work_time;
        }
    }
//-----------------------------------------------------------------------------
void timer::set_countdown_time( u_long new_countdown_time )
    {
    if ( G_DEBUG )
        {
        if ( 0 == new_countdown_time )
            {
            printf( "Error void timer::set_countdown_time( u_long time ), time = %lu!\n",
                new_countdown_time );
            }
        }

    countdown_time = new_countdown_time;
    }
//-----------------------------------------------------------------------------
u_long timer::get_countdown_time() const
    {
    return countdown_time;
    }
//-----------------------------------------------------------------------------
timer::STATE timer::get_state() const
    {
    return state;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
timer_manager::timer_manager( u_int timers_count ) : timers_cnt( timers_count ),
    timers( 0 )
    {
    if ( timers_cnt )
        {
        timers = new timer[ timers_cnt ];
        }
    }
//-----------------------------------------------------------------------------
timer_manager::~timer_manager()
    {
    if ( timers )
        {
        delete [] timers;
        timers     = 0;
        timers_cnt = 0;
        }
    }
//-----------------------------------------------------------------------------
timer* timer_manager::operator[]( unsigned int index )
    {
    index -= 1;

    if ( index < timers_cnt )
        {
        return &timers[ index ];
        }
    else
        {
        if ( G_DEBUG )
            {
            printf( "timer_manager[] - error: index[ %u ] > count [ %u ]\n",
                index, timers_cnt );
            }
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device_manager* G_DEVICE_MANAGER()
    {
    return device_manager::get_instance();
    }
//-----------------------------------------------------------------------------
i_DO_device* V( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "V%d", dev_n );

    return G_DEVICE_MANAGER()->get_V( name );
    }

i_DO_device* V( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_V( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* VC( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_VC( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* M( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "M%d", dev_n );

    return G_DEVICE_MANAGER()->get_M( name );
    }

i_DO_AO_device* M( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_M( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* LS( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "LS%d", dev_n );

    return G_DEVICE_MANAGER()->get_LS( name );
    }

i_DI_device* LS( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_LS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* FS( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "FS%d", dev_n );

    return G_DEVICE_MANAGER()->get_FS( name );
    }

i_DI_device* FS( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_FS( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* AI( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_AI( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* AO( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "AO%d", dev_n );

    return G_DEVICE_MANAGER()->get_AO( name );
    }

i_AO_device* AO( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_AO( dev_name );
    }
//-----------------------------------------------------------------------------
i_counter* FQT( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "FQT%d", dev_n );

    return G_DEVICE_MANAGER()->get_FQT( name );
    }

i_counter* FQT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_FQT( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* TE( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "TE%d", dev_n );

    return G_DEVICE_MANAGER()->get_TE( name );
    }

i_AI_device* TE( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_TE( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* LT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_LT( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* GS( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_GS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* HA( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_HA( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* HL( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_HL( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* SB( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_SB( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* DI( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_DI( dev_name );
    }

i_DI_device* DI( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "DI%d", dev_n );
    return G_DEVICE_MANAGER()->get_DI( name );
    }

//-----------------------------------------------------------------------------
i_DO_device* DO( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_DO( dev_name );
    }

i_DO_device* DO( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "DO%d", dev_n );
    return G_DEVICE_MANAGER()->get_DO( name );
    }
//-----------------------------------------------------------------------------
i_AI_device* QT( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "QT%d", dev_n );

    return G_DEVICE_MANAGER()->get_QT( name );
    }

i_AI_device* QT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_QT( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* PT( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "PT%d", dev_n );

    return G_DEVICE_MANAGER()->get_PT( name );
    }

i_AI_device* PT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_PT( dev_name );
    }
//-----------------------------------------------------------------------------
wages* WT( u_int dev_n )
    {
    static char name[ 20 ] = { 0 };
    snprintf( name, sizeof( name ), "WT%d", dev_n );

    return G_DEVICE_MANAGER()->get_WT( name );
    }

wages* WT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_WT( dev_name );
    }
//-----------------------------------------------------------------------------
dev_stub* STUB()
    {
    return G_DEVICE_MANAGER()->get_stub();
    }
//-----------------------------------------------------------------------------
device* DEVICE( int s_number )
    {
    return G_DEVICE_MANAGER()->get_device( s_number );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS::valve_AS( const char *dev_name, DEVICE_SUB_TYPE sub_type ):
    valve( true, true, dev_name, DT_V, sub_type ),
    AS_number( 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_mix_proof::valve_AS_mix_proof( const char *dev_name ):
    valve_AS( dev_name, DST_V_AS_MIXPROOF )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_DO1_DI2::valve_AS_DO1_DI2( const char *dev_name ):
    valve_AS( dev_name, DST_V_AS_DO1_DI2 )
    {
    }
//---------------------------------------------------------------------------
void valve_AS_DO1_DI2::direct_set_state(int new_state)
    {
    switch ( new_state )
        {
        case V_UPPER_SEAT:
        case V_LOWER_SEAT:
            //Ничего не делаем, так как нет седел.
            break;

        default:
            valve_AS::direct_set_state( new_state );
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void virtual_device::direct_off()
    {
    state = 0;
    }

void virtual_device::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_device::get_value()
    {
    return value;
    }

void virtual_device::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_device::direct_on()
    {
    state = 1;
    }

int virtual_device::get_state()
    {
    return state;
    }

virtual_device::virtual_device( const char *dev_name,
    device::DEVICE_TYPE dev_type,
    device::DEVICE_SUB_TYPE dev_sub_type ) : device (dev_name, dev_type, dev_sub_type, 0)
    {
    value = 0;
    state = 0;
    }
