#pragma once
#include "includes.h"

#include "cip_tech_def.h"
#include "PAC_dev.h"
#include <chrono>
#include <thread>

void InitStationParams()
    {
    printf("===================================Init station params");
    cipline_tech_object::set_station_par(1,1.5f);
    cipline_tech_object::set_station_par(2,0.3f);
    cipline_tech_object::set_station_par(3,0.1f);
    cipline_tech_object::set_station_par(4,1.0f);
    cipline_tech_object::set_station_par(5,0.2f);
    cipline_tech_object::set_station_par(6,0.1f);
    cipline_tech_object::set_station_par(7,20);
    cipline_tech_object::set_station_par(8,5);
    cipline_tech_object::set_station_par(9,0);
    cipline_tech_object::set_station_par(10,8);
    cipline_tech_object::set_station_par(11,1.51f);
    cipline_tech_object::set_station_par(12,1.8f);
    cipline_tech_object::set_station_par(13,47);
    cipline_tech_object::set_station_par(14,57);
    cipline_tech_object::set_station_par(15,1.47f);
    cipline_tech_object::set_station_par(16,1.45f);
    cipline_tech_object::set_station_par(17,1);
    cipline_tech_object::set_station_par(18,52);
    cipline_tech_object::set_station_par(19,0.5f);
    cipline_tech_object::set_station_par(20,30.8f);
    cipline_tech_object::set_station_par(21,0.4f);
    cipline_tech_object::set_station_par(22,30);
    cipline_tech_object::set_station_par(23,120);
    cipline_tech_object::set_station_par(24,50);
    cipline_tech_object::set_station_par(25,10000);
    cipline_tech_object::set_station_par(26,10000);
    cipline_tech_object::set_station_par(27,10000);
    cipline_tech_object::set_station_par(28,100);
    cipline_tech_object::set_station_par(29,100);
    cipline_tech_object::set_station_par(30,50);
    cipline_tech_object::set_station_par(31,0);
    cipline_tech_object::set_station_par(32,0);
    cipline_tech_object::set_station_par(33,12);
    cipline_tech_object::set_station_par(34,11);
    cipline_tech_object::set_station_par(35,20);
    }

void InitCipDevices( )
    {
    auto dm = device_manager::get_instance( );
    dm->clear_io_devices();
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V0", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V10", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V11", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V12", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V13", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V2", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V3", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V4", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V5", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V6", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V7", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V8", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V9", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "V101", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "V102", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_VC, device::DEVICE_SUB_TYPE::DST_VC_VIRT, "LINE1VC14", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_M, device::DEVICE_SUB_TYPE::DST_M_VIRT, "LINE1M1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS2", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS3", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS4", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS5", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS6", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS7", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS8", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS9", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_TE, device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_TE, device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE2", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_FS, device::DEVICE_SUB_TYPE::DST_FS_VIRT, "LINE1FS1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_FQT, device::DEVICE_SUB_TYPE::DST_FQT_VIRT, "LINE1FQT1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_QT, device::DEVICE_SUB_TYPE::DST_QT_VIRT, "LINE1QT1", "", "" );
    }

void ClearCipDevices( )
    {
    device_manager::get_instance( )->clear_io_devices();
    }