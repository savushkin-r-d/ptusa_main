#pragma once
#include "includes.h"

#include "PAC_info.h"
#include "device/manager.h"
#include "device/life_device.h"
#include "uni_bus_coupler_io.h"

/// @brief Test fixture for isolating I/O manager state in PAC_info tests.
class PAC_info_io_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            prev_mngr = io_manager::replace_instance( &mngr );
            }

        void TearDown() override
            {
            io_manager::replace_instance( prev_mngr );
            G_DEVICE_MANAGER()->clear_io_devices();
            G_PAC_INFO()->emulation_on();
            }

        uni_io_manager mngr;
        io_manager* prev_mngr = nullptr;
    };
