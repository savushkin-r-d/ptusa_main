/// @file PAC_err.h
/// @brief Содержит описания классов, которые используются для организации 
/// работы с критическими ошибками PAC.
/// 
/// Класс @ref PAC_critical_errors_manager, служащий для организации работы с 
/// ошибками простых устройств, содержит всю необходимую информацию. Для сбора 
/// всех ошибок служит структура @ref PAC_critical_errors_manager::critical_error. 
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c PAC_ERRORS_H - включение данного файла в компиляцию только один раз.@n
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_ERRORS_H
#define PAC_ERRORS_H

#include <string.h>
#include <vector>

//#include "sys.h"
#include "smart_ptr.h"
#include "dtime.h"
#include "led.h"

//-----------------------------------------------------------------------------
enum ERRORS_CLASS
    {
    EC_NO_CONNECTION = 0,           //Ошибка связи.
    EC_COM_DRIVER,                  //Ошибка работы с COM-портом.

    EC_DEVICE_DEFINITION = 10,      //Ошибки описания устройств.
    EC_COMB_MODE_DEFINITION = 12,   //Ошибки описания режимов гребенки.

    EC_RUNTIME_ERROR = 13,          //Ошибки во время работы.
    };

enum ERRORS_SUBCLASS
    {
    //EC_NO_CONNECTION = 0,         //Ошибка связи.
    ES_WAGO = 1,                    //Ошибки модулей WAGO.
    ES_PANEL,                       //Ошибки панелей EasyView.
    ES_MODBUS_DEVICE,               //Ошибки устройства, опрашиваемого по Modbus.
    ES_CONTROL_PAC,                 //Ошибка связи с контролирующим PAC.
    ES_EASYSERVER = 5,              //Ошибки EasyServer.

    //EC_DEVICE_DEFINITION = 10,    //Ошибки описания устройств.
    ES_WRONG_LS = 1,                //   1  - LS,
    ES_WRONG_FS = 2,                //   2  - FS,
    ES_WRONG_FB = 3,                //   3  - FB,
    ES_WRONG_AO = 4,                //   4  - AO,
    ES_WRONG_CTR = 5,               //   5  - CTR,
    ES_WRONG_TE = 6,                //   6  - TE,
    ES_WRONG_FE = 7,                //   7  - FE,
    ES_WRONG_LE = 8,                //   8  - LE,
    ES_WRONG_QE = 9,                //   9  - QE,
    ES_WRONG_MF = 10,               //   10 - MF,
    ES_WRONG_AI = 11,               //	 11 - AI
    ES_WRONG_MLP_DEFINITION_FILE,   //	 12 - MLP

    EC_COMB_TIMERS_COUNT = 20,      //Ошибка создания таймеров гребенки (количество таймеров должно быть >= 4)
    EC_COMB_PARAMS_COUNT = 21,      //Ошибка создания параметров гребенки (количество параметров должно быть >= 17) 

    //EC_COMB_MODE_DEFINITION = 12, //Ошибки описания режимов гребенки:
    ES_WRONG_PAIR_FB_CONTROL = 1,   //Не правильная пара обратная связь-управляющий сигнал.

    //EC_RUNTIME_ERROR = 13,        //Ошибки во время работы.
    ES_EMERGENCY_BUTTON = 1,        //Нажата аварийная кнопка.
    };

//0, 2, n - нет связи с панелью номер n.
//0, 3, n - нет связи с Modbus устройством номер n.
//0, 4, 4 - нет связи с контролирующим PAC.
//0, 5, 5 - нет связи с сервером.
//1, 1 - ошибка работы с COM-портом WAGO. 
//      1 - CRC error
//	    2 - rcmctr Port error
//	    3 - TWagoDrv::Run(void)          - WAGO ERROR:Unable to open port
//	    4 - TWagoDrv::ReadInputs(void)   - Wago driver error: Unable to write port
//	    5 - TWagoDrv::ReadInputs(void)   - Wago driver error: Unable to write port
//	    6 - TWagoDrv::WriteOutputs(void) - Wago driver error: Unable to write port
//
//10 - ошибки описания устройств - не найдено данное устройство:
//   1  - LS,
//   2  - FS,
//   3  - FB,
//   4  - AO,
//   5  - CTR,
//   6  - TE,
//   7  - FE,
//   8  - LE,
//   9  - QE,
//   10 - MF,
//	 11 - AI
//       n - номер устройства.
//   20, 0 - ошибка создания таймеров гребенки (количество таймеров должно быть >= 4)
//   21, 0 - ошибка создания параметров гребенки (количество параметров должно быть >= 17) 
//
//12 - ошибки описания режимов гребенки:
//  1 - не правильная пара обратная связь-управляющий сигнал.
//       n - номер режим,
//
//11 - ошибки нейроконтроллера:
//  0,  0 - не найден файл описания нейроконтроллера.
//
//13 - ошибки времени работы:
//  1, n  - нажата аварийная кнопка с номером обратной связи n. 
//-----------------------------------------------------------------------------
class PAC_critical_errors_manager
    {
    public:
        enum GE_CONST
            {
            GE_ERROR_SIZE = 3,      ///< Размер одной ошибки, байт.
            };

        PAC_critical_errors_manager();        

        void show_errors();
        void set_global_error( ERRORS_CLASS eclass, ERRORS_SUBCLASS p1, 
            unsigned long param );
        void reset_global_error( ERRORS_CLASS eclass, ERRORS_SUBCLASS p1, 
            unsigned long param );

        int save_to_stream( char *buff );
        unsigned char save_to_stream_2( char *buff );

        static PAC_critical_errors_manager * get_instance();

    private:
        static auto_smart_ptr < PAC_critical_errors_manager > instance;

        struct critical_error
            {
            int             err_class;     ///< Класс ошибки.
            unsigned int    err_sub_class; ///< Подкласс ошибки.
            unsigned int    param;         ///< Параметр ошибки.

            critical_error( int err_class = 0, u_int err_sub_class = 0,
                u_int param = 0 );
            };

        std::vector< critical_error >  errors;

        u_int_2 errors_id;
        int   global_ok;

//        auto_smart_ptr< led > wago_led;
    };
//-----------------------------------------------------------------------------
#endif // PAC_ERRORS_H
