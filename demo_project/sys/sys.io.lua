--version = 1

modules_info = {}

--WAGO
modules_info[ 402 ] =
    {
    comment           = 'Дискретный вход',
    mtype             = 'DI',
    DI_channels_count = 4,
    channel_size      = 1 --Размер области отображения одного канала, байты.
    }

modules_info[ 1420 ] =
    {
    comment           = 'Дискретный вход 24 VDC',
    mtype             = 'DI',
    DI_channels_count = 4,
    channel_size      = 1
    }

modules_info[ 1415 ] =
    {
    comment           = 'Дискретный вход 24 VDC',
    mtype             = 'DI',
    DI_channels_count = 8,
    channel_size      = 1
    }

modules_info[ 1405 ] =
    {
    comment           = 'Дискретный вход 24 VDC',
    mtype             = 'DI',
    DI_channels_count = 16,
    channel_size      = 1
    }

modules_info[ 1504 ] =
    {
    comment           = 'Дискретный выход 24 VDC',
    mtype             = 'DO',
    DO_channels_count = 16,
    channel_size      = 1
    }

modules_info[ 1515 ] =
    {
    comment           = 'Дискретный выход 24 VDC',
    mtype             = 'DO',
    DO_channels_count = 8,
    channel_size      = 1
    }

modules_info[ 638 ] =
    {
    comment           = 'Счетчик импульсов',
    mtype             = 'SPECIAL',
    AI_channels_count = 2,
    AO_channels_count = 2,
    channel_size      = 4
    }
modules_info[ 450 ] =
    {
    comment           = 'Температура RTDs',
    mtype             = 'AI',
    AI_channels_count = 4,
    channel_size      = 2
    }
modules_info[ 466 ] =
    {
    comment           = 'Температура',
    mtype             = 'AI',
    AI_channels_count = 2,
    channel_size      = 2
    }
modules_info[ 496 ] =
    {
    comment           = 'Температура',
    mtype             = 'AI',
    AI_channels_count = 8,
    channel_size      = 2
    }
modules_info[ 461 ] =
    {
    comment           = 'Текущий уровень',
    mtype             = 'AI',
    AI_channels_count = 2,
    channel_size      = 2
    }


modules_info[ 504 ] =
    {
    comment           = 'Дискретный выход, 24 VDC, 0.5 A',
    mtype             = 'DO',
    DO_channels_count = 4
    }
modules_info[ 512 ] =
    {
    comment           = 'Дискретный выход, AC 250 V, 2.0 A, Relay',
    mtype             = 'DO',
    DO_channels_count = 2
    }
modules_info[ 554 ] =
    {
    comment           = 'Аналоговый вывод',
    mtype             = 'AO',
    AO_channels_count = 2,
    channel_size      = 2
    }
modules_info[ 555 ] =
    {
    comment           = 'Аналоговый вывод',
    mtype             = 'AO',
    AO_channels_count = 4,
    channel_size      = 2
    }
modules_info[ 613 ] =
    {
    comment           = 'Internal System Supply Module 24 VDC',
    mtype             = 'STUB'
    }
modules_info[ 612 ] =
    {
    comment           = 'Power Supply; 0 … 230 V AC/DC',
    mtype             = 'STUB'
    }
modules_info[ 602 ] =
    {
    comment           = 'Power Supply; 24 VDC',
    mtype             = 'STUB'
    }
modules_info[ 600 ] =
    {
    comment           = 'End Module',
    mtype             = 'STUB'
    }
modules_info[ 430 ] =
    {
    comment           = '8-channel digital input; 24 VDC; 3 ms',
    mtype             = 'DI',
    DI_channels_count = 8
    }
modules_info[ 530 ] =
    {
    comment           = '8-channel digital output; 24 VDC; 0.5 A',
    mtype             = 'DO',
    DO_channels_count = 8
    }
modules_info[ 655 ] =
    {
    comment           = 'AS Interface Master',
    mtype             = 'AIAO',
    AO_channels_count = 20,
    AI_channels_count = 20,
    channel_size      = 2
    }
modules_info[ 455 ] =
    {
    comment           = '4-channel analog input; Single-ended',
    mtype             = 'AI',
    AI_channels_count = 2,
    channel_size      = 2
    }
modules_info[ 460 ] =
    {
    comment           = '4-channel analog input; For Pt100/RTD sensors',
    mtype             = 'AI',
    AI_channels_count = 2,
    channel_size      = 2
    }
modules_info[ 627 ] =
    {
    comment           = 'Bus Extension End Module',
    mtype             = 'STUB'
    }
modules_info[ 628 ] =
    {
    comment           = 'Bus Extension Coupler Module',
    mtype             = 'STUB'
    }
modules_info[ 491 ] =
    {
    comment           = '1-channel analog input; Resistor bridges',
    mtype             = 'AI',
    AI_channels_count = 2,
    channel_size      = 2
    }
modules_info[ 657 ] =
    {
    comment           = 'IO-Link Master',
    mtype             = 'AIAO',
    AO_channels_count = 12,
    AI_channels_count = 12,
    channel_size      = 2
    }

--PHOENIX
modules_info[ 1027843 ] =
    {
    comment           = 'IO-Link Master',
    mtype             = 'AIAO',
    AO_channels_count = 32,
    AI_channels_count = 32,
    DO_channels_count = 512,
    DI_channels_count = 512,

    channel_size      = 2,
    }

modules_info[ 2688022 ] =
    {
    comment           = 'DI 16/4',
    mtype             = 'AIAO',
    AO_channels_count = 1,
    AI_channels_count = 1,
    DO_channels_count = 16,
    DI_channels_count = 16,

    channel_size      = 2,
    }

modules_info[ 2701916 ] =
    {
    comment           = 'DI 8/1 DO 8/1 1H',
    mtype             = 'AIAO',
    AO_channels_count = 1,
    AI_channels_count = 1,
    DO_channels_count = 16,
    DI_channels_count = 16,

    channel_size      = 2,
    }

modules_info[ 2702071 ] =
    {
    comment           = 'DI 8/3 DO 8/3 2H',
    mtype             = 'AIAO',
    AO_channels_count = 1,
    AI_channels_count = 1,
    DO_channels_count = 16,
    DI_channels_count = 16,

    channel_size      = 2,
    }

modules_info[ 2688491 ] =
    {
    comment           = 'AI4 I 1H',
    mtype             = 'AIAO',
    AO_channels_count = 4,
    AI_channels_count = 4,
    DO_channels_count = 64,
    DI_channels_count = 64,

    channel_size      = 2,
    }

modules_info[ 2688527 ] =
    {
    comment           = 'AO4 1H',
    mtype             = 'AIAO',
    AO_channels_count = 4,
    AI_channels_count = 4,
    DO_channels_count = 64,
    DI_channels_count = 64,

    channel_size      = 2,
    }

modules_info[ 2688048 ] =
    {
    comment           = 'DO16/3 2F',
    mtype             = 'AIAO',
    AO_channels_count = 1,
    AI_channels_count = 1,
    DO_channels_count = 16,
    DI_channels_count = 16,

    channel_size      = 2,
    }

modules_info[ 2688556 ] =
    {
    comment           = 'RTD4 1H',
    mtype             = 'AIAO',
    AO_channels_count = 4,
    AI_channels_count = 4,
    DO_channels_count = 64,
    DI_channels_count = 64,

    channel_size      = 2,
    }

modules_info[ 2688093 ] =
    {
    comment           = 'CNT2 INC2 1F',
    mtype             = 'AIAO',
    AO_channels_count = 14,
    AI_channels_count = 14,
    DO_channels_count = 224,
    DI_channels_count = 224,

    channel_size      = 2,
    }

--PHOENIX SMART MODULES
modules_info[ 1088127 ] =
    {
    comment           = 'DI16/1',
    mtype             = 'AIAO',
    AO_channels_count = 1,
    AI_channels_count = 1,
    DO_channels_count = 16,
    DI_channels_count = 16,

    channel_size      = 2,
    }

modules_info[ 1088129 ] =
    {
    comment           = 'DO16/1',
    mtype             = 'AIAO',
    AO_channels_count = 1,
    AI_channels_count = 1,
    DO_channels_count = 16,
    DI_channels_count = 16,

    channel_size      = 2,
    }

modules_info[ 1088062 ] =
    {
    comment           = 'AI4 I 4-20',
    mtype             = 'AIAO',
    AO_channels_count = 4,
    AI_channels_count = 4,
    DO_channels_count = 64,
    DI_channels_count = 64,

    channel_size      = 2,
    }

modules_info[ 1088123 ] =
    {
    comment = 'AO4 I 4-20',
    mtype             = 'AIAO',
    AO_channels_count = 4,
    AI_channels_count = 4,
    DO_channels_count = 64,
    DI_channels_count = 64,

    channel_size      = 2,
    }

modules_info[ 1088106 ] =
    {
    comment           = 'RTD4 PT100',
    mtype             = 'AIAO',
    AO_channels_count = 4,
    AI_channels_count = 4,
    DO_channels_count = 64,
    DI_channels_count = 64,

    channel_size      = 2,
    }

modules_info[ 1088132 ] =
    {
    comment           = 'IOL4',
    mtype             = 'AIAO',
    AO_channels_count = 32,
    AI_channels_count = 32,
    DO_channels_count = 512,
    DI_channels_count = 512,

    channel_size      = 2,
    }

modules_info[ 1088131 ] =
    {
    comment           = 'CNT1',
    mtype             = 'AIAO',
    AO_channels_count = 3,
    AI_channels_count = 3,
    DO_channels_count = 48,
    DI_channels_count = 48,

    channel_size      = 2,
    }

modules_info[ 1088134 ] =
    {
        comment = 'SC-A',
        mtype = 'STUB',
    }


function get_modules_info ( modules )
    local DO_cnt      = 0
    local AO_cnt      = 0
    local AO_local    = {}
    local DI_cnt      = 0
    local AI_cnt      = 0
    local AI_local    = {}
    local modules_cnt = 0

    if modules == nil or type( modules ) ~= "table" then
        return  { modules_count = 0 }
    end

    local AO_end_address = 0
    local AI_end_address = 0

    for fields, value in pairs( modules ) do
        if modules_info[ value[ 1 ] ] ~= nil then

            if modules_info[ value[ 1 ] ].DO_channels_count then
                DO_cnt = DO_cnt +
                    modules_info[ value[ 1 ] ].DO_channels_count
            end

            if modules_info[ value[ 1 ] ].AO_channels_count then

                for i = AO_cnt, AO_cnt +
                    modules_info[ value[ 1 ] ].AO_channels_count - 1 do

                    AO_local[ i + 1 ] = { value[ 1 ], AO_end_address }
                    AO_end_address = AO_end_address +
                        modules_info[ value[ 1 ] ].channel_size
                end
                AO_cnt = AO_cnt +
                    modules_info[ value[ 1 ] ].AO_channels_count
            end

            if modules_info[ value[ 1 ] ].DI_channels_count then
                DI_cnt = DI_cnt +
                    modules_info[ value[ 1 ] ].DI_channels_count
            end

            if modules_info[ value[ 1 ] ].AI_channels_count then
                for i = AI_cnt, AI_cnt +
                    modules_info[ value[ 1 ] ].AI_channels_count - 1 do

                    AI_local[ i + 1 ] = { value[ 1 ], AI_end_address }
                    AI_end_address = AI_end_address +
                        modules_info[ value[ 1 ] ].channel_size
                end
                AI_cnt = AI_cnt +
                    modules_info[ value[ 1 ] ].AI_channels_count
            end

            modules_cnt = modules_cnt + 1
        else
            print( "Unknown module №", fields, " - '", value[ 1 ], "'!")
        end

    end

    return
        {
        modules_count = modules_cnt,
        DO_count = DO_cnt,
        AO_count = AO_cnt,
        AO       = AO_local,
        AO_size  = AO_end_address,
        DI_count = DI_cnt,
        AI_count = AI_cnt,
        AI       = AI_local,
        AI_size  = AI_end_address,
        }
end

system =
    {
    get_PAC_name_rus = function( self )
        return PAC_name or 'Новый проект'
    end,

    get_PAC_name_eng = function( self )
        return PAC_name_eng or 'new_project'
    end,

    get_PAC_id = function( self )
        if PAC_id == nil then return 1 end
        return PAC_id
    end,

    create_io = function( self )

        local nodes_count = #nodes
        G_IO_MANAGER():init( nodes_count )

        for i = 1, nodes_count do
            local node_info = get_modules_info( nodes[ i ].modules )

            G_IO_MANAGER():add_node( i - 1,
                nodes[ i ].ntype, nodes[ i ].n,
                nodes[ i ].IP,
                nodes[ i ].name or 'Axxx',
                node_info.DO_count, node_info.DI_count,
                node_info.AO_count, node_info.AO_size,
                node_info.AI_count, node_info.AI_size )

            for j = 1, node_info.AO_count do
                G_IO_MANAGER():init_node_AO( i - 1, j - 1,
                    node_info.AO[ j ][ 1 ], node_info.AO[ j ][ 2 ] )
            end

            for j = 1, node_info.AI_count do
                G_IO_MANAGER():init_node_AI( i - 1, j - 1,
                    node_info.AI[ j ][ 1 ], node_info.AI[ j ][ 2 ] )
            end
        end
    end,

    create_devices = function( self )

        G_DEVICE_MANAGER()
        local devices_count = #devices

        for i = 1, devices_count do

            local device_descr = devices[ i ]

            local io_device = G_DEVICE_MANAGER():add_io_device(
                device_descr.dtype, device_descr.subtype, device_descr.name,
                device_descr.descr, device_descr.article or "" )

            if io_device ~= nil then --Устройство имеет модули I/O.

                local DI_channels = 0
                local DO_channels = 0
                local AI_channels = 0
                local AO_channels = 0

                if device_descr.DI ~= nil then
                    DI_channels = #device_descr.DI
                end
                if device_descr.DO ~= nil then
                    DO_channels = #device_descr.DO
                end
                if device_descr.AI ~= nil then
                    AI_channels = #device_descr.AI
                end
                if device_descr.AO ~= nil then
                    AO_channels = #device_descr.AO
                end

                io_device:init( DO_channels, DI_channels,
                    AO_channels, AI_channels )

                local io_vendor = io_device.WAGO
                local module_offset
                local logical_port
                for j = 1, DI_channels do
                    if device_descr.DI[ j ].module_offset then
                        module_offset = device_descr.DI[ j ].module_offset
                    else
                        module_offset = -1
                    end
                    if device_descr.DI[ j ].logical_port then
                        logical_port = device_descr.DI[ j ].logical_port
                    else
                        logical_port = -1
                    end
                    io_device:init_channel( 1, j - 1,
                        device_descr.DI[ j ].node, device_descr.DI[ j ].offset,
                        module_offset, logical_port )

                    if nodes [ device_descr.DI[ j ].node + 1 ].ntype >= 200 then
                        io_vendor = io_device.PHOENIX
                    end
                end
                for j = 1, DO_channels do
                    if device_descr.DO[ j ].module_offset then
                        module_offset = device_descr.DO[ j ].module_offset
                    else
                        module_offset = -1
                    end
                    if device_descr.DO[ j ].logical_port then
                        logical_port = device_descr.DO[ j ].logical_port
                    else
                        logical_port = -1
                    end
                    io_device:init_channel( 2, j - 1,
                        device_descr.DO[ j ].node, device_descr.DO[ j ].offset,
                        module_offset, logical_port )

                    if nodes [ device_descr.DO[ j ].node + 1 ].ntype >= 200 then
                        io_vendor = io_device.PHOENIX
                    end
                end
                for j = 1, AI_channels do
                    if device_descr.AI[ j ].module_offset then
                        module_offset = device_descr.AI[ j ].module_offset
                    else
                        module_offset = -1
                    end
                    if device_descr.AI[ j ].logical_port then
                        logical_port = device_descr.AI[ j ].logical_port
                    else
                        logical_port = -1
                    end
                    io_device:init_channel( 3, j - 1,
                        device_descr.AI[ j ].node, device_descr.AI[ j ].offset,
                        module_offset, logical_port )

                    if nodes [ device_descr.AI[ j ].node + 1 ].ntype >= 200 then
                        io_vendor = io_device.PHOENIX
                    end
                end
                for j = 1, AO_channels do
                    if device_descr.AO[ j ].module_offset then
                        module_offset = device_descr.AO[ j ].module_offset
                    else
                        module_offset = -1
                    end
                    if device_descr.AO[ j ].logical_port then
                        logical_port = device_descr.AO[ j ].logical_port
                    else
                        logical_port = -1
                    end
                    io_device:init_channel( 4, j - 1,
                        device_descr.AO[ j ].node, device_descr.AO[ j ].offset,
                        module_offset, logical_port )

                    if nodes [ device_descr.AO[ j ].node + 1 ].ntype >= 200 then
                        io_vendor = io_device.PHOENIX
                    end
                end

                io_device:set_io_vendor( io_vendor )
            end --if io_device ~= nil then --Устройство имеет модули I/O.
        end --for i = 1, devices_count do
    end
    }
