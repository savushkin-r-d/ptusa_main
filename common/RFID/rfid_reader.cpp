#include "rfid_reader.h"

#pragma warning( push )
#pragma warning( disable: 4996 ) 

 rfid_reader* rfid_reader::rfid_readers[ MAX_READERS_COUNT ] = { 0 };
 int rfid_reader::cnt = 0;

 extern int G_DEBUG;
 //-----------------------------------------------------------------------------
 rfid_reader* rfid_reader::add_reader( const char* ip_address )
	{
	if ( cnt < MAX_READERS_COUNT )
		{
		return rfid_readers[ cnt++ ] = new rfid_reader( ip_address );
		}

	return 0;
	}

 //-----------------------------------------------------------------------------
int rfid_reader::evaluate()
	{
	if ( _hReader == 0 )
		{
		connect();
		}

	if ( _hReader )
		{
		CallSyncGetEPCs();
		}

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
		DisconnectReader( _hReader );
		_hReader = 0;
		state = ST_CANT_CONNECT;
		}
	}

//-----------------------------------------------------------------------------
rfid_reader::rfid_reader( const char* ip_address )
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
	// The reader calls a command-specific result handler if a command is done and the answer is ready to send.
	// So let's tell the reader which functions should be called if a result is ready to send.

	// result handler for setting an extended result flag
	SetDllResultHandlerSetExtendedResultFlag( ResultHandlerSetExtendedResultFlag );

	// result handler for reading epcs synchronous
	SetDllResultHandlerSyncGetEPCs( ResultHandlerSyncGetEPCs );

	// result handler for reading EPCs data asynchronous
	// Note: Here we need two result handler,
	// first is the result handler for the answer of sending the command to the reader,
	// second is the result handler for the asynchronous answers (if a tag comes or leaves the antenna field).
	SetDllResultHandlerASyncReadDataUntilEndOfBankAny( 0, 0 );

	// result handler for asynchronous stop command
	// Note: The reader will do his asynchronous job until the "ASyncStopCommand" is called.
	// Even if you disconnect, the reader will still do his job.
	// Note: Only one receiver of the asynchronous answers is possible, that means if a second reader instance is sending
	// an asynchronous start command, the first reader will not get any answers.
	SetDllResultHandlerASyncStopCommand( 0 );

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
	communicationConfigData.enCommunicationType = CT_EthernetAuto; // auto detect ethernet communication type (auto detect generation 2 / generation 3 reader)
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.bIsIPAddress = 0; // 0: network name; 1: ip address
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.enIPAddressType = IPAT_IPv4; // IPv6 not supported yet
	// Note: Let's set our ip address as a network name, it works like a charm.
	strcpy(communicationConfigData.ustCommunicationConfigParameters.stEthernet.uKindOfConnection.rgcNetworkName, this->ip_address ); // network name, bIsIPAddress must be set to 0
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.udwKeepAliveTime = 300; // keep-alive time
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.pcSSHAuth = NULL; // NULL: don't use ssh connection, != NULL: password or path to key file
	communicationConfigData.ustCommunicationConfigParameters.stEthernet.pcSSHUser = NULL; // NULL: username for ssh connection is "root", != NULL: username for ssh connection

	connect();
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
void rfid_reader::ResultHandlerSyncGetEPCs( tResultFlag enResultFlag,
	TByte ubExtendedResultFlagMask, tEPCListEntry *pEPCList,
	RRU4 hHandle, void *pTag )
	{
	for ( int i = 0; i < cnt; i++ )
		{
		if ( rfid_readers[ i ]->_hReader == hHandle )
			{
			tEPCListEntry *pResultListEntry;

			// Let's store the result flag in a global variable to get access
			// from everywhere.
			rfid_readers[ i ]->_enResultFlag = enResultFlag;

			int idx = 0;

			// Display all available epcs in the antenna field.
			pResultListEntry = pEPCList;
			while (pResultListEntry != NULL)
				{
				TByte ubTemp;

				if ( G_DEBUG )
					{
					printf("EPC: ");
					for ( ubTemp = 0;
						pResultListEntry->stEPC.ubEPCWordLength > ubTemp; ubTemp++)
						{
						printf("%04X", pResultListEntry->stEPC.rguwEPC[
							pResultListEntry->stEPC.ubEPCWordLength - ubTemp - 1 ] );
						}

					printf(", Antenna: %u, RSSI: %u\r\n",
						pResultListEntry->ubPort, pResultListEntry->ubRSSI);
					}

				char *EPC_str = rfid_readers[ i ]->EPC_info_array[ idx ].EPC_str;

				memset( EPC_str, 0, EPC_STR_LENGTH );

				for ( int j = 0; pResultListEntry->stEPC.ubEPCWordLength > j; j++ )
					{
					sprintf( EPC_str + strlen( EPC_str ), "%04X",
						pResultListEntry->stEPC.rguwEPC[ pResultListEntry->stEPC.ubEPCWordLength - j - 1 ] );
					}
				rfid_readers[ i ]->EPC_info_array[ idx ].antenna =
					pResultListEntry->ubPort;

				idx++;

				pResultListEntry = pResultListEntry->pNext;
				}
			rfid_readers[ i ]->EPC_cnt = idx;

			// Let's set the event so that the calling process knows the
			// command was processed by reader and the result is ready to get
			// processed.
			SetEvent( rfid_readers[ i ]->_hResultHandlerEvent );
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
				WaitForSingleObject( _hResultHandlerEvent, 100 ) )
#else
			clock_gettime( CLOCK_REALTIME, &_ts );
			_ts.tv_sec += 5;
			if (!sem_timedwait( &_hResultHandlerEvent, &_ts ) )
#endif
				{
				// The reader's work is done and the result handler was called.
				// Let's check the result flag to make sure everything is ok.
				if ( _enResultFlag == RF_NoError )
					{
					// The command was successfully processed by the reader.
					// We'll display the result in the result handler.
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
				// We're getting no answer from the reader within 5 seconds.
				if ( G_DEBUG )
					{
					printf( "Command \"SyncGetEPCs\" timed out\r\n" );
					}

				disconnect();
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
	if ( G_DEBUG )
		{
		printf( "\r\n" );
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
			WaitForSingleObject( _hResultHandlerEvent, 100 ) )
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
			// We're getting no answer from the reader within 5 seconds.
			if ( G_DEBUG )
				{
				printf("Command \"SetExtendedResultFlag\" timed out\r\n");
				}

			disconnect();
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
		}

	return result;
	}

#pragma warning( pop )