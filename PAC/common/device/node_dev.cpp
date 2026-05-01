#include <fmt/core.h>

#include "node_dev.h"
#include "bus_coupler_io.h"
#include "log.h"

#include <cstdlib>

#ifdef LINUX_OS
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>

#include <sys/wait.h>
#else
#include <ws2tcpip.h>
#endif


#ifdef LINUX_OS
namespace
    {
    std::string to_ipv4_string( const sockaddr_in* sa )
        {
        char buff[ INET_ADDRSTRLEN ]{};
        inet_ntop( AF_INET, &sa->sin_addr, buff, sizeof( buff ) );
        return buff;
        }
    }
#endif
//-----------------------------------------------------------------------------
std::string node_dev::get_local_ipv4()
    {
    std::string result;

#ifdef LINUX_OS
    ifaddrs* ifaddr = nullptr;
    if ( getifaddrs( &ifaddr ) != 0 )
        {
        return {};
        }

    for ( auto* it = ifaddr; it != nullptr; it = it->ifa_next )
        {
        if ( it->ifa_addr == nullptr )
            {
            continue;
            }

        if ( it->ifa_addr->sa_family != AF_INET )
            {
            continue;
            }

        if ( ( it->ifa_flags & IFF_LOOPBACK ) != 0 )
            {
            continue;
            }

        result = to_ipv4_string(
            reinterpret_cast<sockaddr_in*>( it->ifa_addr ) );
        if ( !result.empty() )
            {
            break;
            }
        }

    freeifaddrs( ifaddr );
#endif

    return result;
    }
//-----------------------------------------------------------------------------
node_dev::node_dev( const char* name ) : device( name,
    device::DEVICE_TYPE::DT_NODE, device::DEVICE_SUB_TYPE::DST_NODE, 0 )
    {}
//-----------------------------------------------------------------------------
static std::string replace_action( const std::string& cmd,
    std::string_view from, std::string_view to )
    {
    std::string out{ cmd };
    
    if ( const auto pos = out.find( from ); pos != std::string::npos )
        {
        out.replace( pos, from.length(), to );
        }
    return out;
    }
//-----------------------------------------------------------------------------
void node_dev::set_io_node( io_manager::io_node* io_node )
    {
    if ( !io_node )
        {
        G_LOG->warning( "Null pointer passed as io_node for node '%s'.",
            get_name() );
        return;
        }

    // Check IPv4.    
    if ( struct in_addr ipv4_addr;
        inet_pton( AF_INET, io_node->ip_address, &ipv4_addr ) != 1 )
        {
        G_LOG->warning( "Invalid IPv4 address ('%s') for node '%s'.",
            io_node->ip_address, get_name() );
        return;
        }

    node = io_node;

    port_controller_web = EXTERNAL_WEB_PORT_BASE + node->number;
    ip_controller = get_local_ipv4();

#ifdef LINUX_OS
    if ( ip_controller.empty() )
        {
        G_LOG->warning( "Controller IPv4 address was not detected." );
        return;
        }

    const std::string IPTABLES = "sudo -n /usr/sbin/iptables";
    dnat = IPTABLES + " -t nat -A PREROUTING -p tcp -d " + ip_controller +
        " --dport " + std::to_string( port_controller_web ) +
        " -j DNAT --to-destination " + node->ip_address + ":" + PORT_NODE_WEB;

    forward_in = IPTABLES + " -A FORWARD -p tcp -d " +
        node->ip_address + " --dport " + PORT_NODE_WEB + " -j ACCEPT";

    masq = IPTABLES + " -t nat -A POSTROUTING -p tcp -d " +
        node->ip_address + " --dport " + PORT_NODE_WEB + " -j MASQUERADE";
    
    dnat_delete = replace_action( dnat, " -A ", " -D " );
    forward_in_delete = replace_action( forward_in, " -A ", " -D " );
    masq_delete = replace_action( masq, " -A ", " -D " );

    dnat_check = replace_action( dnat, " -A ", " -C " );
    forward_in_check = replace_action( forward_in, " -A ", " -C " );
    masq_check = replace_action( masq, " -A ", " -C " );

    // Для отладки. Просмотр входящих правил.
    // sudo /usr/sbin/iptables -t nat -S; sudo /usr/sbin/iptables -S FORWARD
    // 
    // Это команды просмотра текущих правил, без изменения конфигурации.
    //    •	sudo /usr/sbin/iptables -t nat -S
    //    Показать все правила таблицы nat в “скриптовом” виде ( -A ... ):
    //      • PREROUTING (обычно DNAT),
    //      • POSTROUTING (обычно SNAT/MASQUERADE),
    //      • OUTPUT (локально сгенерированный трафик).
    // 
    //    • sudo /usr/sbin/iptables -S FORWARD
    //    Показать правила только цепочки FORWARD (таблица filter по
    //    умолчанию), то есть что разрешено/запрещено для транзитного трафика
    //    между интерфейсами.

#endif // LINUX_OS
    }
//-----------------------------------------------------------------------------
void node_dev::evaluate_io()
    {
    // Обновление состояния узла.
    if ( node )
        {
        // Обновление состояния на основе состояния узла.
        device::direct_set_state( 
            static_cast<int>( node->get_display_state() ) );
        }
    else
        {
        device::direct_set_state( -2 );
        }
    }
//-----------------------------------------------------------------------------
int node_dev::run_cmd_exit_code( std::string_view cmd
#ifdef PTUSA_TEST
    , [[maybe_unused]] int expected
#endif
)
    {
    const int rc = std::system( cmd.data() );
    if ( rc == -1 ) return -1;
#ifdef LINUX_OS
    return WIFEXITED( rc ) ? WEXITSTATUS( rc ) : -1;
#else
    return rc;
#endif
    }

static bool ensure_rule( std::string_view check_cmd,
    std::string_view append_cmd )
    {
    if ( const int check_rc = node_dev::run_cmd_exit_code( check_cmd );
        check_rc == 0 )
        {
        return true;
        }
    else if ( check_rc != 1 )
        {
        return false;
        }

    return node_dev::run_cmd_exit_code( append_cmd ) == 0;
    }

static bool delete_all_matches( std::string_view check_cmd, 
    std::string_view delete_cmd )
    {
    for ( ;; )
        {
        const int check_rc = node_dev::run_cmd_exit_code( check_cmd
#ifdef PTUSA_TEST
            , 1
#endif
        );
        if ( check_rc == 1 )
            {
            return true;
            }

        if ( check_rc != 0 )
            {
            return false;
            }

        if ( node_dev::run_cmd_exit_code( delete_cmd ) != 0 )
            {
            return false;
            }
        }
    }
//-----------------------------------------------------------------------------
int node_dev::process_web_cmd( int new_web_value )
    {
    if ( new_web_value != 0 && web_value == 0 )
        {
        if ( dnat.empty() || forward_in.empty() || masq.empty() )
            {
            G_LOG->warning(
                "No iptables rules defined for web port forwarding." );
            return 1;
            }

        if ( auto res = ensure_rule( dnat_check, dnat ) &&
            ensure_rule( forward_in_check, forward_in ) &&
            ensure_rule( masq_check, masq ); !res )
            {
            G_LOG->error( "Failed to enable web port forwarding for node '%s'.",
                get_name() );
            return 1;
            }

        G_LOG->warning( "Web port forwarding enabled for node '%s'.",
            get_name() );
        web_value = 1;
        return 0;
        }

    else if ( new_web_value == 0 && web_value == 1 )
        {
        if ( dnat_delete.empty() || forward_in_delete.empty() ||
            masq_delete.empty() )
            {
            G_LOG->warning(
                "No iptables rules defined for web port forwarding." );
            return 1;
            }

        if ( auto res = delete_all_matches( dnat_check, dnat_delete ) &&
            delete_all_matches( forward_in_check, forward_in_delete ) &&
            delete_all_matches( masq_check, masq_delete );
            !res )
            {
            G_LOG->error( "Failed to disable web port forwarding for node '%s'.",
                get_name() );
            return 1;
            }

        G_LOG->warning( "Web port forwarding disabled for node '%s'.",
            get_name() );
        web_value = 0;
        return 0;
        }

    return 0;
    }

int node_dev::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "WEB" ) == 0 )
        {
        if ( idx != 0 )
            {
            G_LOG->warning( "Invalid index %u for property '%s' of node '%s'.",
                idx, prop, get_name() );
            return 1;
            }

        if ( !node )
            {
            G_LOG->warning( "Node '%s' is not initialized.", get_name() );
            return 1;
            }

        return process_web_cmd( static_cast<int>( val ) );
        }

    else if ( strcmp( prop, "STARTUP" ) == 0 )
        {
        startup_value = static_cast<int>( val );
        return 0;
        }

    return device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
int node_dev::save_device( char* buff ) const
    {
    // LCOV_EXCL_START
    auto res_n = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "{}={{ST={}, WEB={}, STARTUP={}, IP='{}'}},\n",
        get_name(), get_state(), web_value, startup_value, get_ip() );
    // LCOV_EXCL_STOP

    return static_cast<int>( res_n.size );
    }
//-----------------------------------------------------------------------------
const char* node_dev::get_name_in_Lua() const
    {
    return "node_dev";
    }
//-----------------------------------------------------------------------------
const char* node_dev::get_ip() const
    {
    if ( node )
        {
        return node->ip_address;
        }

    return "";
    }
//-----------------------------------------------------------------------------

