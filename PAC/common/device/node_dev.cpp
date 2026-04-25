#include <fmt/core.h>

#include "node_dev.h"
#include "bus_coupler_io.h"
#include "log.h"

#ifdef LINUX_OS
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>

#include <sys/wait.h>
#endif

#ifdef LINUX_OS
namespace
    {
    std::string to_ipv4_string( const sockaddr_in* sa )
        {
        char buff[ INET_ADDRSTRLEN ]{};
        if ( inet_ntop( AF_INET, &sa->sin_addr, buff, sizeof( buff ) ) == nullptr )
            {
            return {};
            }

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
void node_dev::set_io_node( io_manager::io_node* io_node )
    {
    node = io_node;
    port_controller_web = 10'000 + node->number + 80;
    ip_controller = get_local_ipv4();

    if ( ip_controller.empty() )
        {
        G_LOG->warning( "Controller IPv4 address was not detected." );
        return;
        }

#ifdef LINUX_OS
    const std::string IPTABLES = "sudo -n /usr/sbin/iptables";
    dnat = IPTABLES + " -t nat -A PREROUTING -p tcp -d " + ip_controller +
        " --dport " + std::to_string( port_controller_web ) +
        " -j DNAT --to-destination " + node->ip_address + ":" + PORT_NODE_WEB;

    forward_in = IPTABLES + " -A FORWARD -p tcp -d " +
        node->ip_address + " --dport " + PORT_NODE_WEB + " -j ACCEPT";

    masq = IPTABLES + " -t nat -A POSTROUTING -p tcp -d " +
        node->ip_address + " --dport " + PORT_NODE_WEB + " -j MASQUERADE";

    dnat_delete = IPTABLES + " -t nat -D PREROUTING -p tcp -d " +
        ip_controller +
        " --dport " + std::to_string( port_controller_web ) +
        " -j DNAT --to-destination " + node->ip_address + ":" + PORT_NODE_WEB;

    forward_in_delete = IPTABLES + " -D FORWARD -p tcp -d " +
        node->ip_address + " --dport " + PORT_NODE_WEB + " -j ACCEPT";

    masq_delete = IPTABLES + " -t nat -D POSTROUTING -p tcp -d " +
        node->ip_address + " --dport " + PORT_NODE_WEB + " -j MASQUERADE";
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
static bool run_cmd( const std::string& cmd )
    {
    const int rc = std::system( cmd.c_str() );
    if ( rc == -1 ) return false;
#ifdef LINUX_OS
    return WIFEXITED( rc ) && WEXITSTATUS( rc ) == 0;
#else
    return rc == 0;
#endif
    }

int node_dev::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( !node ) return 1;

    if ( strcmp( prop, "WEB" ) == 0 )
        {
        auto new_web_value = static_cast<int>( val );

        if ( new_web_value == 1 && web_value == 0 )
            {
            if ( dnat.empty() || forward_in.empty() || masq.empty() )
                {
                G_LOG->warning(
                    "No iptables rules defined for web port forwarding." );
                return 1;
                }

            auto res = run_cmd( dnat ) &&
                run_cmd( forward_in ) &&
                run_cmd( masq );

            if ( !res )
                {
                G_LOG->error( "Failed to enable web port forwarding for node '%s'.",
                    get_name() );
                return 1;
                }

            G_LOG->warning( "Web port forwarding enabled for node '%s'.",
                get_name() );
            web_value = 1;
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

            auto res = run_cmd( dnat_delete ) &&
                run_cmd( forward_in_delete ) &&
                run_cmd( masq_delete );

            if ( !res )
                {
                G_LOG->error( "Failed to disable web port forwarding for node '%s'.",
                    get_name() );
                return 1;
                }

            G_LOG->warning( "Web port forwarding disabled for node '%s'.",
                get_name() );
            web_value = 0;
            }
        }
    else if ( strcmp( prop, "startup" ) == 0 )
        {
        startup_value = static_cast<int>( val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int node_dev::save_device( char* buff ) const
    {
    auto res_n = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "{}={{ST={}, WEB={}, STARTUP={}, IP='{}'}},\n",
        get_name(), get_state(), web_value, startup_value, get_ip() );
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
