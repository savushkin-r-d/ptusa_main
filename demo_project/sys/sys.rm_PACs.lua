--version = 1

system = system or {}

--Инициализация работы с удаленными PAC.
system.init_rm_PACs = function( self )

    local PACs_count = #rm_PACs

    for i = 1, PACs_count do

        local rm_PAC_descr = rm_PACs[ i ]

        local name          = rm_PAC_descr.name or "?"
        local ip            = rm_PAC_descr.ip   or "192.168.1.1"
        local remote_PAC_id = rm_PAC_descr.id   or 1

        G_RM_MANAGER():add_rm_cmmctr( name, ip, remote_PAC_id )
    end --for i = 1, devices_count do
end

--Менеджер удаленных объектов. При обращении к несуществующим объектам
--возвращаем 0.
rm = {}

rm_meta = {}
rm_meta.__index = function (_, key)
	return 0
end

a = { b = 10 }

setmetatable( rm, rm_meta )
