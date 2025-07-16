#include "profibus_slave_PFC200.h"

auto_smart_ptr < profibus_slave_PFC200 > profibus_slave_PFC200::instance = 0;
//------------------------------------------------------------------------------
profibus_slave_PFC200* profibus_slave_PFC200::get_instance()
    {
    if ( instance.is_null() )
    {
    instance = new profibus_slave_PFC200();
    }

    return instance;
    }
//----------------------------------------------------------------------------------------------------------------------
#ifndef USE_PROFIBUS

#else
int profibus_slave_PFC200::init()
    {
    int32_t iDalResult = DAL_SUCCESS;             	//Return value of the DAL interface.
    tDeviceInfo astDeviceList[MAX_DEVICE_LIST_ENTRIES]; //The list of devices given by the ADI.
    size_t nrDevicesFound = 0;                 //Number of devices found.

#ifdef DEBUG_PROFIBUS_SLAVE
    printf("\n***************************************************\n");
    printf("*** PROFIBUS DP-V1 Slave ADI initialization.\n");
    printf("***************************************************\n\n");
#endif // DEBUG_PROFIBUS_SLAVE

    //Request the ADI.
    iDalResult = get_ADI( &pstAdi );

    if ( DAL_SUCCESS == iDalResult )
    {
    //Initialize the ADI.
    iDalResult = init_ADI( pstAdi );
    }

    if ( DAL_SUCCESS == iDalResult )
    {
    //Scan for devices.
    iDalResult = scan_for_devices( pstAdi );

    if ( DAL_SUCCESS == iDalResult )
        {
        //Get the device list.
        iDalResult = get_device_list( pstAdi, &nrDevicesFound,
            astDeviceList, MAX_DEVICE_LIST_ENTRIES );
        }

    if ( DAL_SUCCESS == iDalResult )
        {
        //Get the PROFIBUS slave device ID.
        iDalResult = get_device_id( nrDevicesFound, astDeviceList,
            &iDpsDeviceId );
        }

    if ( DAL_SUCCESS == iDalResult )
        {
        //Open the device.
        iDalResult = open_device( pstAdi, iDpsDeviceId );
        }

    if ( DAL_SUCCESS == iDalResult )
        {
        //Set up the watchdog.
        iDalResult = setup_watchdog( pstAdi );

        //Configure the device.
        if ( DAL_SUCCESS == iDalResult )
        {
        iDalResult = configure_device( pstAdi, iDpsDeviceId );
        }

        //Report the application state running event.
        if ( DAL_SUCCESS == iDalResult )
        {
        iDalResult = report_app_state_running_evt( pstAdi );
        }

        //Start the watchdog.
        if ( DAL_SUCCESS == iDalResult )
            {
            iDalResult = start_watchdog( pstAdi );
            }
        }
    }

    //Clean the process images.
    (void) memset( aucPlcPrcImgInp, 0x00, sizeof(aucPlcPrcImgInp) );
    (void) memset( aucPlcPrcImgOutp, 0x00, sizeof(aucPlcPrcImgOutp) );

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::get_ADI( tApplicationDeviceInterface** ppstAdi )
    {
    int32_t iDalResult = DAL_SUCCESS;
    *ppstAdi = adi_GetApplicationInterface();

    if ( NULL != ppstAdi )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf ("-> Request the application device interface (ADI)...      ");
    printf ("success\n");
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Request the application device interface (ADI) - failed." );
    G_LOG->write_log( i_log::P_CRIT );

    iDalResult = DAL_FAILURE;
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::init_ADI( tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;

    if( NULL == pstAdi ) return iDalResult;

    iDalResult = pstAdi->Init();

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf ("-> Initialize the application device interface (ADI)...   ");
    printf ("success\n");
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Initialize the application device interface (ADI) - failed." );
    G_LOG->write_log( i_log::P_CRIT );
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::scan_for_devices( tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;

    if( NULL == pstAdi ) return iDalResult;

    iDalResult = pstAdi->ScanDevices();

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Scan for devices...                                    " );
    printf ("success\n");
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Scan for devices - failed." );
    G_LOG->write_log( i_log::P_CRIT );
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::get_device_list(
    tApplicationDeviceInterface* pstAdi, size_t* pulNrDevicesFound,
    tDeviceInfo* pastDeviceList, size_t ulMaxNumOfDevicesInList )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

    iDalResult = pstAdi->GetDeviceList( ulMaxNumOfDevicesInList, pastDeviceList,
        pulNrDevicesFound );

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Get the device list...                                 " );
    printf ("success\n");
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Get the device list - failed." );
    G_LOG->write_log( i_log::P_CRIT );
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::get_device_id( size_t ulNrDevicesFound,
    tDeviceInfo* pastDeviceList, tDeviceId* piDpsDeviceId )
    {
    int32_t iDalResult = DAL_SUCCESS;
    uint_t uiIndex;

    //Initialize the device ID.
    *piDpsDeviceId = -1;

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Find the PROFIBUS slave device <libdps>...\n" );
#endif // DEBUG_PROFIBUS_SLAVE


    for ( uiIndex = 0; uiIndex < ulNrDevicesFound; uiIndex++ )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "    Device-ID %d = %s... ", pastDeviceList[uiIndex].DeviceId,
        pastDeviceList[uiIndex].DeviceName );
#endif // DEBUG_PROFIBUS_SLAVE


    if ( strcmp( pastDeviceList[uiIndex].DeviceName, "libdps" ) == 0 )
        {
        *piDpsDeviceId = pastDeviceList[uiIndex].DeviceId;
#ifdef DEBUG_PROFIBUS_SLAVE
        printf( "found\n" );
#endif // DEBUG_PROFIBUS_SLAVE
        }
    else
        {
#ifdef DEBUG_PROFIBUS_SLAVE
        printf( "\n" );
#endif // DEBUG_PROFIBUS_SLAVE
        }
    }

    if ( -1 == *piDpsDeviceId )
    {
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Find the PROFIBUS slave device <libdps> - failed." );
    G_LOG->write_log( i_log::P_CRIT );
    iDalResult = DAL_FAILURE;
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::open_device( tApplicationDeviceInterface* pstAdi,
    tDeviceId iDpsDeviceId )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

    tError ulError;
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Open the device...                                     " );
#endif // DEBUG_PROFIBUS_SLAVE

    iDalResult = pstAdi->OpenDevice( iDpsDeviceId );

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "success\n" );
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
    (void) pstAdi->GetLastError( iDpsDeviceId, &ulError );
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "failed (%u)\n", ulError );
#else
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Open the device - failed (%u).", ulError );
    G_LOG->write_log( i_log::P_CRIT );
#endif // DEBUG_PROFIBUS_SLAVE
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::setup_watchdog( tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Set up the watchdog...                                 " );
#endif // DEBUG_PROFIBUS_SLAVE

    iDalResult = pstAdi->WatchdogSetTime( 3, 10000000 );

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "success\n" );
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "failed\n" );
#else
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Set up the watchdog - failed." );
    G_LOG->write_log( i_log::P_CRIT );
#endif // DEBUG_PROFIBUS_SLAVE
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::configure_device(
    tApplicationDeviceInterface* pstAdi, tDeviceId iDpsDeviceId )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

    dpsDeviceConfig_t stDeviceConfig;      //Device configuration.
    pbSlotCfgDesc_t astRealSlotCfgDesc[2]; //Real slot configuration description.

    //Set up the real configuration.

    //Slot 1 - 16 byte DP-Master output (BYTE Array [output_byte_size] (DPM-OUT)).
    astRealSlotCfgDesc[0].enmDataType = DPS_ARRAY_OF_BYTE;
    astRealSlotCfgDesc[0].ulPiInpDataBitOffset = 0;
    astRealSlotCfgDesc[0].ulPiInpDataBitLen = 0;
    astRealSlotCfgDesc[0].ulPiOutpDataBitOffset = 0;
    astRealSlotCfgDesc[0].ulPiOutpDataBitLen = output_byte_size * 8;

    //Slot 2 - 16 byte DP-Master input (BYTE Array [input_byte_size] (DPM-IN)).
    astRealSlotCfgDesc[1].enmDataType = DPS_ARRAY_OF_BYTE;
    astRealSlotCfgDesc[1].ulPiInpDataBitOffset = 0;
    astRealSlotCfgDesc[1].ulPiInpDataBitLen = input_byte_size * 8;
    astRealSlotCfgDesc[1].ulPiOutpDataBitOffset = 0;
    astRealSlotCfgDesc[1].ulPiOutpDataBitLen = 0;

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Configure the device...                                " );
#endif // DEBUG_PROFIBUS_SLAVE

    //Set the slave parameter.
    stDeviceConfig.lDeviceId = iDpsDeviceId;
    stDeviceConfig.ulDpSlaveAdress = station_address;

    //Assign the real configuration description.
    stDeviceConfig.stRealConfigDesc.bRealCfgModeStatic = false;
    stDeviceConfig.stRealConfigDesc.ulNoOfSlots = 2;
    stDeviceConfig.stRealConfigDesc.pastRealSlotConfig = &astRealSlotCfgDesc[0];

    iDalResult = pstAdi->ConfigureDevice( iDpsDeviceId, &stDeviceConfig );

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "success\n" );
#endif // DEBUG_PROFIBUS_SLAVE
    }
    else
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "failed\n" );
#else
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Configure the device - failed." );
    G_LOG->write_log( i_log::P_CRIT );
#endif // DEBUG_PROFIBUS_SLAVE
    }

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "    Slot 1: BYTE Array [%d] (DPM-OUT)\n", output_byte_size );
    printf( "    Slot 2: BYTE Array [%d] (DPM-IN)\n", input_byte_size  );
#endif // DEBUG_PROFIBUS_SLAVE

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::report_app_state_running_evt(
    tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Report the application state running event...          " );
#endif // DEBUG_PROFIBUS_SLAVE

    tApplicationStateChangedEvent event;

    event.State = ApplicationState_Running;

    iDalResult = pstAdi->ApplicationStateChanged( event );

    if ( DAL_SUCCESS == iDalResult )
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "success\n" );
#endif // DEBUG_PROFIBUS_SLAVE

    }
    else
    {
#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "failed\n" );
#else
    sprintf( G_LOG->msg, "profibus_slave_PFC200: "
        "Report the application state running event - failed." );
    G_LOG->write_log( i_log::P_CRIT );
#endif // DEBUG_PROFIBUS_SLAVE
    }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::start_watchdog(
    tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Start the watchdog...                                  " );
#endif // DEBUG_PROFIBUS_SLAVE

    iDalResult = pstAdi->WatchdogStart();

    if ( DAL_SUCCESS == iDalResult )
        {
#ifdef DEBUG_PROFIBUS_SLAVE
        printf( "success\n" );
#endif // DEBUG_PROFIBUS_SLAVE
        }
    else
        {
#ifdef DEBUG_PROFIBUS_SLAVE
        printf( "failed\n" );
#else
        sprintf( G_LOG->msg, "profibus_slave_PFC200: "
                "Start the watchdog - failed." );
        G_LOG->write_log( i_log::P_CRIT );
#endif // DEBUG_PROFIBUS_SLAVE
        }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int profibus_slave_PFC200::eval()
    {
    if ( NULL == pstAdi ) return DAL_FAILURE;

    int32_t iDalResult = DAL_SUCCESS; //DAL result.
#ifdef DEBUG_PROFIBUS_SLAVE
    uint32_t ulStatus;               //Status of the device specific function call.
    uint8_t ucDevState = 0;       //Current device state.
#endif // DEBUG_PROFIBUS_SLAVE

    //Trigger the watchdog.
    iDalResult = pstAdi->WatchdogTrigger();

    //Read data from fieldbus output process image.
    if ( iDalResult == DAL_SUCCESS )
        {
        iDalResult = pstAdi->ReadStart( iDpsDeviceId, 0 );

        if ( iDalResult == DAL_SUCCESS )
            {
            iDalResult = pstAdi->ReadBytes( iDpsDeviceId, 0, 0, 244,
                &aucPlcPrcImgInp[0] );

            if ( iDalResult == DAL_SUCCESS )
                {
                iDalResult = pstAdi->ReadEnd( iDpsDeviceId, 0 );
                }
            }
        }

#ifdef DEBUG_PROFIBUS_SLAVE_ECHO
    //Echo the process data.
    memcpy(&aucPlcPrcImgOutp[0], &aucPlcPrcImgInp[0], sizeof(aucPlcPrcImgInp));
#endif

    //Write data to the fieldbus input process image.
    if ( iDalResult == DAL_SUCCESS )
        {
        iDalResult = pstAdi->WriteStart( iDpsDeviceId, 0 );

        if ( iDalResult == DAL_SUCCESS )
            {
            iDalResult = pstAdi->WriteBytes( iDpsDeviceId, 0, 0, 244,
                &aucPlcPrcImgOutp[0] );

            if ( iDalResult == DAL_SUCCESS )
                {
                iDalResult = pstAdi->WriteEnd( iDpsDeviceId, 0 );
                }
            }
        }

#ifdef DEBUG_PROFIBUS_SLAVE
    /* print the PLC input process data */
    if ( iDalResult == DAL_SUCCESS )
        {
        printf( "PROFIBUS slave DP PLC input data = "
            "0x%02X %02X %02X %02X %02X "
              "%02X %02X %02X %02X %02X "
              "%02X %02X %02X %02X %02X\n",
        aucPlcPrcImgInp[0], aucPlcPrcImgInp[1], aucPlcPrcImgInp[2],
        aucPlcPrcImgInp[3], aucPlcPrcImgInp[4], aucPlcPrcImgInp[5],
        aucPlcPrcImgInp[6], aucPlcPrcImgInp[7], aucPlcPrcImgInp[8],
        aucPlcPrcImgInp[9], aucPlcPrcImgInp[10], aucPlcPrcImgInp[11],
        aucPlcPrcImgInp[12], aucPlcPrcImgInp[13], aucPlcPrcImgInp[14] );
        }

    /* get the device state */
    if ( iDalResult == DAL_SUCCESS )
    {
    iDalResult = pstAdi->CallDeviceSpecificFunction( "DPS_GET_DEV_STATE",
        &ulStatus, &ucDevState );

    if ( DAL_SUCCESS == iDalResult )
        {
        if ( DPS_SUCCESS == ulStatus )
            {
            printf( "PROFIBUS slave DP device state %d\n", ucDevState );
            }
        }
    }
#endif // DEBUG_PROFIBUS_SLAVE

    return (iDalResult);
    }
//------------------------------------------------------------------------------
double profibus_slave_PFC200::get_double( int offset )
    {
    double res = 0;
    if ( offset < 240 )
        {
        char tmp[4];

        tmp[0] = aucPlcPrcImgInp[offset + 3];
        tmp[1] = aucPlcPrcImgInp[offset + 2];
        tmp[2] = aucPlcPrcImgInp[offset + 1];
        tmp[3] = aucPlcPrcImgInp[offset];

        float *val = (float*) tmp;
        res = *val;
        }

    return res;
    }
//------------------------------------------------------------------------------
bool profibus_slave_PFC200::get_bool( int byte_offset, int bit_offset )
    {
    bool res = false;
    if ( byte_offset < 244 && bit_offset < 8 )
        {
        char tmp = aucPlcPrcImgInp[byte_offset];
        res = ( 1 << bit_offset ) & tmp;
        }

    return res;
    }
//------------------------------------------------------------------------------
int profibus_slave_PFC200::get_int( int byte_offset )
    {
    int res = 0;
    if ( byte_offset < 242 )
        {
        res = 256 * aucPlcPrcImgInp[byte_offset] +
            aucPlcPrcImgInp[byte_offset + 1];
        }

    return res;
    }
//------------------------------------------------------------------------------
int profibus_slave_PFC200::get_int4( int byte_offset )
    {
    int res = 0;
    if ( byte_offset < 240 )
        {
        res = 16777216 * aucPlcPrcImgInp[byte_offset] +
            65536 * aucPlcPrcImgInp[byte_offset + 1] +
            256 * aucPlcPrcImgInp[byte_offset + 2] +
            aucPlcPrcImgInp[byte_offset + 3];
        }

    return res;
    }
//------------------------------------------------------------------------------
void profibus_slave_PFC200::set_int( int byte_offset, int val )
    {
    if ( byte_offset < 242 )
        {
        aucPlcPrcImgOutp[byte_offset] = val >> 8;
        aucPlcPrcImgOutp[byte_offset + 1] = val;
        }
    }
//------------------------------------------------------------------------------
void profibus_slave_PFC200::set_bool( int byte_offset, int bit_offset, bool val )
    {
    if ( byte_offset < 244 && bit_offset < 8 )
        {
        char tmp = 1 << bit_offset;
        if ( val )
            {
            aucPlcPrcImgOutp[byte_offset] |= tmp;
            }
        else
            {
            aucPlcPrcImgOutp[byte_offset] &= ~tmp;
            }
        }
    }
//------------------------------------------------------------------------------
void profibus_slave_PFC200::close()
    {
    (void) stop_watchdog( pstAdi );

    (void) close_device( pstAdi );

    (void) release_ADI( pstAdi );

    pstAdi = NULL;
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::stop_watchdog( tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

#ifdef DEBUG_PROFIBUS_SLAVE
    printf( "-> Stop the PLC watchdog...                               " );
#endif // DEBUG_PROFIBUS_SLAVE

    iDalResult = pstAdi->WatchdogStop();

    if ( DAL_SUCCESS == iDalResult )
        {
#ifdef DEBUG_PROFIBUS_SLAVE
        printf( "success\n" );
#endif // DEBUG_PROFIBUS_SLAVE
        }
    else
        {
#ifdef DEBUG_PROFIBUS_SLAVE
        printf( "failed\n" );
#else
        sprintf( G_LOG->msg, "profibus_slave_PFC200: "
            "Stop the watchdog - failed." );
        G_LOG->write_log( i_log::P_CRIT );
#endif // DEBUG_PROFIBUS_SLAVE
        }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::close_device( tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

    printf( "-> Close the device...                                    " );

    iDalResult = pstAdi->Exit();

    if ( DAL_SUCCESS == iDalResult )
        {
        printf( "success\n" );
        }
    else
        {
        printf( "failed\n" );
        }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
int32_t profibus_slave_PFC200::release_ADI( tApplicationDeviceInterface* pstAdi )
    {
    int32_t iDalResult = DAL_FAILURE;
    if( NULL == pstAdi ) return iDalResult;

    printf( "-> Release the application device interface (ADI)...      " );

    iDalResult = pstAdi->Exit();

    if ( DAL_SUCCESS == iDalResult )
        {
        printf( "success\n" );
        }
    else
        {
        printf( "failed\n" );
        }

    return (iDalResult);
    }
//------------------------------------------------------------------------------
#endif
