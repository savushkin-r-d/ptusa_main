#ifndef TcpCom_h
#define TcpCom_h

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <stdio.h>

//ERRORS DEFINITION
#define ERR_RETRIVE        1
#define ERR_WRONG_SERVICE  3
#define ERR_TRANSMIT       4
#define ERR_WRONG_CMD      5

//COMMANDS DEFINITION
#define FrameSingle         1
#define AknErr              7
#define AknData             8
#define AknOK               12

#define BUFSIZE 	16384      			/*–азмер буфера*/
#define PORT 		10000      			/*ѕорт*/
#define MAX_SOCKETS 32         			/*ћаксимальное количество сокетов*/
#define QLEN  		MAX_SOCKETS - 1  	/*ћаксимальное количество соединений*/

#define uchar unsigned char

typedef struct socket_state
	{
	int                active;    /* сокет активен*/
	int                init;      /* сокет только что был активирован */
	int                islistener;/*сокет €вл€етс€ инициатором соедини€(=0)/сокет €вл€етс€ слушателем(!=0)*/
	int                evaluated; /*в данном цикле уже произошел обмен информацией по данному сокету*/
	int                clID;      /*идентификатор клиента дл€ идентификации того, не зан€т ли уже сокет другим клиентом*/
	struct sockaddr_in sin;       /* адрес клиента */
	} socket_state;

enum DESTDATA
	{
	DESTMEM = 0,
	DESTFILE,
	};

enum ERRORS_SUBCLASS
	{
	ES_MODBUS_DEVICE = 1,
	ES_EASYSERVER,
	};

typedef long SrvProc( DESTDATA, long int, unsigned char *, unsigned char * );
typedef SrvProc *SrvPtr;

/// @brief
///
/// –азвернутое описание класса.
/// @param
/// @return
class tcp_communicator
	{
	public:
        static tcp_communicator* get_instance();

		tcp_communicator();
		~tcp_communicator();
		
		int Evaluate();
		SrvPtr regService( unsigned char srvID, SrvPtr fk );

	private:
        static tcp_communicator* instance;

		enum CONSTANTS
			{
			TC_MAX_HOST_NAME = 20,
			TC_MAX_SERVICE_NUMBER = 16,
			};

		struct sockaddr_in 	ssin; 			/* адрес клиента */
		unsigned int 		sin_len;    	/* длина адреса */
		int 				master_socket;	/* мастер-сокет дл€ прослушивани€ */

#ifdef MODBUS
		int modbus_socket;				     /*модбас сокет*/
#endif
		int ss;                  /* слейв-сокет, получаемый при подключении клиента */
		int rc;                  /* код возврата selectsocket */
		
		unsigned int inBufCnt;
		int count_cycles;
		unsigned long glob_last_trans;

		unsigned char pidx;
		int NetId;
		SrvPtr Services[ TC_MAX_SERVICE_NUMBER ];
		int do_echo( int skt );
		int netInit();
		void _ErrorAkn( unsigned char error );
		void _AknData( unsigned long len );
		void _AknOK( void );

		int reboot;
		int glob_cmctr_ok;
		unsigned char* buf;
		int max_cycles;                     /*максимальное количество циклов обработки состо€ний сокетов за 1 проход */
		timeval tv;                         /* задержка ожидани€ функции опроса состо€ний сокетов 0 по умолчанию */
		fd_set rfds;                        /* набор дескрипторов сокетных файлов дл€ чтени€ */
		socket_state sst[ MAX_SOCKETS ];    /* таблица состо€ни€ сокетов */		
		char host_name[ TC_MAX_HOST_NAME ];
		int netOK;
		int tcpipClientID;

		void killsockets ();
		int net_init();
		void net_terminate();
		
		int recvtimeout( uint s, uchar *buf, int len, int timeout, int usec );
	};
	
#endif //TcpCom_h
