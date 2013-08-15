#ifndef _PID_H
#define _PID_H

class PID 
    { 
    enum STATES 
        {
        STATE_OFF,
        STATE_ON,  
        };

    float uk_1;
    float ek_1;
    float ek_2;
    float q0;
    float q1;
    float q2;
    float Uk;
    float dUk;
    unsigned long startTime;
    unsigned long lastTime;

    char prevManualMode;

    char isDownToInAccelMode;  //Надо ли при старте регулятора уменьшать, а не увеличивать выходную величину.

    public:
        float par[ 20 ];
        int state;

        int startParamIndex;  
        enum PARAM 
            {
            PAR_Z,            //0 Требуемое значение.
            PAR_k,            //1 Параметр k.
            PAR_Ti,           //2 Параметр Ti.
            PAR_Td,           //3 Параметр Td.
            PAR_dt,           //4 Интервал расчёта
            PAR_dmax,         //5 Мax значение входной величины.
            PAR_dmin,         //6 Мin значение входной величины.
            PAR_AccelTime,    //7 Время выхода на режим регулирования.
            PAR_IsManualMode, //8 Ручной режим.
            PAR_UManual,      //9 Заданное ручное значение выходного сигнала.
            PAR_Uk,           //10 Выход ПИД.
            }; 

        int startWorkParamsIndex;
        enum WORK_PARAMS 
            {
            WPAR_Uk,          //1 Выход ПИД-регулятора.
            WPAR_Z,           //2 Требуемое значение ПИД.
            };

        PID( int startParamIndex = 0, int startWorkParamsIndex = 10 );
        ~PID(); 

        void  On( char isDownToInAccelMode = 0 );
        void  Off();
        float Eval( float currentValue, int deltaSign = 1 );
        void  Reset();
        void  Reset( float new_uk_1 );
        void  SetZ ( float newZ );        //Установить новое задание ПИД.
    };
#endif