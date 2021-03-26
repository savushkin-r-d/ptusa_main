--version  = 1
--PAC_name = 'DEMO'
-- ----------------------------------------------------------------------------
-- ----------------------------------------------------------------------------
init_tech_objects_modes = function()
    return
    {
        {
        n          = 1,
        tech_type  = 2,
        name       = 'Tank',
        name_eplan = 'TANK',
        name_BC    = 'TankObj',
        cooper_param_number = -1,
        timers = 2,
        par_float =
            {
            [ 1 ] =
                {
                name = 'Резерв 1',
                value = 0,
                meter = 'na',
                oper = -1,
                nameLua = 'PP_RESERV1'
                },
            },
        modes =
            {
            }
        }
    }
end
