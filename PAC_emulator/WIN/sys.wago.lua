-- Show Table Content
local indent = ''
local function print_table(tbl, tbl_name)
    if tbl_name == nil then tbl_name = '.' end
    for fields, value in pairs(tbl) do
        if type(fields)=='string' then fields = "'"..fields.."'" end
        if type(value) == "table" then
            print(indent.."+", tbl_name.."["..fields.."] = ", value)
            indent = indent..'  '
            print_table(value, tbl_name.."["..fields.."]")
            indent = string.sub( indent, 3 )
        else
            print(indent.."-", tbl_name.."["..fields.."] = ", value)
        end
    end
end

modules_info = {}

modules_info[ 402 ] =
    {
    comment           = 'Дискретный вход',
    mtype             = 'DI',
    DI_chennels_count = 4,
    chennel_size      = 1 --Размер области отображения одного канала.
    }
modules_info[ 638 ] =
    {
    comment           = 'Счетчик импульсов',
    mtype             = 'SPECIAL',
    AI_chennels_count = 2,
    AO_chennels_count = 2,
    chennel_size      = 4
    }
modules_info[ 466 ] =
    {
    comment           = 'Температура',
    mtype             = 'AI',
    AI_chennels_count = 2,
    chennel_size      = 2
    }
modules_info[ 461 ] =
    {
    comment           = 'Текущий уровень',
    mtype             = 'AI',
    AI_chennels_count = 2,
    chennel_size      = 2
    }


--Спросить у Андрея характеристики модулей.
modules_info[ 504 ] =
    {
    comment           = '??',
    mtype             = 'DO',
    DO_chennels_count = 4
    }
modules_info[ 512 ] =
    {
    comment           = '??',
    mtype             = 'DO',
    DO_chennels_count = 4
    }
modules_info[ 554 ] =
    {
    comment           = '??',
    mtype             = 'AO',
    AO_chennels_count = 2,
    chennel_size      = 2
    }
modules_info[ 613 ] =
    {
    comment           = '??',
    mtype             = 'STUB'
    }
modules_info[ 612 ] =
    {
    comment           = '??',
    mtype             = 'STUB'
    }
modules_info[ 602 ] =
    {
    comment           = '??',
    mtype             = 'STUB'
    }
modules_info[ 600 ] =
    {
    comment           = '??',
    mtype             = 'STUB'
    }
modules_info[ 430 ] =
    {
    comment           = '??',
    mtype             = 'DI',
    DI_chennels_count = 4
    }
modules_info[ 530 ] =
    {
    comment           = '??',
    mtype             = 'DO',
    DO_chennels_count = 4
    }
modules_info[ 655 ] =
    {
    comment           = '??',
    mtype             = 'AIAO',
    AO_chennels_count = 2,
    AI_chennels_count = 2,
    chennel_size      = 2
    }
modules_info[ 455 ] =
    {
    comment           = '??',
    mtype             = 'AI',
    AI_chennels_count = 2,
    chennel_size      = 2
    }
modules_info[ 460 ] =
    {
    comment           = '??',
    mtype             = 'AI',
    AI_chennels_count = 2,
    chennel_size      = 2
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

            if modules_info[ value[ 1 ] ].DO_chennels_count then
                DO_cnt = DO_cnt +
                    modules_info[ value[ 1 ] ].DO_chennels_count
            end

            if modules_info[ value[ 1 ] ].AO_chennels_count then

                for i = AO_cnt, AO_cnt +
                    modules_info[ value[ 1 ] ].AO_chennels_count - 1 do

                    AO_local[ i + 1 ] = { value[ 1 ], AO_end_address }
                    AO_end_address = AO_end_address +
                        modules_info[ value[ 1 ] ].chennel_size
                end
                AO_cnt = AO_cnt +
                    modules_info[ value[ 1 ] ].AO_chennels_count
            end

            if modules_info[ value[ 1 ] ].DI_chennels_count then
                DI_cnt = DI_cnt +
                    modules_info[ value[ 1 ] ].DI_chennels_count
            end

            if modules_info[ value[ 1 ] ].AI_chennels_count then
                for i = AI_cnt, AI_cnt +
                    modules_info[ value[ 1 ] ].AI_chennels_count - 1 do

                    AI_local[ i + 1 ] = { value[ 1 ], AI_end_address }
                    AI_end_address = AI_end_address +
                        modules_info[ value[ 1 ] ].chennel_size
                end
                AI_cnt = AI_cnt +
                    modules_info[ value[ 1 ] ].AI_chennels_count
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
        DI_count = DI_cnt,
        AI_count = AI_cnt,
        AI       = AI_local
        }
end

G_SYSTEM =
    {
    get_file_version = function()
        return G_FILE_VERSION
    end,

    get_editor_version = function()
        return G_EDITOR_VERSION
    end,

    init_wago = function()
        local nodes_count = #G_NODES
        G_WAGO_MANAGER():init( nodes_count )

        for i = 1, nodes_count do
            local node_info = get_modules_info( G_NODES[ i ].modules )

            G_WAGO_MANAGER():add_node( i - 1,
                G_NODES[ i ].ntype, G_NODES[ i ].address,
                G_NODES[ i ].IP_address,
                node_info.DO_count, node_info.DI_count,
                node_info.AO_count, node_info.AI_count )

            for j = 1, node_info.AO_count do
                G_WAGO_MANAGER():init_node_AO( i - 1, j - 1,
                    node_info.AO[ j ][ 1 ], node_info.AO[ j ][ 2 ] )
            end

            for j = 1, node_info.AI_count do
                G_WAGO_MANAGER():init_node_AI( i - 1, j - 1,
                    node_info.AI[ j ][ 1 ], node_info.AI[ j ][ 2 ] )
            end
        end
    end,

    init_devices = function()
        local devices_count = #G_DEVICES

        for i = 1, devices_count do

            local device = G_DEVICES[ i ]

            local wago_device = G_DEVICE_MANAGER():add_device(
                device.dtype, device.subtype, device.number,
                device.descr )

            if wago_device ~= 0 then --Устройство имеет модули Wago.

                local DI_chennels = 0
                local DO_chennels = 0
                local AI_chennels = 0
                local AO_chennels = 0
                local par_count   = 0

                if device.DI ~= nil then
                    DI_chennels = #device.DI
                end
                if device.DO ~= nil then
                    DO_chennels = #device.DO
                end
                if device.AI ~= nil then
                    AI_chennels = #device.AI
                end
                if device.AO ~= nil then
                    AO_chennels = #device.AO
                end
                if device.par ~= nil then
                    par_count = #device.par
                end

                wago_device:init( DO_chennels, DI_chennels,
                    AO_chennels, AI_chennels, par_count )

                for j = 1, DI_chennels do
                    wago_device:init_channel( 1, j - 1,
                        device.DI[ j ].node, device.DI[ j ].offset )
                end
                for j = 1, DO_chennels do
                    wago_device:init_channel( 2, j - 1,
                        device.DO[ j ].node, device.DO[ j ].offset )
                end
                for j = 1, AI_chennels do
                    wago_device:init_channel( 3, j - 1,
                        device.AI[ j ].node, device.AI[ j ].offset )
                end
                for j = 1, AO_chennels do
                    wago_device:init_channel( 4, j - 1,
                        device.AO[ j ].node, device.AO[ j ].offset )
                end

            end --if endwago_device ~= 0 then
        end --for i = 1, devices_count do

        G_DEVICE_MANAGER():complete_init()
    end,
    }



-- local example =
    -- {
    -- { 466, '', '', '', '', '',  },
    -- { 466, '', '', '', '', '',  },
    -- { 402, '', '', '', '', '',  },
    -- { 402, '', '', '', '', '',  },
    -- { 402, '', '', '', '', '',  },
    -- { 402, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 504, '', '', '', '', '',  },
    -- { 638, '', '', '', '', '',  },
    -- { 461, '', '', '', '', '',  },
    -- { 461, '', '', '', '', '',  },
    -- { 461, '', '', '', '', '',  },
    -- { 612, '', '', '', '', '',  },
    -- { 512, '', '', '', '', '',  },
    -- { 512, '', '', '', '', '',  },
    -- { 602, '', '', '', '', '',  },
    -- { 402, '', '', '', '', '',  },
    -- { 600, '', '', '', '', '',  },
    -- }
-- print_table( get_modules_info( example ), 'm_info' )
