/// @file PAC_dev.h
/// @brief Классы, которые реализуют функции передачи 
/// состояния устройств PAC на сервер. Также реализованы классы для работы с 
/// устройствами ( клапана, насосы,... ). Классы используются ТОЛЬКО в 
/// контроллере ( PAC ).
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG - компиляция c выводом отладочной информации в консоль.
/// @c DEBUG_NO_WAGO_MODULES - простые устройства работают без модулей
/// wago (состояния хранят в себе).
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include <string.h>
#include <stdio.h>

#include <vector>

#include "wago.h"
#include "g_device.h"

#include "param_ex.h"

#include "smart_ptr.h"

#define OFF     0
#define ON      1

//-----------------------------------------------------------------------------
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        /// @brief Приостановка работы счетчика.
        virtual void pause() = 0;

        /// @brief Возобновление работы счетчика.
        virtual void start() = 0;

        /// @brief Сброс счетчика и остановка счета.
        /// 
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset() = 0;

        /// @brief Сброс счетчика и продолжение счета.
        void restart();

        /// @brief Получение значения счетчика.
        virtual u_int get_quantity() = 0;

        virtual ~i_counter();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного входа.
///
/// Обратная связь, предельный уровень и т.д. являются примерами таких
/// устройств.
class i_DI_device
    {
    public:
        i_DI_device();

        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде целого числа.
        virtual int get_state_now() = 0;

        /// @brief Получение отфильтрованного состояния устройства.
        ///
        /// Устройство меняет свое состояние, если дискретный вход находится в
        /// в данном состоянии больше заданного интервала времени (Задание
        /// интервала - смотри @ref set_change_time).
        ///
        /// @return - состояние устройства в виде целого числа.
        virtual int get_state();

        /// @brief Установка времени ожидания изменения состояния.
        ///
        /// @param time - время ожидания изменения состояния.
        void set_change_time( u_int time );

        /// @brief Установка состояния с учетом ручного режима.
        ///
        /// Данный метод используется для задания состояния устройства перед
        /// его проверкой.
        ///
        /// @param new_state - новое состояние.
        void set_state( int new_state );

        /// @brief Проверка активного состояния.
        ///
        /// Для уровня - сработал он или нет.
        ///
        /// @return - 1 - устройство активно.
        /// @return - 0 - устройство неактивно.
        virtual bool is_active();

    protected:
        u_int last_check_time;///< Время последней проверки состояния.
        int   state;          ///< Предыдущее состояние, для исключения дребезга.
        u_int dt;             ///< Интервал установления состояния, мсек.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного выхода.
///
/// Клапан, мешалка и т.д. являются примерами таких устройств.
class i_DO_device: public i_DI_device
    {
    public:
        /// @brief Включение устройства с учетом ручного режима.
        virtual void on()
            {
            if ( !get_manual_mode() )
                {
                direct_on();
                }
            }

        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief Установка нового состояния устройства с учетом ручного режима.
        virtual void set_state( int new_state )
            {
            if ( !get_manual_mode() )
                {
                direct_set_state( new_state );
                }
            }

    protected:
        /// @brief Установка нового состояния устройства.
        ///
        /// @param new_state - новое состояние объекта.
        virtual void direct_set_state( int new_state ) = 0;

        /// @brief Включение устройства.
        ///
        /// Установка устройства в активное состояние. Для клапана это означает
        /// его активирование, то есть если он нормально закрытый - открытие.
        virtual void direct_on() = 0;

        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на на основе аналогового входа.
///
/// Температура, расход и т.д. являются примерами таких устройств.
class i_AI_device
    {
    public:
        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде дробного числа.
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе аналогового выхода.
///
/// Аналоговый канал управления и т.д. являются примерами таких устройств.
class i_AO_device: public i_AI_device
    {
    public:
        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief Установка текущего состояния устройства с учетом ручного режима.
        virtual void set_value( float new_value )
            {
            if ( !get_manual_mode() )
                {
                direct_set_value( new_value );
                }
            }

    protected:

        /// @brief Установка текущего состояния устройства.
        ///
        /// @param new_value - новое состояние устройства.
        virtual void direct_set_value( float new_value ) = 0;

        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Класс универсального простого устройства, который используется в 
/// режимах.
class device : public i_AO_device,    
    public i_DO_device, public i_cmd_device
    {
    public:
        /// @brief Выполнение команды.
        ///
        /// Запись в свойство объекта дробного числа.
        ///
        /// @param prop - свойство объекта.
        /// @param idx  - индекс свойства.
        /// @param val  - значение.
        virtual int set_cmd( const char *prop, u_int idx, double val );

        /// @brief Выполнение команды.
        ///
        /// Запись в свойство объекта строки.
        ///
        /// @param prop - свойство объекта.
        /// @param idx  - индекс свойства.
        /// @param val  - значение.
        virtual int set_cmd( const char *prop, u_int idx, char *val );

        /// @brief Сохранение устройства в виде скрипта Lua.
        ///
        /// @param prefix - префикс перед строкой скрипта (обычно символ
        /// табуляции - для визуального форматирования текста).
        /// @param buff [out] - буфер записи строки.
        virtual int save_device( char *buff, const char *prefix );

        //-Ошибки.
        saved_params_u_int_4 err_par;

        enum DEV_ERROR_PARAMS  ///< Параметры ошибки, определяют номера битов.
            {
            DE_IS_ENABLE = 1,  ///< Блокировка тревоги на этапе проектирования.    
            DE_IS_INHIBIT = 2, ///< Блокировка тревоги во время работы.    
            DE_IS_SUPPRESS = 4,///< Подавление тревоги клиентами.
            };
        //-Ошибки.!->

        enum CONSTANTS
            {
            C_DEVICE_TYPE_CNT = 14,     ///< Количество типов устройств.
            };

        static const char DEV_NAMES[][ 5 ];
        static const char DEV_TYPES[];

        /// Типы устройств.
        enum DEVICE_TYPE
            {
            DT_NONE = -1,      ///< Тип не определен.

            DT_V = 0,   ///< Клапан. 
            DT_N,       ///< Насос.
            DT_M,       ///< Мешалка.
            DT_LS,      ///< Уровень (есть/нет).
            DT_TE,      ///< Температура.
            DT_FE,      ///< Расход (значение).
            DT_FS,      ///< Расход (есть/нет).
            DT_CTR,     ///< Счетчик.
            DT_AO,      ///< Аналоговый выход.
            DT_LE,      ///< Уровень (значение).
            DT_FB,      ///< Обратная связь.
            DT_UPR,     ///< Канал управления.
            DT_QE,      ///< Концентрация.
            DT_AI,      ///< Аналоговый вход.
            };

        /// Подтипы устройств.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = -1,      ///< Подтип не определен.

            //DT_V = 0, 
            DST_V_DO_1 = 1,     ///< Клапан с одним каналом управления.
            DST_V_DO_2,         ///< Клапан с двумя каналами управления.
            DST_V_DO_1_DI_1,    ///< Клапан с одним каналом управления и одной обратной связью.
            DST_V_DO_1_DI_2,    ///< Клапан с одним каналом управления и двумя обратными связями.
            DST_V_DO_2_DI_2,    ///< Клапан с двумя каналами управления и двумя обратными связями.
            DST_V_MIXPROOF,     ///< Клапан микспруф.

            DST_V_1DO_3DI,      ///< Клапан с одним каналом управления и тремя обратными связями.
            DST_V_1DO_2DI_S,    ///< Клапан с одним каналом управления и двумя обратными связями на одно из состояний.
            DST_V_AS_MIXPROOF,  ///< Клапан с двумя каналами управления и двумя обратными связями с AS интерфейсом (микспруф).
            };

        device( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type );

        virtual ~device();

        const char *get_name() const;

        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void direct_off() = 0;

        /// @brief Выключение устройства с учетом ручного режима.
        void off();

        /// @brief Вывод объекта в консоль.
        ///
        /// Для использования в отладочных целях.
        virtual void print() const;

        /// @brief Получение номера устройства.
        ///
        /// @return -  номер устройства.
        u_int_4 get_n() const
            {
            return number;
            }

        /// @brief Получение типа устройства.
        int get_type() const
            {
            return type;
            }

        /// @brief Получение подтипа устройства.
        int get_sub_type() const
            {
            return sub_type;
            }

        /// @brief Инициализация параметров устройства.
        virtual int init_params()
            {
            return 0;
            }

    protected:
        u_int_4 number;             ///< Номер устройства.

        DEVICE_TYPE     type;       ///< Тип устройства.
        DEVICE_SUB_TYPE sub_type;   ///< Подтип устройства.

        bool get_manual_mode() const
            {
            return is_manual_mode;
            }

    private:
        bool is_manual_mode;
    }; 
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство.
///
/// Необходимо для возвращения результата поиска устройства с несуществующим
/// номером. Методы данного класса ничего не делают. 
class dev_stub : public device,
    public i_counter
    {
    public:
        dev_stub(): device( 0, DT_NONE, DST_NONE )
            {
            }

        u_int_4 get_n() const;                
        void    print() const;                

        float   get_value();
        void    direct_set_value( float new_value );

        void    direct_on();                
        void    direct_off();                
        void    direct_set_state( int new_state );
        int     get_state_now();

        void    pause();
        void    start();
        void    reset();        
        u_int   get_quantity();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с обратной связью.
///
/// Базовый класс для различных дискретных устройств с обратной связью.
class fb_device
    {
    public:
        enum FB
            {
            FB_NO, ///< Нет обратной связи.
            FB_IS  ///< Обратная связь есть.
            };

        enum STATE
            {
            S_FB_NO,         ///< Нет обратной связи.
            S_FB_IS_AND_ON,  ///< Обратная связь есть и она включена.
            S_FB_IS_AND_OFF  ///< Обратная связь есть, но она отключена.
            };

        enum CONSTANT
            {
            C_PARAMS_COUNT = 1,

            C_FB_OFF = 0,
            C_FB_ON  = 1,
            };

        enum PARAMS
            {
            P_FB_USE         ///< Включена или выключена ОС.
            };

        fb_device( FB fb1, FB fb2 );

        ///@brief Инициализация параметров по умолчанию (ОС включена).
        int init_params();

        ///@brief Получение состояния ОС.
        ///
        /// @param fb_number - индекс ОС.
        ///
        /// @return - состояние ОС.
        STATE get_fb_state( int fb_number );

        ///@brief Сохранение устройства в виде скрипта Lua.
        ///
        /// @param buff - строка, куда сохраняем.
        ///
        /// @return - длина строки Lua.
        int save_device( char *buff );

        ///@brief Выполнение команды.
        int set_cmd( const char *prop, u_int idx, double val );

    protected:
        saved_params_u_int_4 par;
        FB  fb[ 2 ];    
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый класс для различных дискретных устройств.
class digital_wago_device : public device,  
    public wago_device
    {
    public:
        digital_wago_device( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type, 
            bool use_fb = false, bool fb1 = false, bool fb2 = false );

        virtual ~digital_wago_device();

        float   get_value();
        void    direct_set_value( float new_value );
        void    direct_set_state( int new_state );  
        void    print() const;

        int init_params();

        int set_cmd( const char *prop, u_int idx, double val );

        int save_device( char *buff, const char *prefix );

#ifdef DEBUG_NO_WAGO_MODULES
        /// @brief Получение мгновенного состояния объекта.        
        ///        
        /// @return - мгновенное состояние объекта.
        int  get_state_now();

        void direct_on();
        void direct_off();
#endif // DEBUG_NO_WAGO_MODULES

        int get_state();

    protected:
        enum CONSTANTS
            {
            C_SWITCH_TIME = 3, ///< Время переключения устройства, сек.
            };

        auto_smart_ptr< fb_device > fb;

    protected:
#ifdef DEBUG_NO_WAGO_MODULES
        char state;  ///< Состояние устройства.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговыми входами/выходами.
///
/// Базовый класс для различных аналоговых устройств.
class analog_wago_device : public device, public wago_device      
    {
    public:
        analog_wago_device( int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): device( number, type, sub_type )
#ifdef DEBUG_NO_WAGO_MODULES
            ,value( 0 )
#endif  // DEBUG_NO_WAGO_MODULES
            {
            }

        void  direct_set_state( int new_state );
        int   get_state_now();

        void  print() const;
        void  direct_on();        
        void  direct_off();

#ifdef DEBUG_NO_WAGO_MODULES
        float get_value();
        void  direct_set_value( float new_value );

#else  // DEBUG_NO_WAGO_MODULES

        float get_value() = 0;

#endif // DEBUG_NO_WAGO_MODULES


#ifdef DEBUG_NO_WAGO_MODULES
    private:
        float value;    ///< Состояние устройства.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом.
///
/// Это может быть клапан, насос, канал управления...
class DO_1 : public digital_wago_device
    {
    public:
        DO_1( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type, 
            bool use_fb = false, bool fb1 = false, bool fb2 = false 
            ): digital_wago_device( number, type, sub_type, use_fb, fb1, fb2 )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void direct_on();
        void direct_off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами.
///
/// Это может быть клапан, насос...
class DO_2 : public digital_wago_device
    {
    public:
        DO_2( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type 
            ): digital_wago_device( number, type, sub_type, true, false, false )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void direct_on();
        void direct_off();        

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0, ///< Индекс канала дискретного выхода №1.
            DO_INDEX_2,     ///< Индекс канала дискретного выхода №2.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и одним дискретным входом.
///
/// Это может быть клапан, насос...
class DO_1_DI_1 : public digital_wago_device
    {
    public:
        DO_1_DI_1( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type
            ): digital_wago_device( number, type, sub_type, true, true, false )            
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void direct_on();
        void direct_off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
            DI_INDEX = 0,           ///< Индекс канала дискретного входа.
            };

        u_long start_switch_time;  ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_1_DI_2 : public digital_wago_device
    {
    public:
        DO_1_DI_2( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type 
            ): digital_wago_device( number, type, sub_type, true, true, true )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void direct_on();
        void direct_off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_2_DI_2 : public digital_wago_device
    {
    public:
        DO_2_DI_2( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type 
            ): digital_wago_device( number, type, sub_type, true, true, true )            
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void direct_on();
        void direct_off();

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0,         ///< Индекс канала дискретного выхода №1.
            DO_INDEX_2,             ///< Индекс канала дискретного выхода №2.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс mixproof.
class i_mix_proof 
    {
    public:
        enum STATES
            {
            ST_CLOSE = 0,   ///< Закрыт.
            ST_OPEN,        ///< Открыт.
            ST_UPPER_SEAT,  ///< Открыть верхнее седло.
            ST_LOWER_SEAT,  ///< Открыть нижнее седло.
            };

        virtual void open_upper_seat() = 0;
        virtual void open_lower_seat() = 0;        
    };
//-----------------------------------------------------------------------------
/// @brief Клапан mixproof.
class valve_mix_proof : public digital_wago_device, public i_mix_proof
    {
    public:
        valve_mix_proof( u_int number 
            ): digital_wago_device( number, DT_V, DST_V_MIXPROOF, true, true, true )
            {
            }

        void open_upper_seat();
        void open_lower_seat();

#ifdef DEBUG_NO_WAGO_MODULES
        void direct_set_state( int new_state );
#endif // DEBUG_NO_WAGO_MODULES

#ifndef DEBUG_NO_WAGO_MODULES
        int  get_state_now();
        void direct_on();
        void direct_off();
        void direct_set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_U,     ///< Индекс канала дискретного выхода верхнего седла.
            DO_INDEX_L,     ///< Индекс канала дискретного выхода нижнего седла.

            DI_INDEX_U = 0, ///< Индекс канала дискретного входа верхнего седла.
            DI_INDEX_L,     ///< Индекс канала дискретного входа нижнего седла.
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-mixproof.
class valve_AS_mix_proof : public device, public i_mix_proof
    {
    public:
        valve_AS_mix_proof( u_int number );

        void open_upper_seat();
        void open_lower_seat();

        float get_value();

        void direct_set_state( int new_state );

        void direct_set_value( float new_value );

        void direct_off();

        void direct_on();

        int get_state_now();

    private:
#ifdef DEBUG_NO_WAGO_MODULES
        STATES state;  ///< Состояние устройства.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI_1 : public analog_wago_device
    {
    public:
        AI_1( u_int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type );

        int init_params();

        int set_cmd( const char *prop, u_int idx, double val );

    protected:

        enum PARAMS ///< Индексы параметров устройства.
            {
            P_ZERO_ADJUST_COEFF = 0, ///< Сдвиг нуля.
            };

        enum CONSTANTS
            {
            C_PARAMS_COUNT = 1, ///< Количество параметров устройства.

            C_AI_INDEX = 0,     ///< Индекс канала аналогового входа.
            };

        int save_device( char *buff, const char *prefix );

#ifdef DEBUG_NO_WAGO_MODULES
        float get_value();
#endif // DEBUG_NO_WAGO_MODULES

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        float get_value();
        void  direct_set_value( float new_value );

        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val() = 0;

        /// @brief Получение минимального значения выхода устройства.
        virtual float get_min_val() = 0;

#endif // DEBUG_NO_WAGO_MODULES

    private:
        saved_params_float par; ///< Сохраняемые параметры устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Температура.
class temperature_e : public AI_1
    {
    public:
        temperature_e( u_int number ): AI_1( number, DT_TE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень.
class level_e : public AI_1
    {
    public:
        level_e( u_int number ): AI_1( number, DT_LE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Текущий поток.
class flow_e : public AI_1
    {
    public:
        flow_e( u_int number ): AI_1( number, DT_FE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_MIN_PAR_NUMBER = 0,   ///< Индекс параметра минимального значения.
            C_MAX_PAR_NUMBER,       ///< Индекс параметра максимального значения.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Концентрация.
class concentration_e : public AI_1
    {
    public:
        concentration_e( u_int number ): AI_1( number, DT_QE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_MIN_PAR_NUMBER = 0,   ///< Индекс параметра минимального значения.
            C_MAX_PAR_NUMBER,       ///< Индекс параметра максимального значения.
            };                      
    };
//-----------------------------------------------------------------------------
/// @brief Устройство аналогового выхода 4-20 мА.
class analog_input_4_20 : public AI_1
    {
    public:
        analog_input_4_20( u_int number ): AI_1( number, DT_AI, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым выходом.
///
/// Это может быть управляемый клапан...
class AO_1 : public analog_wago_device
    {
    public:
        AO_1( u_int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): analog_wago_device( number, type, sub_type )
            {
            }

        int save_device( char *buff, const char *prefix )
            {
            sprintf( buff, "%s[%d]={V=", prefix, get_n() );

            if ( get_value() == 0 )
                {
                sprintf( buff + strlen( buff ), "0" );
                }
            else
                {
                sprintf( buff + strlen( buff ), "%.2f", get_value() );
                }
            sprintf( buff + strlen( buff ), ", M=%d},\n", get_manual_mode() );

            return strlen( buff );
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val() = 0;

        /// @brief Получение минимального значения входа устройства.
        virtual float get_min_val() = 0;

        float get_value();
        void  direct_set_value( float new_value );

    private:
        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть управляемый клапан...
class AO_0_100 : public AO_1
    {
    public:
        AO_0_100( u_int number ) : AO_1( number, DT_AO, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_AO_MIN_VALUE = 0,     ///< Индекс параметра минимального значения.
            C_AO_MAX_VALUE = 100,   ///< Индекс параметра максимального значения.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным входом.
///
/// Это может быть обратная связь, расход (есть/нет)...
class DI_1 : public digital_wago_device
    {
    public:
        DI_1( u_int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type, u_int dt = 0 ):
        digital_wago_device( number, type, sub_type )
            {
            set_change_time( dt );
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void direct_on();
        void direct_off();

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< Индекс канала дискретного входа.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления.
class valve_DO_1 : public DO_1
    {
    public:
        valve_DO_1( u_int number ) : DO_1( number, DT_V, DST_V_DO_1,true, false, false )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления.
class valve_DO_2 : public DO_2
    {
    public:
        valve_DO_2( u_int number ): DO_2( number, DT_V, DST_V_DO_2 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и одной обратной связью.
class valve_DO_1_DI_1 : public DO_1_DI_1
    {
    public:
        valve_DO_1_DI_1( u_int number ): DO_1_DI_1( number, DT_V, DST_V_DO_1_DI_1 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и двумя обратными связями.
class valve_DO_1_DI_2 : public DO_1_DI_2
    {
    public:
        valve_DO_1_DI_2( u_int number ): DO_1_DI_2( number, DT_V, DST_V_DO_1_DI_2 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями.
class valve_DO_2_DI_2 : public DO_2_DI_2
    {
    public:
        valve_DO_2_DI_2( u_int number ): DO_2_DI_2( number, DT_V, DST_V_DO_2_DI_2 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Насос.
class pump : public DO_1_DI_1
    {
    public:
        pump( u_int number ): DO_1_DI_1( number, DT_N, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Мешалка.
class mixer : public DO_1_DI_1
    {
    public:
        mixer( u_int number ): DO_1_DI_1( number, DT_M, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня.
class level_s : public DI_1
    {
    public:
        level_s( u_int number, u_int dt = 1000 );

        bool is_active();

    private:
        enum CONSTANTS
            {
            C_ACTIVE_STATE = 0
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора расхода.
class flow_s : public DI_1
    {
    public:
        flow_s( u_int number ): DI_1( number, DT_FS, DST_NONE )
            {            
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик обратной связи.
class feedback : public DI_1
    {
    public:
        feedback( u_int number ): DI_1( number, DT_FB, DST_NONE )
            {            
            }
    };
//-----------------------------------------------------------------------------
/// @brief Сигнал управления
class control_s : public DO_1
    {
    public:
        control_s( u_int number ): DO_1( number, DT_UPR, DST_NONE )
            {            
            }
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик.
class counter : public device,
    public i_counter,   
    public wago_device
    {
    public:
        counter( u_int number ): device( number, DT_CTR, DST_NONE ), 
            value( 0 ),
            last_read_value( 0 ),
            state( S_STOP )
            {            
            }

        virtual ~counter()
            {            
            }

        float get_value();
        void  direct_set_value( float new_value );
        int   get_state_now();
        void  direct_on();
        void  direct_off();
        void  direct_set_state( int new_state );
        void  print() const;

        void  pause();
        void  start();
        void  reset();
        u_int get_quantity();

    private:
        enum STATES
            {
            S_STOP,
            S_WORK,
            S_PAUSE,
            };

        enum CONSTANTS
            {
            AI_INDEX = 0,       ///< Индекс канала аналогового входа.

            MAX_VAL = 65535L,   ///< Максимальное значение счетчика.
            };

        u_int value;
        u_int last_read_value;

        STATES state;
    };
//-----------------------------------------------------------------------------
/// @brief Менеджер устройств.
///
/// Содержит информацию обо всех устройствах проекта.
class device_manager: public i_Lua_save_device
    {
    public:
        device_manager();

        virtual ~device_manager();

        /// @brief Получение устройства по его номеру.        
        device* get_device( device::DEVICE_TYPE dev_type, u_int dev_number );

        /// @brief Получение клапана по его номеру.
        i_DO_device* get_V( int number );

        /// @brief Получение насоса по номеру.
        i_DO_device* get_N( int number );

        /// @brief Получение мешалки по номеру.
        i_DO_device* get_M( int number );

        /// @brief Получение уровня по номеру.
        i_DI_device* get_LS( int number );

        /// @brief Получение расхода по номеру.
        i_DI_device* get_FS( int number );

        /// @brief Получение аналогового входа по номеру.
        i_AI_device* get_AI( int number );

        /// @brief Получение аналогового выхода по номеру.
        i_AO_device* get_AO( int number );

        /// @brief Получение счетчика по номеру.
        i_counter* get_CTR( int number );

        /// @brief Получение температуры по номеру.
        i_AI_device* get_TE( int number );

        /// @brief Получение текущего расхода по номеру.
        i_AI_device* get_FE( int number );

        /// @brief Получение текущего уровня по номеру.
        i_AI_device* get_LE( int number );

        /// @brief Получение обратной связи по номеру.
        i_DI_device* get_FB( int number );

        /// @brief Получение управляющего канала по номеру.
        i_DO_device* get_UPR( int number );

        /// @brief Получение текущей концентрации по номеру.
        i_AI_device* get_QE( int number );

        /// @brief Получение единственного экземпляра класса.
        static device_manager* get_instance();

        /// @brief Отладочная печать объекта в консоль.
        void print() const;

        /// @brief Получение фиктивного устройства.
        dev_stub* get_stub()
            {
            return &stub;
            }

        int init_params();

#ifdef __BORLANDC__
#pragma option -w-inl
#endif // __BORLANDC__
        int save_device( char *buff );

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

    protected:
        dev_stub stub;  ///< Устройство-заглушка, фиктивное устройство.

        struct range    ///< Диапазон устройств одного типа.
            {
            int start_pos;
            int end_pos;
            };

        /// Диапазоны устройств всех типов.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        /// @brief Получение индекса устройства по его номеру.        
        int get_device_n( device::DEVICE_TYPE dev_type, u_int dev_number );

        std::vector< device* > project_devices; ///< Все устройства.

        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < device_manager > instance;

    public:
        // @brief Установка числа устройств.
        //
        // Вызывается из Lua.
        wago_device* add_wago_device( int dev_type, int dev_sub_type, 
            u_int number, char * descr );  
    };
//-----------------------------------------------------------------------------
/// @brief таймер.
///
/// Предоставляет функциональность таймера.
class timer
    {
    public:
        enum STATE
            {
            S_STOP = 0, ///< Не работает.
            S_WORK,     ///< Работает.
            S_PAUSE,    ///< Пауза.
            };

        /// @brief Сохранение состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///        
        /// @return >= 0 - количество записанных байт.
        int save( char *buff );

        /// @brief Считывание состояния устройства из буфера.
        ///
        /// @param buff [ out ] - адрес буфера, откуда будут считываться данные.
        ///        
        /// @return >= 0 - количество считанных байт.
        int load( char *buff );

        /// @brief Получение размера буфера для сохранения состояния устройства.
        ///
        /// @return >= 0 - размер буфера, байт.
        int get_saved_size() const;

        timer();

        /// @brief Запуск таймера.
        void start();

        /// @brief Сброс таймера.
        void reset();

        /// @brief Пауза таймера.
        void pause();

        /// @brief Проверка исхода времени таймера.
        ///
        /// @return 1 - время вышло.
        /// @return 0 - время не вышло.
        bool is_time_up() const;

        /// @brief Получение времени работы таймера.
        ///
        /// @return - время работы таймера.
        u_long  get_work_time() const;

        /// @brief Установка задания таймера.
        ///
        /// @param new_countdown_time - задание.
        void set_countdown_time( u_long new_countdown_time );

        /// @brief Получение задания таймера.
        ///
        /// @return - задание таймера.
        u_long  get_countdown_time() const;

        /// @brief Получение состояния таймера.
        ///
        /// @return - состояние таймера.
        STATE get_state() const;

    private:
        u_long  last_time;  ///< Время, когда таймер был запущен/остановлен.
        u_long  work_time;  ///< Время работы таймера.

        STATE   state;           ///< Состояние.
        u_long  countdown_time;  ///< Задание таймера.
    };
//-----------------------------------------------------------------------------
/// @brief таймер.
///
/// Реализация группы таймеров.
class timer_manager
    {
    public:
        /// @param timers_count - количество таймеров в группе.
        timer_manager( u_int timers_count );

        ~timer_manager();

        /// @brief Безопасное получение таймера по индексу.
        ///
        /// @param index - индекс таймера.
        ///
        /// @return - таймер с нужным индексом, заглушка - в случае выхода за 
        /// диапазон.
        timer* operator[] ( unsigned int index );

    private:
        u_int   timers_cnt; ///< Количество таймеров.
        timer   *timers;    ///< Таймеры.

        timer   stub;       ///< Заглушка.
    };
//-----------------------------------------------------------------------------
///@brief Получение менеджера устройств.
///
///@return Менеджер устройств проекта.
device_manager* G_DEVICE_MANAGER();
//-----------------------------------------------------------------------------
/// @brief Получение клапана по номеру.
///
/// @param number - номер клапана.
/// @return - клапан с заданным номером. Если нет такого клапана, возвращается
/// заглушка (@ref dev_stub).
i_DO_device* V( int number );
//-----------------------------------------------------------------------------
/// @brief Получение насоса по номеру.
///
/// @param number - номер насоса.
/// @return - насос с заданным номером. Если нет такого насоса, возвращается
/// заглушка (@ref dev_stub).
i_DO_device* N( int number );
//-----------------------------------------------------------------------------
/// @brief Получение мешалки по номеру.
///
/// @param number - номер мешалки.
/// @return - мешалка с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* M( int number );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального уровня по номеру.
///
/// @param number - номер сигнального уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* LS( int number );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального расхода по номеру.
///
/// @param number - номер сигнального расхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* FS( int number );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового входа по номеру.
///
/// @param number - номер аналогового входа.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* AI( int number );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового выхода по номеру.
///
/// @param number - номер аналогового выхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AO_device* AO( int number );
//-----------------------------------------------------------------------------
/// @brief Получение счетчика по номеру.
///
/// @param number - номер счетчика.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_counter* CTR( int number );
//-----------------------------------------------------------------------------
/// @brief Получение температуры по номеру.
///
/// @param number - номер температуры.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* TE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение текущего расхода по номеру.
///
/// @param number - номер текущего расхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* FE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение текущего уровня по номеру.
///
/// @param number - номер текущего уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* LE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение обратной связи по номеру.
///
/// @param number - номер обратной связи.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* FB( int number );
//-----------------------------------------------------------------------------
/// @brief Получение канала управления по номеру.
///
/// @param number - номер канала управления.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* UPR( int number );
//-----------------------------------------------------------------------------
/// @brief Получение текущей концентрации по номеру.
///
/// @param number - номер текущей концентрации.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* QE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение насоса по номеру.
///
/// @return - виртуальное устройство.
dev_stub* STUB();
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
