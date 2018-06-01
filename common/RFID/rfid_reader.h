#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#ifdef _WIN32
    #include <winsock2.h>

	#include <windows.h>
	#include <conio.h>   
    
    void SetColor( WORD color );

#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <fcntl.h>

	#define closesocket close
	#define INVALID_SOCKET (-1)
	#define SOCKET_ERROR   (-1)

	#include <semaphore.h>
	#include <termios.h>
	#include <time.h>
	#include <unistd.h>
#endif

#include "API/RRU4.h"

#ifdef _WIN32
#else
	#define SetEvent(x)		sem_post(&x)
	#define ResetEvent(x)	while (!sem_trywait(&x));
	#define HANDLE			sem_t
#endif

// macros
#ifdef _WIN32
#define RED				(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define GREEN			(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define YELLOW			(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define WHITE			(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)

#define CLEARSCREEN		system("cls")
#else
#define RED				"\e[1;31m"
#define GREEN			"\e[1;32m"
#define YELLOW			"\e[1;33m"
#define WHITE			"\e[1;37m"
#define RESET			"\e[0m"

#define CLEARSCREEN		printf("\e[2J\e[H")
#define _getch()		getchar()
#define SetColor(x)		printf(x)
#endif

#include "PAC_err.h"

const int EPC_STR_LENGTH = 40;

struct EPC_info
    {
    char* EPC_str;
    int  antenna;
    int RSSI;

    EPC_info()
        {
        EPC_str = new char[ EPC_STR_LENGTH ];
        }

    EPC_info( char* EPC_str, int antenna, int RSSI );

    ~EPC_info()
        {
        delete [] EPC_str;
        EPC_str = 0;
        }
    };

class rfid_reader
	{
	public:

        enum CONSTANTS
            {
            MAX_READERS_COUNT = 10,
            MAX_EPS_COUNT = 10,

            MAX_WAIT_TIME = 300,

            ST_ACTIVE = 0,
            ST_CANT_CONNECT = -1,

            MAX_RETR_CNT = 3,
		    };

		static rfid_reader* add_reader( const char* ip_address, int number );

		int evaluate();

		const EPC_info* get_EPC_info( int idx ) const;
        int get_EPC_cnt() const;

		~rfid_reader();


	private:
		void connect();

		void disconnect();

		rfid_reader( const char* ip_address, int number );

		static void ResultHandlerSetExtendedResultFlag( tResultFlag enResultFlag,
			RRU4 hHandle, void *pTag );

        static void ResultHandlerASyncReadDataUntilEndOfBankAnySync(
            tResultFlag enResultFlag,
            tStartStopHeartbeatFlag enStartStopHeartbeatFlag,
            RRU4 hHandle, void *pTag );

        static void ResultHandlerASyncReadDataUntilEndOfBankAnyASync(
            tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlagMask,
            tEPCListEntry *pEPCList, RRU4 hHandle, void *pTag );

        static void ResultHandlerASyncStopCommand( tResultFlag enResultFlag,
            RRU4 hHandle, void *pTag );

		void CallSyncGetEPCs();

        void CallASyncReadDataAny();

        void stopAsyncREading();

		TBool CallSetExtResultFlag(TByte ubExtendedResultFlagMask);

		RRU4 _hReader;
		HANDLE _hResultHandlerEvent;
		volatile tResultFlag _enResultFlag;
#ifdef _WIN32
		HANDLE _hStdOut;
#else
		struct timespec _ts;
#endif

        int number;
		char* ip_address;
		tCommunicationConfigData communicationConfigData;

		static rfid_reader* rfid_readers[ MAX_READERS_COUNT ];
		static int cnt;

		int state;

		int EPC_cnt = 0;
		EPC_info EPC_info_array[ MAX_EPS_COUNT ];        
        
        int socket_number;
        sockaddr_in sock_address;
        timeval tv;

        int retr_cnt = 0;
        bool is_set_err = false;

        bool asinc_going = false;
	};
