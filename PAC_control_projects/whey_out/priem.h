#ifndef PRIEM_H
#define	PRIEM_H

#include "tech_def.h"
#include "PAC_dev.h"
//-----------------------------------------------------------------------------
class my_comb : public tech_object
    {
    public:
        //-Comb modes.
        enum COMB_MODES
            {
            C_WASH,     // Мойка линии приёмки сыворотки.
            };
        //-End of comb modes.-!>

        my_comb( int stCnt, int parCnt, int workParCnt, int tmrCnt );
        virtual ~my_comb();

        int evaluate();
        int final_mode( u_int mode );

        int check_on_mode( int mode );
    };
//-----------------------------------------------------------------------------
// Индексы объектов.
enum CONSTANTS
    {
    C_COMB_IDX   = 0,

    C_T1_IDX = 1,
    C_T2_IDX = 2,
    C_T3_IDX = 3,
    C_T4_IDX = 4,
    
    C_POST1_IDX = 5,
    C_POST2_IDX = 6,
    };
//-----------------------------------------------------------------------------
//-Tank modes.
enum TANK_MODES
    {
    T_WHEY_ACCEPTING,            // Приёмка сыворотки.
    T_WHEY_OUT_P1,               // Выдача сыворотки пост 1.
    T_WHEY_OUT_P2,               // Выдача сыворотки пост 2.
    T_WASH,                      // Мойка.

    T_WHEY_WACCEPTING = 8,       // Ожидание приёмки сыворотки.
    T_WHEY_WOUT_P1,              // Ожидание выдачи сыворотки пост 1.
    T_WHEY_WOUT_P2,              // Ожидание выдачи сыворотки пост 2.

    //--Режимы поста.
    POST_WHEY_HEATING = 16,      // Подогрев сыворотки постом.
    POST_WHEY_ACCEPTING,         // Приёмка сыворотки постом.
    POST_WHEY_ACCEPTING_PAUSE,   // Пауза приёмки сыворотки постом.
    POST_WHEY_ACCEPTING_END,     // Завершение приёмки сыворотки постом.
    //--Режимы поста.!->

    //--Команды.
    CMD_RESET_POST = 30,             // Выключает пауза/работу, включает Выдача завершена.
    CMD_SET_PAUSE_AND_HEATING,       // Включает пауза и подогрев.
    CMD_RESET_TANK_POST1,            // Выключает режим выдачи для танка и, если надо, поста 1.
    CMD_RESET_TANK_POST2,            // Выключает режим выдачи для танка и, если надо, поста 2.

    CMD_SET_POST1_AND_TANK,          // Включить танк и пост 1.
    CMD_SET_POST2_AND_TANK,          // Включить танк и пост 2.
    CMD_SET_HEATING_POST1_AND_TANK,  // Включить танк и подогрев пост 1.
    CMD_SET_HEATING_POST2_AND_TANK,  // Включить танк и подогрев пост 2.
    };
//-End of tank modes.!->
//-----------------------------------------------------------------------------
class post;
//-----------------------------------------------------------------------------
class whey_tank : public tech_object
    {
    friend class post;

    //-Tank parameters.
    enum TANK_PARAMETERS
        {
        T_WARNING_REASON = 4,
        };
    //-End of tank parameters.-!>

    //-Tanks errors.
    enum TANK_WARNINGS
        {
        TW_TANK_IN_STOP = 1,
        TW_TANK_IN_START
        };
    //-Tanks errors.-!>

    smart_ptr< i_DO_device > V1;  // Запорный клапан на подачу в танк.
    smart_ptr< i_DO_device > V2;  // Подача в танк из линии подачи сыворотки.
    smart_ptr< i_DO_device > V3;  // Клапан на выдачу.
    smart_ptr< i_DO_device > V4;  // Запорный клапан на выдачу.
    smart_ptr< i_DO_device > V5;  // Запорный клапан входа мойки танка.

    smart_ptr< i_DO_device > V7_1; // Клапан запорный линии мойки 1.
    smart_ptr< i_DO_device > V7_2; // Клапан запорный линии мойки 2.

    smart_ptr< i_DI_device > LSL;
    smart_ptr< i_AI_device > LE1;
    smart_ptr< i_DI_device > LSH;
    smart_ptr< i_AI_device > T;

    //-Для выдачи с переходом.
    smart_ptr< i_DO_device > V_1;
    smart_ptr< i_DO_device > V_2;
    smart_ptr< i_DO_device > V_3;
    smart_ptr< i_DO_device > V_4;
    //-Для выдачи с переходом.-!>

    public:
        int  check_on_mode( u_int mode );
        void init_mode( u_int mode );
        int  evaluate();
        int  final_mode( u_int mode );

        int exec_cmd( u_int cmd );

        whey_tank( int n );
        virtual ~whey_tank();
    };
//-----------------------------------------------------------------------------
class post : public tech_object
    {
    friend class whey_tank;

    //-Post parameters.
    enum P_PARAMS
        {
        RT_F__SECTION1 = 1,   // Ёмкость секции 1.
        RT_F__SECTION2,
        RT_F__SECTION3,
        RT_F__SECTION4,
        RT_F__SECTION5,
        RT_F__SECTION6,
        RT_F__SECTION7,
        RT_F__SECTION8,
        RT_F__SECTION9,
        RT_F__SECTION10,
        RT_F__AUTO_NUM,       // Номер машины.
        RT_F__CURRENT_SEC,    // Текущая секция.
        RT_F__SECTIONS,       // Общее число секций.
        RT_F__TOT_VOL,        // Общий объём.
        RT_F__IS_RESET_POST,  // При включении паузы сбросить счётчик.

        RT_F__WARNING_REASON = 17,

        // Минимальный уровень в танке, ниже которого переходим к
        // следующему танку очереди.
        S_F__LE_MIN = 1,

        // Максимальная температура выдачи, при превышении которой
        // возникает аварийная ситуация.
        S_F__T_OUT_MAX = 2,

        // Температура выдачи, при превышении которой закрывается паровой
        // клапан подогрева.
        S_F__OUT_NORMAL = 3,

        // Максимальное время режима ПАУЗА, мин.
        S_F__PAUSE_TIME_MAX = 4,
        };
    //-End of post parameters.-!>

    //-Post errors.
    enum P_WARNINGS
        {
        W_MAX_OUT_TEMPER = 1,
        W_NO_FLOW,
        W_PAUSE_TIME_LEFT,
        };
    //-Post errors.-!>

    enum P_TIMERS
        {
        TMR_DELAY_TEMP = 1,
        TMR_PAUSE_TIME = 2,
        };

    int  btnStartPrevState;
    int  btnPausePrevState;

    int  prevSectVol;
    char shutDown;

    smart_ptr< i_DO_device > V1;        // Клапан пара.
    smart_ptr< i_AI_device > outTE;     // Температура выхода.
    smart_ptr< i_DO_device > N1;        // Насос выдачи.
    smart_ptr< i_counter >   ctr;       // Счётчик
    smart_ptr< i_DI_device > flow;

    smart_ptr< i_DO_device > lampReady;
    smart_ptr< i_DO_device > lampWorking;
    smart_ptr< i_DI_device > btnStart;  // Start button.
    smart_ptr< i_DI_device > btnPause;  // Stop button.

    public:
        int  check_on_mode( u_int mode );
        void init_mode( u_int mode );        
        int  evaluate();
        int  final_mode( u_int mode );

        int exec_cmd( u_int cmd );

        int init_params();

        post( int n );
        virtual ~post();
    };
//-----------------------------------------------------------------------------
#endif	// PRIEM_H
