-- ----------------------------------------------------------------------------
-- Проект "Выдача сыворотки".
-- ----------------------------------------------------------------------------
DEBUG = true
-- ----------------------------------------------------------------------------
-- Общее описание.
-- ----------------------------------------------------------------------------
--Индексы объектов.
CONST =
    {
    COMB_IDX  = 0,

    TANK1_IDX = 1,
    TANK2_IDX = 2,
    TANK3_IDX = 3,
    TANK4_IDX = 4,

    POST1_IDX = 5,
    POST2_IDX = 6,
    }

--Команды.
CMD =
    {
    RESET_POST                 = 30, -- Выключает пауза/работу, включает Выдача завершена.
    SET_PAUSE_AND_HEATING      = 31, -- Включает пауза и подогрев.
    RESET_TANK_POST1           = 32, -- Выключает режим выдачи для танка и, если надо, поста 1.
    RESET_TANK_POST2           = 33, -- Выключает режим выдачи для танка и, если надо, поста 2.

    SET_POST1_AND_TANK         = 34, -- Включить танк и пост 1.
    SET_POST2_AND_TANK         = 35, -- Включить танк и пост 2.
    SET_HEATING_POST1_AND_TANK = 36, -- Включить танк и подогрев пост 1.
    SET_HEATING_POST2_AND_TANK = 37, -- Включить танк и подогрев пост 2.
    }
-- ----------------------------------------------------------------------------
-- Описание - танк сыворотки.
-- ----------------------------------------------------------------------------
whey_tank = project_tech_object:new()

whey_tank.states_count               = 20
whey_tank.timers_count               = 5
whey_tank.runtime_params_float_count = 10

--Параметры.
whey_tank.PAR =
    {
    WARNING_REASON = 4
    }

--Ошибки.
whey_tank.WARN =
    {
    TANK_IN_STOP  = 1,
    TANK_IN_START = 2
    }

--Режимы.
whey_tank.MODES =
    {
    W_ACCEPTING  = 0, --Приёмка сыворотки.
    W_OUT_P1     = 1, --Выдача сыворотки пост 1.
    W_OUT_P2     = 2, --Выдача сыворотки пост 2.
    WASH         = 3, --Мойка.

    W_WACCEPTING = 8, --Ожидание приёмки сыворотки.
    W_WOUT_P1    = 9, --Ожидание выдачи сыворотки пост 1.
    W_WOUT_P2    = 10,--Ожидание выдачи сыворотки пост 2.
    }


-- ----------------------------------------------------------------------------
-- Работа танка сыворотки.
-- ----------------------------------------------------------------------------
local function on_whey_out( self, out_post )
    self.V1:on()

    if ( out_post == POST1 and
        ( 1 == self.number or 2 == self.number ) ) or
        ( out_post == POST2 and
        ( 3 == self.number or 4 == self.number ) ) then
        self.V3:off()
        if out_post:get_mode( post.MODES.W_ACCEPTING ) == 1 then
            self.V4:on()
        else
            self.V4:off()
        end
    else
        self.V3:on()
        self.V4:off()

        V( 107 ):off()
        V( 207 ):on()
        V( 206 ):off()
        V( 307 ):on()
        V( 507 ):off()

        self.V_1:off()
        self.V_2:off()
        self.V_3:on()
        if out_post:get_mode( post.MODES.W_ACCEPTING ) == 1 then
            self.V_4:on()
        else
            self.V_4:off()
        end
    end

    if self.LSL:get_state() == LNO or
        ( out_post:get_mode( post.MODES.W_ACCEPTING ) == 1 and
        out_post.flow:get_state() == 0 ) then
        local idx = G_TECH_OBJECT_MNGR():get_object_with_active_mode(
            self.MODES.W_WOUT_P1, CONST.TANK1_IDX, CONST.TANK2_IDX )

        if idx >= 0 and
            self.LE1:get_value() <
            out_post.par_float[ post.RT_F_PAR.LE_MIN ] then
            self:set_mode( self.MODES.W_OUT_P1, 0 )
        else
            out_post:set_mode( post.MODES.W_ACCEPTING_PAUSE, 1 )
            out_post.rt_par_float[
                post.RT_F_PAR.WARNING_REASON ] = post.WARN.NO_FLOW
        end
    end
end
-- ----------------------------------------------------------------------------
function whey_tank:init()
    --Клапана.
    self.V1 = V( self.number * 100 + 1 )
    self.V2 = V( self.number * 100 + 2 )
    self.V3 = V( self.number * 100 + 3 )
    self.V4 = V( self.number * 100 + 4 )
    self.V5 = V( self.number * 100 + 5 )
    self.V7_1 = ( 1 == self.number or 2 == self.number )
        and V( 107 )  or V( 507 )
    self.V7_2 = ( 1 == self.number or 2 == self.number )
        and V( 207 )  or V( 307 )
    self.LSL = LS( self.number * 100 + 1 )
    self.LE1 = LE( self.number )
    self.LSH = LS( self.number * 100 + 2 )
    self.T = TE( self.number )

    self.V_1 = STUB()
    self.V_2 = STUB()
    self.V_3 = STUB()
    self.V_4 = STUB()
end
-- ----------------------------------------------------------------------------
function whey_tank:final_mode( mode )
    if mode == self.MODES.W_ACCEPTING then
        self.V1:off()
        self.V2:off()
    end

    if mode == self.MODES.W_OUT_P1 then
        self.V1:off()

        if 1 == number or 2 == number then
            self.V4:off()
        else
            self.V3:off()
            V( 207 ):off()
            V( 307 ):off()
            self.V_3:off()
            self.V_4:off()
        end
    end

    if mode == self.MODES.W_OUT_P2 then
        self.V1:off()

        if 3 == number or 4 == number then
            self.V4:off()
        else
            self.V3:off()

            V( 207 ):off()
            V( 307 ):off()

            self.V_3:off()
            self.V_4:off()
        end
    end

    if mode == self.MODES.WASH then
        self.V1:off()
        self.V3:off()
        self.V5:off()

        self.V7_2:off()
        V( 206 ):off()
        V( 208 ):off()
        V( 109 ):off()
        N( 2202 ):off()
    end

    --При отключении приёмки/выдачи включаем при наличии ожидающие
    --приёмку/выдачу.
    if mode >= self.MODES.W_ACCEPTING and
        mode <= self.MODES.W_OUT_P2 then
        idx = G_TECH_OBJECT_MNGR():get_object_with_active_mode(
            mode + self.MODES.W_WACCEPTING, CONST.TANK1_IDX,
            CONST.TANK2_IDX )

        if idx >= 0 then
            local post_obj = G_TECH_OBJECTS( idx )
            post_obj:set_mode( mode + self.MODES.W_WACCEPTING, 0 )
            post_obj:set_mode( mode, 1 )
            if self.MODES.W_ACCEPTING == mode then
                post_obj.rt_par_float[ self.PAR.WARNING_REASON ] =
                    self.WARN.TANK_IN_START
            end
        else
            if self.MODES.W_ACCEPTING == mode then
                self.rt_par_float[ self.PAR.WARNING_REASON ] =
                    self.WARN.TANK_IN_STOP
            end
        end
    end
    --При отключении приёмки/выдачи включаем при наличии ожидающие
    --приёмку/выдачу-!>

    return 0
end
-- ----------------------------------------------------------------------------
function whey_tank:evaluate( par )
    for i = 0, self:get_modes_count() do

        if self:get_mode( i ) == 1 then

            if i == self.MODES.W_ACCEPTING then
                self.V1:on()
                self.V2:on()
                self.V4:off()
                V( 106 ):off()

                if self.LSH:get_state() == LIS then
                    self:set_mode( self.MODES.W_ACCEPTING, 0 )
                end

            elseif i == self.MODES.W_OUT_P1 then
                on_whey_out( self, POST1 )

            elseif i == self.MODES.W_OUT_P2 then
                on_whey_out( self, POST2 )

            elseif i == self.MODES.WASH then
                self.V1:on()
                self.V2:off()
                self.V3:on()
                self.V4:off()
                self.V5:on()
                self.V7_1:off()
                self.V7_2:on()
                V( 108 ):off()
                V( 206 ):on()
                V( 208 ):on()
                V( 109 ):on()
                N( 2202 ):on()

            elseif i == self.MODES.W_WACCEPTING then

            elseif i == self.MODES.W_WOUT_P1 then

            elseif i == self.MODES.W_WOUT_P2 then
            end
        end
    end

    return 0
end
-- ----------------------------------------------------------------------------
function whey_tank:init_mode( mode )
    if mode == self.MODES.W_OUT_P1 then
        if 3 == self.number or 4 == self.number then
            --В зависимости от того, в какой танк идёт приёмка ( или ни
            --в какой ), определяем клапаны перехода.
            self.V_1 = V( 101 )
            self.V_2 = V( 102 )
            self.V_3 = V( 103 )
            self.V_4 = V( 104 )
        else
            self.V_1 = STUB()
            self.V_2 = STUB()
            self.V_3 = STUB()
            self.V_4 = STUB()
        end

    end

    if mode == self.MODES.W_OUT_P2 then
        if 1 == self.number or 2 == self.number then
            --В зависимости от того, в какой танк идёт приёмка ( или ни
            --в какой ), определяем клапаны перехода.
            self.V_1 = V( 401 )
            self.V_2 = V( 402 )
            self.V_3 = V( 403 )
            self.V_4 = V( 404 )
        else
            self.V_1 = STUB()
            self.V_2 = STUB()
            self.V_3 = STUB()
            self.V_4 = STUB()
        end

    end
end
-- ----------------------------------------------------------------------------
function whey_tank:exec_cmd( cmd )
    local idx = -1

    if cmd == CMD.RESET_TANK_POST1 then
        idx = G_TECH_OBJECT_MNGR():get_object_with_active_mode(
            self.MODES.W_WOUT_P1, CONST.TANK1_IDX, CONST.TANK2_IDX )

        if idx == -1 then
            POST1:exec_cmd( CMD.RESET_POST ) -- Сбрасываем пост 1.
        end
        self:set_mode( self.MODES.W_OUT_P1, 0 )
    end

    if cmd == CMD.RESET_TANK_POST2 then
        idx = G_TECH_OBJECT_MNGR():get_object_with_active_mode(
            self.MODES.W_WOUT_P2, CONST.TANK1_IDX, CONST.TANK2_IDX )

        if idx == -1 then
            POST2:exec_cmd( CMD.RESET_POST ) -- Сбрасываем пост 2.
        end
        self:set_mode( self.MODES.W_OUT_P2, 0 )
    end

    if cmd == CMD.SET_POST1_AND_TANK then
        if self:get_mode( self.MODES.WASH ) == 1 or   -- Во время мойки танка нельзя.
            self:get_mode( self.MODES.W_OUT_P2 ) == 1 or
            self:get_mode( self.MODES.W_WOUT_P2 ) == 1 then
            return 1
        end

        if self:set_mode( self.MODES.W_OUT_P1, 1 ) ==
            1000 + self.MODES.W_OUT_P1 then
            POST1:set_mode( POST1.MODES.W_ACCEPTING_PAUSE, 1 )
        end
    end

    if cmd == CMD.SET_POST2_AND_TANK then
        if self:get_mode ( self.MODES.WASH ) == 1 or   -- Во время мойки танка нельзя.
            self:get_mode( self.MODES.W_OUT_P1 ) == 1 or
            self:get_mode( self.MODES.W_WOUT_P1 ) == 1 then
            return 1
        end

        if self:set_mode( self.MODES.W_OUT_P2, 1 ) ==
            1000 + self.MODES.W_OUT_P2 then
            POST2:set_mode( POST1.MODES.W_ACCEPTING_PAUSE, 1 )
        end
    end

    if cmd == CMD.SET_HEATING_POST1_AND_TANK then
        self:exec_cmd( CMD.SET_POST1_AND_TANK )
        POST1:set_mode( POST1.MODES.W_HEATING, 1 )
    end

    if cmd == CMD.SET_HEATING_POST2_AND_TANK then
        self:exec_cmd( CMD.SET_POST2_AND_TANK )
        POST2:set_mode( POST1.MODES.W_HEATING, 1 )
    end

    self.rt_par_float[ self.PAR.WARNING_REASON ] = 0
    return 1000 + cmd
end
-- ----------------------------------------------------------------------------
-- Описание - пост.
-- ----------------------------------------------------------------------------
post = project_tech_object:new()

post.states_count               = 20
post.timers_count               = 5
post.params_float_count         = 5
post.runtime_params_float_count = 20

--Параметры времени выполнения типа float.
post.RT_F_PAR =
    {
    SECTION1      = 1, -- Ёмкость секции 1.
    SECTION2      = 2,
    SECTION3      = 3,
    SECTION4      = 4,
    SECTION5      = 5,
    SECTION6      = 6,
    SECTION7      = 7,
    SECTION8      = 8,
    SECTION9      = 9,
    SECTION10     = 10,
    AUTO_NUM      = 11, -- Номер машины.
    CURRENT_SEC   = 12, -- Текущая секция.
    SECTIONS      = 13, -- Общее число секций.
    TOT_VOL       = 14, -- Общий объём.
    IS_RESET_POST = 15, -- При включении паузы сбросить счётчик.

    WARNING_REASON = 17
    }
--Сохряняемые параметры типа float.
post.S_F_PAR =
    {
    -- Минимальный уровень в танке, ниже которого переходим к
    -- следующему танку очереди.
    LE_MIN         = 1,

    -- Максимальная температура выдачи, при превышении которой
    -- возникает аварийная ситуация.
    T_OUT_MAX      = 2,

    -- Температура выдачи, при превышении которой закрывается паровой
    -- клапан подогрева.
    OUT_NORMAL     = 3,

    PAUSE_TIME_MAX = 4 -- Максимальное время режима ПАУЗА, мин.
    }

--Ошибки.
post.WARN =
    {
    MAX_OUT_TEMPER  = 1,
    NO_FLOW         = 2,
    PAUSE_TIME_LEFT = 3
    }

--Режимы.
post.MODES =
    {
    W_HEATING         = 16, -- Подогрев сыворотки постом.
    W_ACCEPTING       = 17, -- Приёмка сыворотки постом.
    W_ACCEPTING_PAUSE = 18, -- Пауза приёмки сыворотки постом.
    W_ACCEPTING_END   = 19, -- Завершение приёмки сыворотки постом.
    }

--Таймеры.
post.TMR =
    {
    DELAY_TEMP = 1,
    PAUSE_TIME = 2
    }


-- ----------------------------------------------------------------------------
-- Работа поста.
-- ----------------------------------------------------------------------------
local function on_heating( self )
    if self:get_mode( post.MODES.W_ACCEPTING_PAUSE ) == 1 and
        self.flow:get_state() == 1 and
        self.outTE:get_value() <= self.rt_par_float[
        post.S_F_PAR.OUT_NORMAL ] then
        self.V1:on()
    else
        self.V1:off()
    end

    --Клапан подогрева.
    if self:get_mode( self.MODES.W_ACCEPTING ) == 1 then
        if self.outTE:get_value() > self.rt_par_float[
            post.S_F_PAR.T_OUT_MAX ] then
            if self.timers[ post.TMR.DELAY_TEMP ]:get_state() ==
                1 then

                --Таймер запущен, проверяем, вышло ли время.
                if self.timers[
                    post.TMR.DELAY_TEMP ]:is_time_up() == 1 then

                    self.rt_par_float[
                        post.RT_F_PAR.WARNING_REASON ] =
                        post.WARN.MAX_OUT_TEMPER

                    self:set_mode(
                        POST.MODES.W_ACCEPTING_PAUSE, 1 )
                    self.timers[ TMR_DELAY_TEMP ]:reset()
                end
            else
                --Запускаем таймер.
                self.timers[
                    post.TMR.DELAY_TEMP ]:set_countdown_time( 2000 )
                self.timers[ post.TMR.DELAY_TEMP ]:reset()
                self.timers[ post.TMR.DELAY_TEMP ]:start()
            end
        else
            self.timers[ post.TMR.DELAY_TEMP ]:reset()
        end
    end
    --Клапан подогрева.-!>
end
-- ----------------------------------------------------------------------------
local function on_whey_accepting( self )
    self.N1:on()
    self.lamp_ready:on()
    self.lamp_working:on()

    --Общее заполнение машины.
    if self.ctr:get_quantity() >=
        self.rt_par_float[ post.RT_F_PAR.TOT_VOL ] then
        self:exec_cmd( post.CMD.RESET_POST )
    else
        --Заполнение текущей секции.
        if self.ctr:get_quantity() - self.prevSectVol >=
            self.rt_par_float[ self.rt_par_float[
            self.RT_F_PAR.CURRENT_SEC ] +
            self.RT_F_PAR.SECTION1 - 1 ] then

            self.prevSectVol = self.prevSectVol + self.rt_par_float[
                self.rt_par_float[ post.RT_F_PAR.CURRENT_SEC ] +
                post.RT_F_PAR.SECTION1 - 1 ]
            self.rt_par_float[ post.RT_F_PAR.CURRENT_SEC ] =
                self.rt_par_float[ post.RT_F_PAR.CURRENT_SEC ] + 1

            --Есть секции.
            if self.rt_par_float[ post.RT_F_PAR.CURRENT_SEC ] <
                self.rt_par_float[ post.RT_F_PAR.CURRENT_SEC ] + 1 then

                self:set_mode( self.MODES.W_ACCEPTING_PAUSE, 1 )

            else
                self:exec_cmd( self.CMD.RESET_POST )
            end
        end

    end
end
-- ----------------------------------------------------------------------------
function post:init()
    self.prevSectVol = 0

    --Клапана.
    self.V1     = V( self.number - 5 )
    self.outTE  = 6 == self.number and TE( 5 ) or TE( 6 )
    self.N1     = N( self.number - 5 )
    self.ctr    = CTR( self.number - 5 )
    self.flow   = 6 == self.number and FB( 5 ) or FB( 6 )

    self.lamp_ready  = 6 == self.number and UPR( 1 ) or UPR( 3 )
    self.lamp_working= 6 == self.number and UPR( 2 ) or UPR( 4 )
    self.btn_start   = 6 == self.number and FB( 1 )  or FB( 3 )
    self.btn_start_prev_state = 0
    self.btn_pause   = 6 == self.number and FB( 2 )  or FB( 4 )
    self.btn_pause_prev_state = 0
end
-- ----------------------------------------------------------------------------
function post:init_params()
    self.par_float:save( self.S_F_PAR.LE_MIN, 		 1  )
    self.par_float:save( self.S_F_PAR.T_OUT_MAX, 	 50 )
    self.par_float:save( self.S_F_PAR.OUT_NORMAL, 	 40 )
    self.par_float:save( self.S_F_PAR.PAUSE_TIME_MAX, 5 )
end
-- ----------------------------------------------------------------------------
function post:evaluate()

    for i = 0, self:get_modes_count() do
        if self:get_mode( i ) == 1 then

            if i == post.MODES.W_HEATING then
                on_heating( self )

            elseif i == post.MODES.W_ACCEPTING then
                on_whey_accepting( self )

            elseif i == post.MODES.W_ACCEPTING_PAUSE then
                if self.timers[
                    post.TMR.PAUSE_TIME ]:is_time_up() == 1 then
                    self.timers[ post.TMR.PAUSE_TIME ]:reset()
                    self.timers[ post.TMR.PAUSE_TIME ]:start()
                    self.rt_par_float[ post.RT_F_PAR.WARNING_REASON ] =
                        post.WARN.PAUSE_TIME_LEFT
                end

                self.lamp_ready:on()

            elseif i == post.MODES.W_ACCEPTING_END then
            end
        end
    end


    --Start and stop buttons.
    if self.btn_start:get_state() == 1 and
        self.btn_start_prev_state == 0  then

        if DEBUG == true then
            print( "Pressed start button post 1!" )
        end
        if self.lamp_ready:get_state() == 1 then
            self:set_mode( post.MODES.W_ACCEPTING, 1 )
        end

        self.btn_start_prev_state = 1
    end

    if self.btn_start:get_state() == 0 and
        self.btn_start_prev_state == 1  then
        self.btn_start_prev_state = 0
    end

    if self.btn_pause:get_state() == 0 and
        self.btn_pause_prev_state == 1 then

        if DEBUG == true then
            print( "Pressed pause button post 1!" )
        end

        if self.lamp_working:get_state() == 1 then
            self:set_mode( post.MODES.W_ACCEPTING_PAUSE, 1 )
        end

        self.btn_pause_prev_state = 0
    end

    if self.btn_pause:get_state() == 1 and
        self.btn_pause_prev_state == 0 then

        self.btn_pause_prev_state = 1
    end
    --Start and stop buttons.-!>

    return 0
end
-- ----------------------------------------------------------------------------
function post:init_mode( mode )
    if mode == post.MODES.W_ACCEPTING then
        self.flow:set_state( 1 )
        self.flow:set_change_time( 5000 )


        self.rt_par_float[ post.RT_F_PAR.WARNING_REASON ] = 0
        self:set_mode( post.MODES.W_ACCEPTING_PAUSE, 0 )
        self:set_mode( post.MODES.W_ACCEPTING_END, 0 )

    elseif mode == post.MODES.W_ACCEPTING_PAUSE then
        if self.rt_par_float[ post.RT_F_PAR.IS_RESET_POST ] == 1 then

            self.ctr:reset()
            self.ctr:start()

            self.prevSectVol = 0
            self.rt_par_float[ post.RT_F_PAR.CURRENT_SEC ] = 1
            self.rt_par_float[ post.RT_F_PAR.IS_RESET_POST ] = 0
        end

        self.timers[ post.TMR.PAUSE_TIME ]:set_countdown_time(
            60 * 1000 * self.par_float[ post.S_F_PAR.PAUSE_TIME_MAX ] )
        self.timers[ post.TMR.PAUSE_TIME ]:reset()
        self.timers[ post.TMR.PAUSE_TIME ]:start()

        self:set_mode( post.MODES.W_ACCEPTING, 0 )
        self:set_mode( post.MODES.W_ACCEPTING_END, 0 )
    end

end
-- ----------------------------------------------------------------------------
function post:final_mode( mode )

    --Подогрев сыворотки постом.
    if mode == post.MODES.W_HEATING then
        self.V1:off()

    --Приёмка сыворотки постом.
    elseif mode == post.MODES.W_ACCEPTING then
        self.N1:off()
        self.lamp_working:off()
        self.lamp_ready:off()

    --Пауза приёмки сыворотки постом.
    elseif mode == post.MODES.W_ACCEPTING_PAUSE then
        self.lamp_ready:off()

    --Завершение приёмки сыворотки постом.
    elseif mode == post.MODES.W_ACCEPTING_END then
    end

    return 0
end
-- ----------------------------------------------------------------------------
function post:check_on_mode( mode )

    if mode == post.MODES.W_HEATING then
            if not( self:get_mode( post.MODES.W_ACCEPTING ) or
                self:get_mode( post.MODES.W_ACCEPTING_PAUSE ) ) then
                return 1
            end
    end

    return 0
end
-- ----------------------------------------------------------------------------
function post:exec_cmd( cmd )
    if cmd == CMD.RESET_POST then
        self:set_mode( post.MODES.W_ACCEPTING_PAUSE, 0 )
        self:set_mode( post.MODES.W_ACCEPTING, 0 )
        self:set_mode( post.MODES.W_HEATING, 0 )
        self:set_mode( post.MODES.W_ACCEPTING_END, 1 )

        local post_mode1
        local post_mode2
        --Пост №1.
        if 6 == self.number then
            post_mode1 = whey_tank.MODES.W_OUT_P1
            post_mode2 = whey_tank.MODES.W_WOUT_P1
        --Пост №2.
        else
            post_mode1 = whey_tank.MODES.W_OUT_P2
            post_mode2 = whey_tank.MODES.W_WOUT_P2
        end

        TANK1:set_mode( post_mode1, 0 )
        TANK1:set_mode( post_mode2, 0 )
        TANK2:set_mode( post_mode1, 0 )
        TANK2:set_mode( post_mode2, 0 )
        TANK3:set_mode( post_mode1, 0 )
        TANK3:set_mode( post_mode2, 0 )
        TANK4:set_mode( post_mode1, 0 )
        TANK4:set_mode( post_mode2, 0 )

    elseif cmd == CMD.SET_PAUSE_AND_HEATING then
            self:set_mode( post.MODES.W_ACCEPTING_PAUSE, 1 )
            self:set_mode( post.MODES.W_HEATING, 1 )
    end

    return 1000 + cmd
end
-- ----------------------------------------------------------------------------
-- Описание - гребенка.
-- ----------------------------------------------------------------------------
comb = project_tech_object:new{ name = "COMB",
    runtime_params_float_count = 5,
    params_float_count = 20 }

--Режимы.
comb.MODES =
    {
    WASH = 0, --Мойка линии приёмки сыворотки.
    }

-- ----------------------------------------------------------------------------
-- Работа гребенки.
-- ----------------------------------------------------------------------------
function comb:evaluate()
    for i = 0, self:get_modes_count() do

        if self:get_mode( i ) == 1 then
            if i == comb.MODES.WASH then
                V( 106 ):on()
                V( 108 ):on()
                V( 109 ):on()

                V( 102 ):off()
                V( 202 ):off()
                V( 208 ):off()
                V( 302 ):off()
                V( 402 ):off()
            end
        end
    end
end
-- ----------------------------------------------------------------------------
function comb:final_mode( mode )
    if mode == comb.MODES.WASH then
        V( 106 ):off()
        V( 108 ):off()
        V( 109 ):off()
    end

    return 0
end
-- ----------------------------------------------------------------------------
function comb:check_on_mode( mode )
   if mode == comb.MODES.WASH then
        if  --Во время приёмки нельзя мыть.
            TANK1:get_mode( whey_tank.MODES.W_ACCEPTING ) == 1 or
            TANK2:get_mode( whey_tank.MODES.W_ACCEPTING ) == 1  or
            TANK3:get_mode( whey_tank.MODES.W_ACCEPTING ) == 1  or
            TANK4:get_mode( whey_tank.MODES.W_ACCEPTING ) == 1  or

            -- Мойка какого-либо танка сыворотки.
            TANK1:get_mode( whey_tank.MODES.WASH ) == 1  or
            TANK2:get_mode( whey_tank.MODES.WASH ) == 1  or
            TANK3:get_mode( whey_tank.MODES.WASH ) == 1  or
            TANK4:get_mode( whey_tank.MODES.WASH ) == 1  then

            return 1
        end
    end

    return 0
end
-- ----------------------------------------------------------------------------
-- Создание необходимых объектов.
-- ----------------------------------------------------------------------------
TANK1 = whey_tank:new{ number = 1 }
TANK2 = whey_tank:new{ number = 2 }
TANK3 = whey_tank:new{ number = 3 }
TANK4 = whey_tank:new{ number = 4 }
TANK1:init()
TANK2:init()
TANK3:init()
TANK4:init()

POST1 = post:new{ number = 6 }
POST2 = post:new{ number = 7 }
POST1:init()
POST2:init()

COMB1 = comb:new()
-- ----------------------------------------------------------------------------
-- Регистрация необходимых объектов.
-- ----------------------------------------------------------------------------
object_manager:add_object( TANK1 )
object_manager:add_object( TANK2 )
object_manager:add_object( TANK3 )
object_manager:add_object( TANK4 )
object_manager:add_object( POST1 ) -- Пост 1.
object_manager:add_object( POST2 ) -- Пост 2.
object_manager:add_object( COMB1 ) -- Гребенка.

-- Создаем копии объектов со стандартными именем - TANK[номер танка] - для
-- корректной работы скрипта.
TANK6 = POST1
TANK7 = POST2
-- ----------------------------------------------------------------------------
