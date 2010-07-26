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
            C_WASH,     //Мойка линии приёмки сыворотки
            };
        //-End of comb modes.-!>

        //-Comb timers.
        enum COMB_TIMERS
            {
            //Timers N 0-3 - system timers.
            };
        //-End of comb timers.-!>

        //-Comb parameters.
        enum COMB_PARAMS
            {
            };
        //-End of comb parameters.-!>

        int init_mode( u_int mode );
        int evaluate();
        int final_mode( u_int mode );
        my_comb( int stCnt, int parCnt, int workParCnt, int tmrCnt );
        ~my_comb();
    };
//-----------------------------------------------------------------------------
enum CONSTANTS
    {
    C_COMB_INDEX   = 0,

    C_TANK_1_INDEX = 1,
    C_TANK_2_INDEX = 2,
    C_TANK_3_INDEX = 3,
    C_TANK_4_INDEX = 4,
    
    C_POST_1_INDEX = 5,
    C_POST_2_INDEX = 6,

    POST1_FLOW = 5,
    POST2_FLOW = 6,
    };
//-----------------------------------------------------------------------------
//-Tank modes.
enum TANK_MODES
    {
    T_WHEY_ACCEPTING,            //Приёмка сыворотки
    T_WHEY_OUT_P1,               //Выдача сыворотки пост 1
    T_WHEY_OUT_P2,               //Выдача сыворотки пост 2
    T_WASH,                      //Мойка

    T_WHEY_WACCEPTING = 8,       //Ожидание приёмки сыворотки
    T_WHEY_WOUT_P1,              //Ожидание выдачи сыворотки пост 1
    T_WHEY_WOUT_P2,              //Ожидание выдачи сыворотки пост 2

    //--Режимы поста.
    POST_WHEY_HEATING = 16,      //Подогрев сыворотки постом
    POST_WHEY_ACCEPTING,         //Приёмка сыворотки постом
    POST_WHEY_ACCEPTING_PAUSE,   //Пауза приёмки сыворотки постом
    POST_WHEY_ACCEPTING_END,     //Завершение приёмки сыворотки постом
    //--Режимы поста.!->

    //--Команды.
    RESET_POST = 30,             //Выключает пауза/работу, включает Выдача завершена
    SET_PAUSE_AND_HEATING,       //Включает пауза и подогрев
    RESET_TANK_POST1,            //Выключает режим выдачи для танка и, если надо, поста 1
    RESET_TANK_POST2,            //Выключает режим выдачи для танка и, если надо, поста 2

    SET_POST1_AND_TANK,          //Включить танк и пост 1
    SET_POST2_AND_TANK,          //Включить танк и пост 2
    SET_HEATING_POST1_AND_TANK,  //Включить танк и подогрев пост 1
    SET_HEATING_POST2_AND_TANK,  //Включить танк и подогрев пост 2
    };
//-End of tank modes.!->
//-----------------------------------------------------------------------------
class whey_tank : public tech_object
    {
    public:
        //-Tank parameters.
        enum TANK_PARAMETERS
            {
            T_WARNING_REASON = 4,
            };
        //-End of tank parameters.-!>

        //-Tanks errors.
        enum TANK_WARNINGS
            {
            tankInStop = 1,
            tankInStart = 2,
            };
        //-Tanks errors.-!>

        smart_ptr< i_DO_device > V1;  //Запорный клапан на подачу в танк
        smart_ptr< i_DO_device > V2;  //Клапан на подачу в танк из линии подачи сыворотки
        smart_ptr< i_DO_device > V3;  //Клапан на выдачу
        smart_ptr< i_DO_device > V4;  //Запорный клапан на выдачу
        smart_ptr< i_DO_device > V5;  //Запорный клапан входа мойки танка

        smart_ptr< i_DO_device > V7_1; //Клапан запорный линии мойки 1
        smart_ptr< i_DO_device > V7_2; //Клапан запорный линии мойки 2

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

        int set_mode( u_int mode, int newm );
        int evaluate();
        int final_mode( u_int mode );
        int init_mode( u_int mode );

        whey_tank( int par_count, int n, int tmr_cnt );
        ~whey_tank();
    };
//-----------------------------------------------------------------------------
const int delayTempTmr = 1;
const int pauseTimeTmr = 2;

class post : public tech_object
    {
    public:
        //-Post parameters.
        enum PParameters
            {
            SECTION1 = 1,     //Ёмкость секции 1.
            SECTION2,
            SECTION3,
            SECTION4,
            SECTION5,
            SECTION6,
            SECTION7,
            SECTION8,
            SECTION9,
            SECTION10,
            AUTO_NUM,       //Номер машины.
            CURRENT_SEC,    //Текущая секция.
            SECTIONS,       //Общее число секций.
            TOT_VOL,        //Общий объём.
            IS_RESET_POST,  //15 При включении паузы сбросить счётчик.

            P_WARNING_REASON = 17,

            T_LE_MIN = 19,  //19 Минимальный уровень в танке, ниже которого переходим к следующему танку очереди
            T_OUT_MAX,      //20 Максимальная температура выдачи, при превышении которой возникает аварийная ситуация
            T_OUT_NORMAL,   //21 Температура выдачи, при превышении которой закрывается паровой клапан подогрева
            PAUSE_TIME_MAX, //22 Макс время режима ПАУЗА.
            };
        //-End of post parameters.-!>

        //-Post errors.
        enum postWarnings
            {
            maxTPost = 1,
            noFlowPost = 2,
            pauseTimeLeft,
            };
        //-Post errors.-!>

        smart_ptr< i_DO_device > V1;    //Клапан пара.
        smart_ptr< i_AI_device > outTE; //Температура выхода.

        smart_ptr< i_DO_device > N1; //Насос выдачи.
        smart_ptr< i_counter >   ctr;//Счётчик

        smart_ptr< i_DO_device > lampReady;
        smart_ptr< i_DO_device > lampWorking;
        smart_ptr< i_DI_device > flow;

        smart_ptr< i_DI_device > btnStart;
        smart_ptr< i_DI_device > btnPause; //Start, stop buttons.
        int       btnStartPrevState, btnPausePrevState;

        int       prevSectVol;
        char      shutDown;

        int evaluate();
        int final_mode( u_int mode );
        int init_mode( u_int mode );

        int init_params();

        post( int par_count, int n, int tmr_cnt );
        ~post();
    };
//-----------------------------------------------------------------------------
#endif	// PRIEM_H
