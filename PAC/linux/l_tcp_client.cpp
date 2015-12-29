#include <netinet/in.h>
#include <fcntl.h>

#include "l_tcp_client.h"
#include "l_tcp_cmctr.h"

int linux_tcp_client::Communicate( unsigned int bytestosend )
    {

    if (!connectedstate)
        {
        if (!Connect())
            {
            return 0;
            }
        }

    if (send(socket_number, buff, bytestosend, 0 ) < 0)
        {
#ifdef DEBUG
        Print("tcp_client_%d Error sending message.\n", id);
#endif //DEBUG
        Disconnect();
        return 0;
        }

    int res = tcp_communicator_linux::recvtimeout(socket_number, (unsigned char*)buff, buff_size, 0, timeout * 1000, ip  );

    if (0 == res)
    {
#ifdef DEBUG
    Print("tcp_client_%d Server closed connection\n", id);
#endif //DEBUG
    Disconnect();
    return 0;
    }

    if (res < 0)
        {

#ifdef DEBUG
            Print("tcp_client_%d Error receiving answer\n", id);
#endif //DEBUG
        Disconnect();
        return 0;
        }

    return res;
    }

linux_tcp_client::linux_tcp_client( const char* client_ip, unsigned int client_port, unsigned int client_id,
                               unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/,
                               unsigned long send_receive_timeout /*= 100*/ ):  tcp_client(
                               client_ip, client_port, client_id, alarm_subclass, exchange_buf_size, send_receive_timeout)
    {

    }

int linux_tcp_client::Connect()
    {
    int res;
    if ( connectedstate )
        {
        return 1;
        }
    socket_number = socket(AF_INET,SOCK_STREAM,0);
    if (socket_number < 0)
        {
#ifdef DEBUG
        Print("tcp_client_%d: Error creating socket!\n", id);
#endif // DEBUG

        return 0;
        }

    const int C_ON = 1;

    if ( setsockopt(socket_number, SOL_SOCKET, SO_REUSEADDR, &C_ON, sizeof(C_ON)))
        {
#ifdef DEBUG
        Print("tcp_client_%d: Error setting socket params!\n", id);
#endif // DEBUG
        close(socket_number);
        return 0;
        }

    //Переводим сокет в неблокирующий режим.
    res = fcntl( socket_number, F_SETFL, O_NONBLOCK );
    if ( res != 0 )
        {
#ifdef DEBUG
        Print("tcp_client_%d: Error setting nonblock mode!\n", id);
#endif // DEBUG
        close( socket_number);
        socket_number = 0;
        return 0;
        }

    struct sockaddr_in sock_address;
    memset(&sock_address,0,sizeof(sockaddr_in));
    sock_address.sin_family  = AF_INET;
    sock_address.sin_port = htons( ( u_short ) port);
    sock_address.sin_addr.s_addr = inet_addr(ip);

    res = connect( socket_number, ( struct sockaddr* ) &sock_address, sizeof( sock_address ) );

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO(&rdevents);
    FD_SET(socket_number, &rdevents);

    tv.tv_sec = connectTimeout / 1000;
    tv.tv_usec = (connectTimeout % 1000) * 1000;

    res = select( socket_number + 1, 0, &rdevents, 0, &tv );

    if (res <= 0)
        {
#ifdef DEBUG
        Print("tcp_client_%d: Error in connect!\n", id);
#endif // DEBUG
        close( socket_number);
        socket_number = 0;
        return 0;
        }

    if (FD_ISSET(socket_number, &rdevents))
        {
        socklen_t err_len;
        int error = 0;
        err_len = sizeof(error);
        if (getsockopt(socket_number, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0 || error != 0)
            {
#ifdef DEBUG
            Print("tcp_client_%d: Error in connect (select)!\n", id);
#endif // DEBUG
            close(socket_number);
            socket_number = 0;
            return 0;
            }
        }


    connectedstate = 1;
    return 1;
    }

void linux_tcp_client::Disconnect()
    {
    shutdown( socket_number, SHUT_RDWR );
    close( socket_number );
    socket_number = 0;
    connectedstate = 0;
    }

int linux_tcp_client::AsyncSend( unsigned int bytestosend )
{
    async_result = AR_BUSY;

    if (!connectedstate)
        {
        if (get_delta_millisec(async_last_connect_try) > reconnectTimeout)
            {
            async_last_connect_try = get_millisec();
            if (!Connect())
                {
                async_result = AR_SOCKETERROR;
                reconnectTimeout += connectTimeout;
                if (reconnectTimeout > maxreconnectTimeout)
                    {
                    reconnectTimeout = maxreconnectTimeout;
                    }
                return 0;
                }
            else
                {
                reconnectTimeout = connectTimeout * RECONNECT_MIN_MULTIPLIER;
                }
            }
        else
            {
            async_result = AR_SOCKETERROR;
            return 0;
            }
        }

    int res = send(socket_number, buff, bytestosend, 0 );
    if ( res < 0)
        {
        async_result = AR_SOCKETERROR;
        Disconnect();
        return 0;
        }
    else
        {
        return tcp_client::AsyncSend(bytestosend);
        }
}

linux_tcp_client::~linux_tcp_client()
    {
    if (connectedstate)
        {
        Disconnect();
        }
    }

