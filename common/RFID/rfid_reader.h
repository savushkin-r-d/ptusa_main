#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>

	#include <windows.h>
	#include <conio.h>    
#else
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

        MAX_WAIT_TIME = 30000,

		ST_ACTIVE 			= 0,
		ST_CANT_CONNECT 	= -1,
		};

		static rfid_reader* add_reader( const char* ip_address );

		int evaluate();

		const EPC_info* get_EPC_info( int idx ) const;
        int get_EPC_cnt() const;

		~rfid_reader();


	private:
		void connect();

		void disconnect();

		rfid_reader( const char* ip_address );

		static void ResultHandlerSetExtendedResultFlag( tResultFlag enResultFlag,
			RRU4 hHandle, void *pTag );

		static void ResultHandlerSyncGetEPCs( tResultFlag enResultFlag,
			TByte ubExtendedResultFlagMask,
			tEPCListEntry *pEPCList, RRU4 hHandle, void *pTag );

		void CallSyncGetEPCs();

		TBool CallSetExtResultFlag(TByte ubExtendedResultFlagMask);

		RRU4 _hReader;
		HANDLE _hResultHandlerEvent;
		volatile tResultFlag _enResultFlag;
#ifdef _WIN32
		HANDLE _hStdOut;
#else
		struct timespec _ts;
#endif

		char* ip_address;
		tCommunicationConfigData communicationConfigData;

		static rfid_reader* rfid_readers[ MAX_READERS_COUNT ];
		static int cnt;

		int state;

		int EPC_cnt;
		EPC_info EPC_info_array[ MAX_EPS_COUNT ];
        
        int socket_number;
        sockaddr_in sock_address;
        timeval tv;
	};
