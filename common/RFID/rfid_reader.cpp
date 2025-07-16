#include "rfid_reader.h"
#include "dtime.h"

 rfid_reader* rfid_reader::rfid_readers[ MAX_READERS_COUNT ] = { 0 };
 int rfid_reader::cnt = 0;

 extern int G_DEBUG;
 //-----------------------------------------------------------------------------
 EPC_info::EPC_info( char* EPC_str, int antenna, int RSSI ) : antenna( antenna ),
     RSSI( RSSI )
     {
     this->EPC_str = new char[ EPC_STR_LENGTH ];
     strcpy( this->EPC_str, EPC_str );
     }                                                                          
 //-----------------------------------------------------------------------------
 rfid_reader* rfid_reader::add_reader( const char* ip_address, int number )
	{
	if ( cnt < MAX_READERS_COUNT )
		{
		return rfid_readers[ cnt++ ] = new rfid_reader( ip_address, number );
		}

	return 0;
	}

 //-----------------------------------------------------------------------------
int rfid_reader::evaluate()
	{
	if ( _hReader == 0 )
		{

        //Для проверки доступности reader'а в сети
        socket_number = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
        if ( socket_number == INVALID_SOCKET )
            {
            if ( G_DEBUG )
                {
#ifdef _WIN32
                printf( "rfid_reader: Ошибка создания сокета %d!\n",
                    WSAGetLastError() );
#else
                printf( "rfid_reader: Ошибка создания сокета!\n" );
#endif
                }
            }
        else
            {
			//Переводим сокет в неблокирующий режим.
#ifdef _WIN32
			u_long mode = 1;
			int res = ioctlsocket( socket_number, FIONBIO, &mode );
#else
			int res = fcntl( socket_number, F_SETFL, O_NONBLOCK );
#endif

			if ( res == SOCKET_ERROR )
				{
				if ( G_DEBUG )
					{
#ifdef _WIN32
					printf( "rfid_reader: Ошибка перевода сокета в "
						"неблокирующий режим %d!\n", WSAGetLastError() );
#else
					printf( "rfid_reader: Ошибка перевода сокета в "
						"неблокирующий режим!\n" );
#endif
					}
				closesocket( socket_number );
				socket_number = 0;
				}
			else
				{
				unsigned int port = 22;
				tv.tv_sec = 0;
				tv.tv_usec = MAX_WAIT_TIME;

				memset( &sock_address, 0, sizeof( sockaddr_in ) );
				sock_address.sin_family = AF_INET;
				sock_address.sin_port = htons( ( u_short )port );
				sock_address.sin_addr.s_addr = inet_addr( ip_address );
				}
            }

        if ( socket_number > 0 )
            {
            ::connect( socket_number, ( struct sockaddr * ) &sock_address,
                sizeof( sockaddr_in ) );

            fd_set rfds;
            FD_ZERO( &rfds );
            FD_SET( socket_number, &rfds );
            int res = select( socket_number + 1, 0, &rfds, 0, &tv );

            closesocket( socket_number );
            socket_number = 0;

            if ( res > 0 )
                {
                connect();
                }
            }
		}

	if ( _hReader )
		{
        CallASyncReadDataAny();
		}

    // Проверка связи.
    if ( _hReader == 0 )
        {        
        if ( false == is_set_err )
            {
            is_set_err = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_RFID_READER, number );
            }
        }
    else
        {
        if ( is_set_err )
            {
            is_set_err = false;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_RFID_READER, number );
            }
        }
    // Проверка связи.-!>

	return 0;
	}

//-----------------------------------------------------------------------------
const EPC_info* rfid_reader::get_EPC_info( int idx ) const
	{
    if ( idx <= this->EPC_cnt )
        {
        return &EPC_info_array[ idx - 1 ];
        }

    return 0;
	}
//-----------------------------------------------------------------------------
int rfid_reader::get_EPC_cnt() const
    {
    return EPC_cnt;
    }
//-----------------------------------------------------------------------------
rfid_reader::~rfid_reader()
	{
	disconnect();
	}

//-----------------------------------------------------------------------------
void rfid_reader::connect()
	{
	_hReader = ConnectReader( &communicationConfigData, NULL );
	if ( _hReader )
		{
		state = ST_ACTIVE;
		}
	}

//-----------------------------------------------------------------------------
void rfid_reader::disconnect()
	{
	if ( _hReader )
		{
        EPC_cnt = 0;
        
        stopAsyncREading();

        DisconnectReader( _hReader );
		_hReader = 0;
		state = ST_CANT_CONNECT;
		}

    asinc_going = false;
	}

//-----------------------------------------------------------------------------
rfid_reader::rfid_reader( const char* ip_address, int number ): _hReader( 0 ),
    number( number )
	{
	this->ip_address = new char[ strlen( ip_address ) + 1 ];
	strcpy( this->ip_address, ip_address );
           
#ifdef _WIN32
	_hResultHandlerEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
#else
	sem_init( &_hResultHandlerEvent, 0, 0 );
#endif
	// ------------------------------------------------------------------------
	// Let's connecting the result handlers.
	// The reader calls a command-specific result handler if a command is done
    // and the answer is ready to send.
	// So let's tell the reader which functions should be called if a result is
    // ready to send.

	// result handler for setting an extended result flag
	SetDllResultHandlerSetExtendedResultFlag( ResultHandlerSetExtendedResultFlag );

	// result handler for reading epcs synchronous
	SetDllResultHandlerSyncGetEPCs( 0 );

	// result handler for reading EPCs data asynchronous
	// Note: Here we need two result handler,
	// first is the result handler for the answer of sending the command to
    // the reader, second is the result handler for the asynchronous
    // answers (if a tag comes or leaves the antenna field).    
    SetDllResultHandlerASyncReadDataUntilEndOfBankAny(
        ResultHandlerASyncReadDataUntilEndOfBankAnySync,
        ResultHandlerASyncReadDataUntilEndOfBankAnyASync );

	// result handler for asynchronous stop command
	// Note: The reader will do his asynchronous job until the
    // "ASyncStopCommand" is called.
	// Even if you disconnect, the reader will still do his job.
	// Note: Only one receiver of the asynchronous answers is possible,
    // that means if a second reader instance is sending
	// an asynchronous start command, the first reader will not get any answers.
	SetDllResultHandlerASyncStopCommand( ResultHandlerASyncStopCommand );

	// result handler for adding an action to an actionlist
	SetDllResultHandlerGPIOAddActionToActionlist( 0 );

	// result handler for assigning an actionlist to an input
	SetDllResultHandlerGPIOAssignInputToActionlist( 0 );

	// result handler for switching an output on and off
	SetDllResultHandlerGPIOSetOutput( 0 );

	// result handler for getting the io card configuration
	SetDllResultHandlerGetIOCardHwConfig( 0 );

	// result handler for setting the io card configuration
	SetDllResultHandlerSetIOCardHwConfig( 0 );

	// result handler which is called, if an usb device is added or removed
	SetDllEventHandlerUSBDevice( 0 );

	// Let's build the communication configuration.
	memset( &communicationConfigData, 0, sizeof( tCommunicationConfigData ) );

	// To connect via another option, just uncomment the corresponding code-block.

	// connect via ethernet
    // auto detect ethernet communication type
	communicationConfigData.enCommunicationType = CT_EthernetAuto; 
    // 0: network name; 1: ip address
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.bIsIPAddress = 0; 
    // IPv6 not supported yet
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.enIPAddressType = IPAT_IPv4;
	// Note: Let's set our ip address as a network name, it works like a charm.
    // network name, bIsIPAddress must be set to 0
	strcpy(communicationConfigData.ustCommunicationConfigParameters.stEthernet.uKindOfConnection.rgcNetworkName, this->ip_address );
    // keep-alive time
    communicationConfigData.ustCommunicationConfigParameters.stEthernet.udwKeepAliveTime = 3000;
    // NULL: don't use ssh connection, != NULL: password or path to key file
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.pcSSHAuth = NULL;
    // NULL: username for ssh connection is "root", != NULL: username for ssh connection
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.pcSSHUser = NULL;
	}

//-----------------------------------------------------------------------------
void rfid_reader::ResultHandlerSetExtendedResultFlag(
	tResultFlag enResultFlag, RRU4 hHandle, void *pTag )
	{
	for ( int i = 0; i < cnt; i++ )
		{
		if ( rfid_readers[ i ]->_hReader == hHandle )
			{
			// Let's store the result flag in a global variable to get access
			// from everywhere.
			rfid_readers[ i ]->_enResultFlag = enResultFlag;

			// Let's set the event so that the calling process knows the command
			// was processed by reader and the result is ready to get processed.
			SetEvent( rfid_readers[ i ]->_hResultHandlerEvent );

			break;
			}
		}
	}
//-----------------------------------------------------------------------------
void rfid_reader::ResultHandlerASyncReadDataUntilEndOfBankAnySync(
    tResultFlag enResultFlag,
    tStartStopHeartbeatFlag enStartStopHeartbeatFlag,
    RRU4 hHandle, void *pTag )
    {
    for ( int i = 0; i < cnt; i++ )
        {
        if ( rfid_readers[ i ]->_hReader == hHandle )
            {
            // Let's store the result flag in a global variable to get
            // access from everywhere.
            rfid_readers[ i ]->_enResultFlag = enResultFlag;

            // Let's set the event so that the calling process knows
            // the command was processed by reader and the result is
            // ready to get processed.
            SetEvent( rfid_readers[ i ]->_hResultHandlerEvent );
            }
        }
    }
//-----------------------------------------------------------------------------
void rfid_reader::ResultHandlerASyncReadDataUntilEndOfBankAnyASync(
    tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlagMask,
    tEPCListEntry *pEPCList, RRU4 hHandle, void *pTag )
    {
    for ( int i = 0; i < cnt; i++ )
        {
        rfid_reader* r = rfid_readers[ i ];
        if ( r->_hReader == hHandle )
            {
            tEPCListEntry *pResultListEntry;

            // Display all available epcs in the antenna field.
            pResultListEntry = pEPCList;
            while ( pResultListEntry != NULL )
                {
                TByte ubTemp;

                // At asynchronous commands you will get the requested data
                // and the epc.
                if ( enComingGoingFlag == CGF_Coming )
                    {
                    char* EPC_str = r->EPC_info_array[ r->EPC_cnt ].EPC_str;
                    memset( EPC_str, 0, EPC_STR_LENGTH );

                    for ( int j = 0; pResultListEntry->stEPC.ubEPCWordLength > j; j++ )
                        {
                        sprintf( EPC_str + strlen( EPC_str ), "%04X",
                            pResultListEntry->stEPC.rguwEPC[
                            pResultListEntry->stEPC.ubEPCWordLength - j - 1 ] );
                        }
                    r->EPC_info_array[ r->EPC_cnt ].antenna = pResultListEntry->ubPort;
                    r->EPC_info_array[ r->EPC_cnt ].RSSI = pResultListEntry->ubRSSI;

                    r->EPC_cnt++;

                    if ( G_DEBUG )
                        {
                        SetColor( WHITE );
                        printf( "%d EPC: ", i + 1 );
                        for ( ubTemp = 0; pResultListEntry->stEPC.ubEPCWordLength > ubTemp; ubTemp++ )
                            printf( "%04X", pResultListEntry->stEPC.rguwEPC[ pResultListEntry->stEPC.ubEPCWordLength - ubTemp - 1 ] );
                        printf( ", Antenna: %u, RSSI: %u is ", pResultListEntry->ubPort, pResultListEntry->ubRSSI );
                        SetColor( GREEN );
                        printf( "coming\r\n" );
                        SetColor( WHITE );
                        }
                    }
                else if ( enComingGoingFlag == CGF_Going )
                    {
                    static char EPC_str[ EPC_STR_LENGTH ];
                    memset( EPC_str, 0, EPC_STR_LENGTH );

                    for ( int j = 0; pResultListEntry->stEPC.ubEPCWordLength > j; j++ )
                        {
                        sprintf( EPC_str + strlen( EPC_str ), "%04X",
                            pResultListEntry->stEPC.rguwEPC[ pResultListEntry->stEPC.ubEPCWordLength - j - 1 ] );
                        }
                    //Ищем и удаляем данную метку.
                    for ( int j = 0; j < r->EPC_cnt; j++ )
                        {
                        if ( pResultListEntry->ubPort == r->EPC_info_array[ j ].antenna &&
                            strcmp( EPC_str, r->EPC_info_array[ j ].EPC_str ) == 0 )
                            {
                            for ( int l = j; l < r->EPC_cnt - 1; l++ )
                                {
                                r->EPC_info_array[ l ].antenna = r->EPC_info_array[ l + 1 ].antenna;
                                r->EPC_info_array[ l ].RSSI = r->EPC_info_array[ l + 1 ].RSSI;
                                strcpy( r->EPC_info_array[ l ].EPC_str, r->EPC_info_array[ l + 1 ].EPC_str );
                                }

                            r->EPC_cnt--;
                            }
                        }

                    if ( G_DEBUG )
                        {
                        SetColor( WHITE );
                        printf( "%d EPC: ", i + 1 );
                        for ( ubTemp = 0; pResultListEntry->stEPC.ubEPCWordLength > ubTemp; ubTemp++ )
                            printf( "%04X", pResultListEntry->stEPC.rguwEPC[ pResultListEntry->stEPC.ubEPCWordLength - ubTemp - 1 ] );
                        printf( ", Antenna: %u, Reads: %u is ", pResultListEntry->ubPort, pResultListEntry->uwReadCount );
                        SetColor( RED );
                        printf( "going\r\n" );
                        SetColor( WHITE );
                        }
                    }
                else
                    {
                    if ( G_DEBUG )
                        {
                        SetColor( WHITE );
                        printf( "%d EPC: ", i + 1 );
                        for ( ubTemp = 0; pResultListEntry->stEPC.ubEPCWordLength > ubTemp; ubTemp++ )
                            printf( "%04X", pResultListEntry->stEPC.rguwEPC[ pResultListEntry->stEPC.ubEPCWordLength - ubTemp - 1 ] );
                        printf( ", Antenna: %u is ", pResultListEntry->ubPort );
                        SetColor( YELLOW );
                        printf( "data changing\r\n" );
                        SetColor( WHITE );
                        }
                    }

                pResultListEntry = pResultListEntry->pNext;
                }

            // Let's set the event so that the calling process knows 
            // the command was processed by reader and the result is
            // ready to get processed.
            SetEvent( r->_hResultHandlerEvent );
            }
        }
    }
//-----------------------------------------------------------------------------
void rfid_reader::CallSyncGetEPCs()
	{
	// Let's set the extended result flag to "ERF_00RA". This means, that we'll
	// receive the antenna and rssi value.
	if ( CallSetExtResultFlag(ERF_MASK_A | ERF_MASK_R ) )
		{
		// The command was successfully processed by the reader.
		// Let's start synchronous reading epcs. This means that the reader
		// list all epcs together in the antenna field after completed
		// inventory.
		ResetEvent( _hResultHandlerEvent );

		tReaderErrorCode enReaderErrorCode = SyncGetEPCs( _hReader );

		if ( enReaderErrorCode == REC_NoError )
			{
			// No error occurs while sending the command to the reader. Let's
			// wait until the result handler was called.
#ifdef _WIN32
			if ( WAIT_OBJECT_0 ==
				WaitForSingleObject( _hResultHandlerEvent, MAX_WAIT_TIME ) )
#else
			clock_gettime( CLOCK_REALTIME, &_ts );
			_ts.tv_sec += 1;
			if (!sem_timedwait( &_hResultHandlerEvent, &_ts ) )
#endif
				{
				// The reader's work is done and the result handler was called.
				// Let's check the result flag to make sure everything is ok.
				if ( _enResultFlag == RF_NoError || _enResultFlag == RF_NoTag )
					{
					// The command was successfully processed by the reader.
					// We'll display the result in the result handler.

                    retr_cnt = 0;
					}
				else
					{
					// The command can't be proccessed by the reader.
					// To know why check the result flag.
					if ( G_DEBUG )
						{
						printf( "Command \"SyncGetEPCs\" returns with "
							"error %u\r\n", _enResultFlag );
						}
					}
				}
			else
				{
				// We're getting no answer from the reader within MAX_WAIT_TIME mseconds.
				if ( G_DEBUG )
					{
					printf( "Command \"SyncGetEPCs\" timed out\r\n" );
					}

                retr_cnt++;
                if ( retr_cnt > MAX_RETR_CNT )
                    {
                    disconnect();
                    }
				}
			}
		else
			{
			// Sending the command to the reader failed. To know why check the
			// error code.
			if ( G_DEBUG )
				{
				printf( "Sending the command \"SyncGetEPCs\" was not "
					"successful (%u)\r\n", enReaderErrorCode );
				}
			}
		}
	}
//-----------------------------------------------------------------------------
void rfid_reader::CallASyncReadDataAny()
    {
    // Let's set the extended result flag to "ERF_00RA". This means, that we'll
    //receive the antenna and rssi value.
    if ( CallSetExtResultFlag( ERF_MASK_A | ERF_MASK_R ) )
        {
        // The command was successfully processed by the reader.

        if ( asinc_going ) return;

        // Let's use the command "ASyncReadDataUntilEndOfBankAny" to read the
        //complete tid memory bank.
        // Using this command prevents error if a tags memory bank is not
        //accessible.
        ResetEvent( _hResultHandlerEvent );
        tReaderErrorCode enReaderErrorCode = 
            ASyncReadDataUntilEndOfBankAny( _hReader, 0, TMB_TID, 0, 0xFF );
        if ( enReaderErrorCode == REC_NoError )
            {
            // No error occurs while sending the command to the reader. Let's
            //wait until the result handler was called.
#ifdef _WIN32
            if ( WAIT_OBJECT_0 == 
                WaitForSingleObject( _hResultHandlerEvent, MAX_WAIT_TIME ) )
#else
            clock_gettime( CLOCK_REALTIME, &_ts );
            _ts.tv_sec += 1;
            if ( !sem_timedwait( &_hResultHandlerEvent, &_ts ) )
#endif
                {
                // The reader's work is done and the result handler was called.
                //Let's check the result flag to make sure everything is ok.
                if ( _enResultFlag == RF_NoError )
                    {
                    asinc_going = true;
                    }
                else
                    {
                    if ( G_DEBUG )
                        {
                        // The command can't be proccessed by the reader. To know
                        //why check the result flag.
                        SetColor( RED );
                        printf( "Command \"ASyncReadDataUntilEndOfBankAny\" "
                            "exited with error %u\r\n", _enResultFlag );
                        SetColor( WHITE );
                        }
                    }
                }
            else
                {
                if ( G_DEBUG )
                    {
                    // We're getting no answer from the reader within MAX_WAIT_TIME.
                    SetColor( RED );
                    printf( "Command \"ASyncReadDataUntilEndOfBankAny\" "
                        "timed out\r\n" );
                    SetColor( WHITE );
                    }
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                // Sending the command to the reader failed. To know why check the
                //error code.
                SetColor( RED );
                printf( "Sending the command \"ASyncReadDataUntilEndOfBankAny\" "
                    "was not successful (%u)\r\n", enReaderErrorCode );
                SetColor( WHITE );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
void rfid_reader::stopAsyncREading()
    {    
    ResetEvent( _hResultHandlerEvent );
    tReaderErrorCode enReaderErrorCode = ASyncStopCommand( _hReader );
    if ( enReaderErrorCode == REC_NoError )
        {
        // No error occurs while sending the command to the reader. Let's wait
        //until the result handler was called.
#ifdef _WIN32
        if ( WAIT_OBJECT_0 == WaitForSingleObject( _hResultHandlerEvent, MAX_WAIT_TIME ) )
#else
        clock_gettime( CLOCK_REALTIME, &_ts );
        _ts.tv_sec += 1;
        if ( !sem_timedwait( &_hResultHandlerEvent, &_ts ) )
#endif
            {
            // The reader's work is done and the result handler was called.
            //Let's check the result flag to make sure everything is ok.
            if ( _enResultFlag == RF_NoError )
                {
                // The command was successfully processed by the reader.
                }
            else
                {
                if ( G_DEBUG )
                    {
                    // The command can't be proccessed by the reader. To know why
                    //check the result flag.
                    SetColor( RED );
                    printf( "Command \"ASyncStopCommand\" exited with error %u\r\n",
                        _enResultFlag );
                    SetColor( WHITE );
                    }
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                // We're getting no answer from the reader within MAX_WAIT_TIME
                // time.
                SetColor( RED );
                printf( "Command \"ASyncStopCommand\" timed out\r\n" );
                SetColor( WHITE );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
void rfid_reader::ResultHandlerASyncStopCommand( tResultFlag enResultFlag,
    RRU4 hHandle, void *pTag )
    {
    for ( int i = 0; i < cnt; i++ )
        {
        if ( rfid_readers[ i ]->_hReader == hHandle )
            {

            // Let's store the result flag in a global variable to get access
            // from everywhere.
            rfid_readers[ i ]->_enResultFlag = enResultFlag;

            // Let's set the event so that the calling process knows the
            // command was processed by reader and the result is ready to
            // get processed.
            SetEvent( rfid_readers[ i ]->_hResultHandlerEvent );
            }
        }
    }
//-----------------------------------------------------------------------------
TBool rfid_reader::CallSetExtResultFlag(TByte ubExtendedResultFlagMask)
	{
	TBool result = false;

	ResetEvent( _hResultHandlerEvent );
	tReaderErrorCode enReaderErrorCode = SetExtendedResultFlag( _hReader,
		ubExtendedResultFlagMask );
	if ( enReaderErrorCode == REC_NoError )
		{
		// No error occurs while sending the command to the reader. Let's wait
		// until the result handler was called.
#ifdef _WIN32
		if ( WAIT_OBJECT_0 ==
			WaitForSingleObject( _hResultHandlerEvent, MAX_WAIT_TIME ) )
#else
		clock_gettime( CLOCK_REALTIME, &_ts );
		_ts.tv_sec += 5;
		if (!sem_timedwait(&_hResultHandlerEvent, &_ts))
#endif
			{
			// The reader's work is done and the result handler was called.
			// Let's check the result flag to make sure everything is OK.
			if (_enResultFlag == RF_NoError)
				{
				// The command was successfully processed by the reader.
                retr_cnt = 0;
				result = true;
				}
			else
				{
				// The command can't be processed by the reader.
				// To know why check the result flag.
				if ( G_DEBUG )
					{
					printf("Command \"SetExtendedResultFlag\" returns with "
						"error %u\r\n", _enResultFlag);
					}
				}
			}
		else
			{
			// We're getting no answer from the reader within MAX_WAIT_TIME mseconds.
			if ( G_DEBUG )
				{
				printf("Command \"SetExtendedResultFlag\" timed out\r\n");
				}

            retr_cnt++;
            if ( retr_cnt > MAX_RETR_CNT )
                {
                disconnect();
                }
			}
		}
	else
		{
		// Sending the command to the reader failed. To know why check the
		// error code.
		if ( G_DEBUG )
			{
			printf("Sending the command \"SetExtendedResultFlag\" was not "
					"successful (%u)\r\n", enReaderErrorCode);
			}

        retr_cnt++;
        if ( retr_cnt > MAX_RETR_CNT )
            {
            disconnect();
            }        
		}

	return result;
	}
