--version = 1

system = system or {}

-- ----------------------------------------------------------------------------
--Инициализация имен устройств.
system.init_devices_names = function( self )

    local devices_count = #devices

    for i = 1, devices_count do

        local device = G_DEVICE_MANAGER():get_device(
            devices[ i ].dtype, devices[ i ].name )

        if device ~= nil then
            device:set_descr( devices[ i ].descr or "" )
        end --if

    end --for i = 1, devices_count do
end
-- ----------------------------------------------------------------------------
--Инициализация свойств устройств.
system.init_devices_properties = function( self )

    local devices_count = #devices

    for i = 1, devices_count do

        local device_descr = devices[ i ]

        --Есть дополнительные свойства.
        if device_descr.prop ~= nil then

            local device = G_DEVICE_MANAGER():get_device(
                devices[ i ].dtype, devices[ i ].name )

            if device ~= nil then

                for field, value in pairs( device_descr.prop ) do
                    if field == MT then
                        if value ~= '' then
                            assert( loadstring( "dev = __"..value ) )( )
                            if dev == nil then
                                error( "Unknown device '"..
                                    value.."' (__"..value..")." )
                            end
                            device:set_property( field, dev )
                        end
                    else
                        device:set_string_property( field, value )
                    end
                end
            end
        end --if
    end --for i = 1, devices_count do
end
-- ----------------------------------------------------------------------------
--Инициализация параметров устройств.
system.init_devices_params = function( self )

    local devices_count = #devices

    for i = 1, devices_count do

        local device_descr = devices[ i ]

        local device = G_DEVICE_MANAGER():get_device(
            devices[ i ].dtype, devices[ i ].name )

        if device ~= nil then

            local par_count = 0
            if device_descr.par ~= nil then
                par_count = #device_descr.par
            end

            for j = 1, par_count do
                device:set_par( j, 0, device_descr.par[ j ] )
            end

        end --if
    end --for i = 1, devices_count do
end
-- ----------------------------------------------------------------------------
--Инициализация рабочих параметров устройств.
system.init_devices_rt_params = function( self )

    local devices_count = #devices

    for i = 1, devices_count do

        local device_descr = devices[ i ]

        local device = G_DEVICE_MANAGER():get_device(
            devices[ i ].dtype, devices[ i ].name )

        if device ~= nil then
            local rt_par_count = 0
            if device_descr.rt_par ~= nil then
                rt_par_count = #device_descr.rt_par
            end

            for j = 1, rt_par_count do
                device:set_rt_par( j, device_descr.rt_par[ j ] )
            end

        end --if
    end --for i = 1, devices_count do
end
