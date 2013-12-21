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
/// Wago (состояния хранят в себе).
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
#include <string>

#include "dtime.h"
#include "wago.h"
#include "g_device.h"

#include "param_ex.h"

#include "smart_ptr.h"

//-----------------------------------------------------------------------------
/// @brief Устройство c параметрами.
///
/// Параметры хранятся в энергонезависимой памяти (сохраняют значение после
/// перезагрузки PAC). Доступ к параметрам производится на основе номера и
/// смещения (итоговый индекс равен их сумме). Каждый параметр имеет имя.
class par_device
    {
    friend class device;

    public:
        /// @param par_cnt - количество параметров.
        par_device( u_int par_cnt );

        ~par_device();

        /// @brief Сохранение в виде скрипта Lua.
        ///
        /// Сохраняем на основе названий параметров в виде таблицы:
        /// имя_параметра = 'значение'.
        ///
        /// @param str - строка, куда сохраняем.
        void save_device( char *str );

        /// @brief Выполнение команда (установка значения параметра).
        ///
        /// @param name - имя команды (модифицируемого параметра).
        /// @param val - новое значение.
        ///
        /// @return 0 - ок.
        /// @return 1 - ошибка, параметр с таким именем не найден.
        int set_cmd( const char *name, double val );

        /// @brief Установка значения параметра.
        ///
        /// @param idx - индекс параметра.
        /// @param offset - смещение индекса.
        /// @param value - новое значение.
        void set_par( u_int idx, u_int offset, float value );

        /// @brief Получение значения параметра.
        ///
        /// @param idx - индекс параметра.
        /// @param offset - смещение индекса.
        ///
        /// @return значение параметра.
        float get_par( u_int idx, u_int offset );

        /// @brief Задание имени параметра.
        ///
        /// @param idx - индекс параметра.
        /// @param offset - смещение индекса.
        /// @param name - имя параметра.
        void set_par_name( u_int idx, u_int offset, const char* name );

        /// @brief Получение параметров для хранения настроек ошибок устройства.
        saved_params_u_int_4* get_err_par() const
            {
            return err_par;
            }

        /// @brief Установка параметров для хранения настроек ошибок устройства.
        void set_err_par( saved_params_u_int_4* err_par )
            {
            this->err_par = err_par;
            }

    protected:
        /// @brief Ошибки устройства.
        saved_params_u_int_4 *err_par;

        enum CONSTANTS
            {
            C_MAX_PAR_NAME_LENGTH = 20, ///< Максимальная длина имени параметра.
            };

        saved_params_float *par; ///< Параметры.
        char **par_name;         ///< Названия параметров.
    };
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

        /// @brief Получение значения счетчика.
        virtual float get_flow() = 0;

        /// @brief Получение состояния работы счетчика.
        virtual int get_state() = 0;

        virtual ~i_counter();
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс противосмешивающего клапана (mixproof).
class i_mix_proof
    {
    public:
        enum STATES
            {
            ST_CLOSE = 0,   ///< Закрыть.
            ST_OPEN,        ///< Открыть.
            ST_UPPER_SEAT,  ///< Открыть верхнее седло.
            ST_LOWER_SEAT,  ///< Открыть нижнее седло.
            };

        /// @brief Открыть верхнее седло.
        virtual void open_upper_seat() = 0;

        /// @brief Открыть нижнее седло.
        virtual void open_lower_seat() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного входа.
///
/// Обратная связь, предельный уровень и т.д. являются примерами таких
/// устройств.
class i_DI_device: public i_cmd_device
    {
    public:
        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state () = 0;

        /// @brief Проверка активного состояния.
        ///
        /// Для уровня - сработал он или нет.
        ///
        /// @return 1 - устройство активно.
        /// @return 0 - устройство неактивно.
        virtual bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного выхода.
///
/// Клапан, мешалка и т.д. являются примерами таких устройств.
class i_DO_device: public i_DI_device
    {
    public:
        /// @brief Включение устройства с учетом ручного режима.
        virtual void on();

        virtual void print() const = 0;

        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief Установка нового состояния устройства с учетом ручного режима.
        ///
        /// @param new_state - новое состояние устройства.
        virtual void set_state( int new_state );

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

        /// @brief Включен ли ручной режим.
        ///
        /// В ручном режиме устройство управляется по командам от сервера
        /// (управляющая программа не влияет на устройство).
        ///
        /// @return true - ручной режим включен.
        /// @return false - ручной режим выключен.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на на основе аналогового входа.
///
/// Температура, расход и т.д. являются примерами таких устройств.
class i_AI_device: public i_cmd_device
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
        virtual void set_value( float new_value );

    protected:
        /// @brief Установка текущего состояния устройства.
        ///
        /// @param new_value - новое состояние устройства.
        virtual void direct_set_value( float new_value ) = 0;

        /// @brief Включен ли ручной режим.
        ///
        /// В ручном режиме устройство управляется по командам от сервера
        /// (управляющая программа не влияет на устройство).
        ///
        /// @return true - ручной режим включен.
        /// @return false - ручной режим выключен.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства как с аналоговыми, так и дискретными каналами.
class i_DO_AO_device: public i_AO_device, public i_DO_device
    {
    public:
        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void off() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Класс универсального простого устройства, который используется в
/// режимах.
class device : public i_DO_AO_device, public par_device
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

#ifdef RM_PAC
        /// @brief Сохранение состояния устройства в виде скрипта Lua.
        ///
        /// @param prefix - префикс перед строкой скрипта (обычно название
        /// таблицы).
        /// @param buff [out] - буфер записи строки.
        virtual int rm_save_device_state( char *buff, const char *prefix );
#endif // RM_PAC

        enum CONSTANTS
            {
            C_DEVICE_TYPE_CNT = 17,     ///< Количество типов устройств.

            C_MAX_NAME = 10
            };

        static const char DEV_NAMES[][ 5 ];

        /// Типы устройств.
        enum DEVICE_TYPE
            {
            DT_NONE = -1,      ///< Тип не определен.

            DT_V = 0,   ///< Клапан.
            DT_VC,      ///< Управляемый клапан.
            DT_M,       ///< Двигатель.
            DT_LS,      ///< Уровень (есть/нет).
            DT_TE,      ///< Температура.
            DT_FS,      ///< Расход (есть/нет).
            DT_GS,      ///< Датчик положения.
            DT_FQT,     ///< Счетчик.
            DT_LT,      ///< Уровень (значение).
            DT_QT,      ///< Концентрация.

            DT_HA,      ///< Аварийная звуковая сигнализация.
            DT_HL,      ///< Аварийная световая сигнализация.
            DT_SB,      ///< Кнопка.
            DT_DI,      ///< Дискретный входной сигнал.
            DT_DO,      ///< Дискретный выходной сигнал.
            DT_AI,      ///< Аналоговый входной сигнал.
            DT_AO,      ///< Аналоговый выходной сигнал.
            };

        /// Подтипы устройств.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = -1,      ///< Подтип не определен.

            //V
            DST_V_DO1 = 1,      ///< Клапан с одним каналом управления.
            DST_V_DO2,          ///< Клапан с двумя каналами управления.
            DST_V_DO1_DI1_FB_OFF,  ///< Клапан с одним каналом управления и одной обратной связью на закрытое состояние.
            DST_V_DO1_DI1_FB_ON,   ///< Клапан с одним каналом управления и одной обратной связью на открытое состояние.
            DST_V_DO1_DI2,      ///< Клапан с одним каналом управления и двумя обратными связями.
            DST_V_DO2_DI2,      ///< Клапан с двумя каналами управления и двумя обратными связями.
            DST_V_MIXPROOF,     ///< Клапан противосмешивающий.
            DST_V_AS_MIXPROOF,  ///< Клапан с двумя каналами управления и двумя обратными связями с AS интерфейсом (противосмешивающий).

            //LS
            DST_LS_MIN = 1,     ///< Подключение по схеме минимум.
            DST_LS_MAX,         ///< Подключение по схеме максимум.

            //M,
            DST_M = 1,          ///< Мотор без управления частотой вращения.
            DST_M_FREQ,         ///< Мотор с управлением частотой вращения.
            DST_M_REV, 	        ///< Мотор с реверсом с управлением частотой вращения.
            DST_M_REV_FREQ,     ///< Мотор с реверсом без управления частотой вращения.
            };

        device( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~device();

        const char *get_name() const
            {
            return name;
            }

        const char *get_description() const
            {
            return description;
            }

        void set_descr( const char *new_description );

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

        /// @brief Получение порядкового номера устройства.
        ///
        /// @return - номер устройства.
        virtual u_int_4 get_serial_n() const
            {
            return s_number;
            }

        /// @brief Установка порядкового номера устройства.
        ///
        /// @return - номер устройства.
        void set_serial_n( u_int_4 s_n )
            {
            s_number = s_n;
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

        /// @brief Установка дополнительных свойств, значения которых -
        /// устройства.
        ///
        /// Для использования в Lua.
        virtual void set_property( const char* field, device* dev )
            {
            }

    protected:
        /// @brief Сохранение дополнительных данных устройства в виде скрипта Lua.
        ///
        /// @param buff [out] - буфер записи строки.
        virtual void save_device_ex( char *buff )
            {
            }

        u_int_4 s_number;            ///< Последовательный номер устройства.

        DEVICE_TYPE     type;        ///< Тип устройства.
        DEVICE_SUB_TYPE sub_type;    ///< Подтип устройства.

        bool get_manual_mode() const
            {
            return is_manual_mode;
            }

    private:
        bool is_manual_mode;      ///< Признак ручного режима.

        char name[ C_MAX_NAME ];
        char *description;
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
        dev_stub(): device( 0, DT_NONE, DST_NONE, 1 )
            {
            }

        u_int_4 get_serial_n() const;
        void    print() const;

        float   get_value();
        void    direct_set_value( float new_value );

        void    direct_on();
        void    direct_off();
        void    direct_set_state( int new_state );
        int     get_state();

        void    pause();
        void    start();
        void    reset();
        u_int   get_quantity();
        float   get_flow();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый класс для различных дискретных устройств.
class digital_wago_device : public device,
    public wago_device
    {
    public:
        digital_wago_device( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~digital_wago_device();

        float   get_value();
        void    direct_set_value( float new_value );
        void    direct_set_state( int new_state );

#ifdef DEBUG_NO_WAGO_MODULES
        /// @brief Получение состояния объекта.
        ///
        /// @return - состояние объекта.
        int  get_state();

        void direct_on();
        void direct_off();
#endif // DEBUG_NO_WAGO_MODULES

        void    print() const;

#ifdef DEBUG_NO_WAGO_MODULES
    protected:
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
        analog_wago_device( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt ):
        device( dev_name, type, sub_type, par_cnt ),
        wago_device( dev_name )
#ifdef DEBUG_NO_WAGO_MODULES
            ,value( 0 )
#endif  // DEBUG_NO_WAGO_MODULES
            {
            }

        void  direct_set_state( int new_state );
        int   get_state();

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
class DO1 : public digital_wago_device
    {
    public:
        DO1( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type ):
        digital_wago_device( dev_name, type, sub_type, 0 )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state();
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
class DO2 : public digital_wago_device
    {
    public:
        DO2( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ):
        digital_wago_device( dev_name, type, sub_type, par_cnt )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state();
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
/// @brief Устройство с обратными связями.
///
class valve: public digital_wago_device
    {
    public:
        /// @param is_on_fb - использовать обратную связь на включенное состояние.
        /// @param is_off_fb - использовать обратную связь на выключенное состояние.
        /// @param number - номер устройства.
        /// @param type - тип устройства.
        /// @param sub_type - подтип устройства.
        valve( bool is_on_fb, bool is_off_fb,
            const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type );

        /// @param number - номер устройства.
        /// @param type - тип устройства.
        /// @param sub_type - подтип устройства.
        valve( const char *dev_name, device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type ):
            digital_wago_device( dev_name, type, sub_type, 0 ),
            is_on_fb( false ),
            is_off_fb( false ),
            start_switch_time( 0 )
            {
            }

        /// @brief Получение значения обратной связи на включенное состояние.
        virtual int get_on_fb_value()
            {
            return on_fb;
            }

        /// @brief Получение значения обратной связи на выключенное состояние.
        virtual int get_off_fb_value()
            {
            return off_fb;
            }

        /// @brief Сохранение дополнительных данных.
        void save_device_ex( char *buff );

        //Состояния клапана (расширенное).
        enum VALVE_STATE_EX
            {
            VX_ON_FB_OFF  = 11,  ///< Включен, ОС отключена.
            VX_OFF_FB_OFF = 10,  ///< Выключен, ОС отключена.

            VX_UPPER_SEAT_FB_OFF = 32, ///< Открыто верхнее седло, ОС отключена.
            VX_LOWER_SEAT_FB_OFF = 33, ///< Открыто нижнее седло, ОС отключена.

            VX_UPPER_SEAT = 2,   ///< Открыто верхнее седло.
            VX_LOWER_SEAT = 3,   ///< Открыто нижнее седло.

            VX_ON_FB_OK  = 1,    ///< Включен, ОС ок.
            VX_OFF_FB_OK = 0,    ///< Выключен, ОС ок.

            VX_ON_FB_ERR  = -2,  ///< Включен, ошибка ОС.
            VX_OFF_FB_ERR = -3,  ///< Выключен, ошибка ОС.

            //Учет ручного режима.
            VX_ON_FB_OFF_MANUAL  = 31,   ///< Включен вручную, ОС отключена.
            VX_OFF_FB_OFF_MANUAL = 30,   ///< Выключен вручную, ОС отключена.

            VX_LOWER_SEAT_MANUAL = 23,   ///< Открыто вручную нижнее седло.
            VX_UPPER_SEAT_MANUAL = 22,   ///< Открыто вручную верхнее седло.

            VX_ON_FB_OK_MANUAL  = 21,    ///< Включен вручную, ОС ок.
            VX_OFF_FB_OK_MANUAL = 20,    ///< Выключен вручную, ОС ок.

            VX_ON_FB_ERR_MANUAL  = -12,  ///< Включен вручную, ошибка ОС.
            VX_OFF_FB_ERR_MANUAL = -13,  ///< Выключен вручную, ошибка ОС.


            VX_UNKNOWN = 100,            ///< Неизвестное состояние.
            };

        /// @brief Получение расширенного состояния клапана (учет обратной
        /// связи, ручного режима, ...).
        int get_state()
#if _MSC_VER >= 1700
        final;
#else
        ;
#endif

#ifdef DEBUG_NO_WAGO_MODULES
        int set_cmd( const char *prop, u_int idx, double val );
#endif // DEBUG_NO_WAGO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:

        ///Состояние клапана без учета обратной связи.
        enum VALVE_STATE
            {
            V_LOWER_SEAT = 3, ///< Открыто нижнее седло.
            V_UPPER_SEAT = 2, ///< Открыто верхнее седло.

            V_ON  = 1,        ///< Включен.
            V_OFF = 0,        ///< Выключен.
            };

        /// @brief Получение состояния клапана без учета обратной связи.
        virtual VALVE_STATE get_valve_state() = 0;

        /// @brief Получение состояния обратной связи.
        virtual bool get_fb_state()
            {
            return true;
            }

    protected:
        enum FB_STATE
            {
            FB_IS_AND_OFF = 0, ///< Обратная связь отключена.
            FB_IS_AND_ON,      ///< Обратная связь включена.
            };

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 2,  ///Количество дополнительных параметров.

            P_ON_TIME = 1,
            P_FB
            };

    private:
        /// @brief Есть обратная связь на включенное состояние.
        bool is_on_fb;

        /// @brief Есть обратная связь на выключенное состояние.
        bool is_off_fb;

        bool on_fb;
        bool off_fb;

    protected:
        u_long start_switch_time;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним дискретным выходом и одним дискретным входом.
///
class valve_DO1_DI1_off : public valve
    {
    public:
        valve_DO1_DI1_off( const char *dev_name );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
            DI_INDEX = 0,           ///< Индекс канала дискретного входа.
            };

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_WAGO_MODULES

        // Интерфейс valve для реализации получения расширенного состояния с
        // учетом всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return ( VALVE_STATE ) digital_wago_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_WAGO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return valve::get_fb_state();
#else
            int o = get_DO( DO_INDEX );
            int i = get_DI( DI_INDEX );

            if ( o != i )
                {
                start_switch_time = get_millisec();
                return true;
                }

            if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_WAGO_MODULES
            }

#ifndef DEBUG_NO_WAGO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX );
            }

        int get_on_fb_value()
            {
            return false;
            }
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним дискретным выходом и одним дискретным входом.
///
class valve_DO1_DI1_on : public valve
    {
    public:
        valve_DO1_DI1_on( const char *dev_name ): valve( true, false,
            dev_name, DT_V, DST_V_DO1_DI1_FB_ON )
            {
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
            DI_INDEX = 0,           ///< Индекс канала дискретного входа.
            };

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_WAGO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return ( VALVE_STATE ) digital_wago_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            return ( VALVE_STATE ) !o;
#endif // DEBUG_NO_WAGO_MODULES
            }

        bool get_fb_state()
            {
            int o = get_DO( DO_INDEX );
            int i = get_DI( DI_INDEX );

            if ( o == i )
                {
                start_switch_time = get_millisec();
                return true;
                }

            if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
            }

#ifndef DEBUG_NO_WAGO_MODULES
        int get_on_fb_value()
            {
            return get_DI( DI_INDEX );
            }

        int get_off_fb_value()
            {
            return false;
            }
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и двумя обратными связями.
///
class valve_DO1_DI2 : public valve
    {
    public:
        valve_DO1_DI2( const char *dev_name ):
          valve( true, true, dev_name, DT_V, DST_V_DO1_DI2 )
            {
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_WAGO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return ( VALVE_STATE ) digital_wago_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_WAGO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return valve::get_fb_state();
#else
            int o = get_DO( DO_INDEX );
            int i1 = get_DI( DI_INDEX_1 );
            int i0 = get_DI( DI_INDEX_2 );

            if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
                ( o == 1 && i1 == 1 && i0 == 0 ) )
                {
                return true;
                }

            if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_WAGO_MODULES
            }

#ifndef DEBUG_NO_WAGO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_1 );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_2 );
            }
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями.
///
class valve_DO2_DI2 : public valve
    {
    public:
        valve_DO2_DI2( const char *dev_name ):
            valve( true, true, dev_name, DT_V, DST_V_DO2_DI2 )
            {
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0,         ///< Индекс №1 канала дискретного выхода.
            DO_INDEX_2,             ///< Индекс №2 канала дискретного выхода.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_WAGO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return ( VALVE_STATE ) digital_wago_device::get_state();
#else
            int o1 = get_DO( DO_INDEX_2 );

            return ( VALVE_STATE ) o1;
#endif // DEBUG_NO_WAGO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return true;
#else
            int o0 = get_DO( DO_INDEX_1 );
            int o1 = get_DO( DO_INDEX_2 );
            int i0 = get_DI( DI_INDEX_1 );
            int i1 = get_DI( DI_INDEX_2 );

            if ( o1 == i1 && o0 == i0 )
                {
                return true;
                }

            if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_WAGO_MODULES
            }

#ifndef DEBUG_NO_WAGO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_1 );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_2 );
            }
#endif // DEBUG_NO_WAGO_MODULES

    };
//-----------------------------------------------------------------------------
/// @brief Клапан mixproof.
class valve_mix_proof : public i_mix_proof,  public valve
    {
    public:
        valve_mix_proof( const char *dev_name
            ): valve( true, true, dev_name, DT_V, DST_V_MIXPROOF )
            {
            }

        /// @brief Открыть верхнее седло.
        void open_upper_seat();

        /// @brief Открыть нижнее седло.
        void open_lower_seat();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_U,     ///< Индекс канала дискретного выхода верхнего седла.
            DO_INDEX_L,     ///< Индекс канала дискретного выхода нижнего седла.

            DI_INDEX_OPEN = 0, ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE,     ///< Индекс канала дискретного входа Закрыт.
            };

         void direct_set_state( int new_state );

#ifndef DEBUG_NO_WAGO_MODULES
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_WAGO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return ( VALVE_STATE ) digital_wago_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return V_UPPER_SEAT;
            if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return V_LOWER_SEAT;

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_WAGO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return true;
#else
            int o = get_DO( DO_INDEX );
            int i0 = get_DI( DI_INDEX_CLOSE );
            int i1 = get_DI( DI_INDEX_OPEN );

            if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
                ( o == 1 && i1 == 1 && i0 == 0 ) )
                {
                return true;
                }

            if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return true;
            if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return true;

            if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_WAGO_MODULES
            }

#ifndef DEBUG_NO_WAGO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_CLOSE );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_OPEN );
            }
#endif // DEBUG_NO_WAGO_MODULES

    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-mixproof.
class valve_AS_mix_proof : public device, public i_mix_proof
    {
    public:
        valve_AS_mix_proof( const char *dev_name );

        void open_upper_seat();
        void open_lower_seat();

        float get_value();

        void direct_set_state( int new_state );

        void direct_set_value( float new_value );

        void direct_off();

        void direct_on();

        int get_state();

    private:
#ifdef DEBUG_NO_WAGO_MODULES
        STATES state;  ///< Состояние устройства.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI1 : public analog_wago_device
    {
    public:
        AI1( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt, u_int *start_par_idx );

    protected:

        enum CONSTANTS
            {
            P_ZERO_ADJUST_COEFF = 1,    ///< Сдвиг нуля.

            ADDITIONAL_PARAM_COUNT = 1, ///< Количество параметров.

            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };

        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val()
            {
            return 0;
            }

        /// @brief Получение минимального значения выхода устройства.
        virtual float get_min_val()
            {
            return 0;
            }
#ifdef DEBUG_NO_WAGO_MODULES
        float get_value();
#endif // DEBUG_NO_WAGO_MODULES

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        float get_value();
        void  direct_set_value( float new_value );

#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Температура.
class temperature_e : public AI1
    {
    public:
        temperature_e( const char *dev_name ): AI1( dev_name, DT_TE, DST_NONE, 0, 0 )
            {
            }

        float get_value()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            float v = analog_wago_device::get_value();

            return -1000 == v ? -1000 : AI1::get_value();
#else
            float v = get_AI( C_AI_INDEX, 0, 0 );
            return -1000 == v ? -1000 :
                get_par( P_ZERO_ADJUST_COEFF, 0 ) + v;
#endif
            }
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень.
class level_e : public AI1
    {
    public:
        level_e( const char *dev_name ): AI1( dev_name, DT_TE, DST_NONE, 0, 0 )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Концентрация.
class concentration_e : public AI1
    {
    public:
        concentration_e( const char *dev_name ): AI1( dev_name, DT_QT, DST_NONE,
            ADDITIONAL_PARAM_COUNT, &start_param_idx )
            {
            set_par_name( P_MIN_V,  start_param_idx, "P_MIN_V" );
            set_par_name( P_MAX_V,  start_param_idx, "P_MAX_V" );
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 2,

            P_MIN_V = 1,   ///< Индекс параметра минимального значения.
            P_MAX_V,       ///< Индекс параметра максимального значения.
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство аналогового входа.
class analog_input : public AI1
    {
    public:
        analog_input( const char *dev_name ): AI1( dev_name, DT_AI, DST_NONE,
            ADDITIONAL_PARAM_COUNT, &start_param_idx )
            {
            set_par_name( P_MIN_V,  start_param_idx, "P_MIN_V" );
            set_par_name( P_MAX_V,  start_param_idx, "P_MAX_V" );
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 2,

            P_MIN_V = 1,   ///< Индекс параметра минимального значения.
            P_MAX_V,       ///< Индекс параметра максимального значения.
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым выходом.
///
/// Это может быть управляемый клапан...
class AO1 : public analog_wago_device
    {
    public:
        AO1( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt ):
        analog_wago_device( dev_name, type, sub_type, par_cnt )
            {
            }

        virtual float get_min_value() = 0;
        virtual float get_max_value() = 0;

#ifndef DEBUG_NO_WAGO_MODULES
        float get_value();
        void  direct_set_value( float new_value );
#endif // DEBUG_NO_WAGO_MODULES

    protected:
        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть управляемый клапан...
class analog_output : public AO1
    {
    public:
        analog_output( const char *dev_name ) :
            AO1( dev_name, DT_AO, DST_NONE, ADDITIONAL_PARAM_COUNT )
              {
              }

          float get_min_value()
              {
              return get_par( P_MIN_VALUE, 0 );
              }

          float get_max_value()
              {
              return get_par( P_MAX_VALUE, 0 );
              }

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 2,

            P_MIN_VALUE = 1,   ///< Индекс параметра минимального значения.
            P_MAX_VALUE,       ///< Индекс параметра максимального значения.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Управляемый клапан.
class analog_valve : public AO1
    {
    public:
        analog_valve( const char *dev_name ): AO1( dev_name, DT_VC, DST_NONE, 0 )
            {
            }

        float get_min_value()
            {
            return C_MIN;
            }

        float get_max_value()
            {
            return C_MAX;
            }

    private:
        enum CONSTANTS
            {
            C_MIN = 0,   ///< Минимальное значение.
            C_MAX = 100, ///< Максимальное значение.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным входом.
///
/// Это может быть обратная связь, расход (есть/нет)...
class DI1 : public digital_wago_device
    {
    public:
        DI1( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ):
        digital_wago_device( dev_name, type, sub_type, par_cnt )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        void direct_on();
        void direct_off();

        int get_state()
            {
            return get_DI( DI_INDEX );
            }

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< Индекс канала дискретного входа.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления.
class valve_DO1 : public valve
    {
    public:
        valve_DO1( const char *dev_name ) : valve( dev_name, DT_V, DST_V_DO1 )
            {
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_WAGO_MODULES

    protected:
        /// @brief Получение состояния клапана без учета обратной связи.
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_WAGO_MODULES
            return ( VALVE_STATE ) digital_wago_device::get_state();
#else
            return ( VALVE_STATE ) get_DO( DO_INDEX );
#endif // DEBUG_NO_WAGO_MODULES
            };

        /// @brief Получение состояния обратной связи.
        bool get_fb_state()
            {
            return true;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления.
class valve_DO2 : public DO2
    {
    public:
        valve_DO2( const char *dev_name ): DO2( dev_name, DT_V, DST_V_DO2, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Электродвигатель (мешалка, насос).
class motor : public device, public wago_device
    {
    public:
        motor( const char *dev_name, device::DEVICE_SUB_TYPE sub_type ):
            device( dev_name, DT_M, sub_type, ADDITIONAL_PARAM_COUNT ),
            wago_device( dev_name )
#ifdef DEBUG_NO_WAGO_MODULES
            ,state( 0 ),
            freq( 0 )
#endif // DEBUG_NO_WAGO_MODULES
            {
            set_par_name( P_ON_TIME,  0, "P_ON_TIME" );
            }

        void save_device_ex( char *buff );

        float get_value();

        void direct_set_value( float value );

        void direct_set_state( int new_state );

        int  get_state();

        void direct_on();

        void direct_off();

    private:
        u_int start_param_idx;

        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 1,

            C_MIN_VALUE = 0,
            C_MAX_VALUE = 100,

            P_ON_TIME = 1,    ///< Индекс параметра времени включения (мсек).

            DO_INDEX = 0,         ///< Индекс канала дискретного выхода.
            DO_INDEX_REVERSE = 1, ///< Индекс канала дискретного выхода реверса.

            DI_INDEX = 0,     ///< Индекс канала дискретного входа.

            AO_INDEX = 0,     ///< Индекс канала аналогового выхода.
            };

        u_long start_switch_time;

#ifdef DEBUG_NO_WAGO_MODULES
        char  state;  ///< Состояние устройства.

        float freq;   ///< Состояние устройства (частота).
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня.
class level_s : public DI1
    {
    public:
        level_s( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

        bool is_active();

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 1,

            P_DT = 1,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора расхода.
class flow_s : public DI1
    {
    public:
        flow_s( const char *dev_name ): DI1( dev_name, DT_FS, DST_NONE,
            ADDITIONAL_PARAMS_COUNT )
            {
            set_par_name( P_DT,  0, "P_DT" );
            }

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 1,

            P_DT = 1,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик положения.
class state_s : public DI1
    {
    public:
        state_s( const char *dev_name ): DI1( dev_name, DT_GS, DST_NONE,
            ADDITIONAL_PARAMS_COUNT )
            {
            set_par_name( P_DT,  0, "P_DT" );
            }

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 1,

            P_DT = 1,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик дискретного входа связи.
class DI_signal : public DI1
    {
    public:
        DI_signal( const char *dev_name ): DI1( dev_name, DT_DI, DST_NONE, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Кнопка.
class button : public DI1
    {
    public:
        button( const char *dev_name ): DI1( dev_name, DT_SB, DST_NONE, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Дискретный сигнал управления.
class DO_signal : public DO1
    {
    public:
        DO_signal( const char *dev_name ): DO1( dev_name, DT_DO, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Аварийная звуковая сигнализация.
class siren : public DO1
    {
    public:
        siren( const char *dev_name ): DO1( dev_name, DT_HA, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Аварийная световая сигнализация.
class lamp : public DO1
    {
    public:
        lamp( const char *dev_name ): DO1( dev_name, DT_HL, DST_NONE )
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
        counter( const char *dev_name ): device(
            dev_name, DT_FQT, DST_NONE, ADDITIONAL_PARAMS_COUNT ),
            wago_device( dev_name ),
            value( 0 ),
            last_read_value( 0 ),
            state( S_STOP ),
            flow_value( 0 )
            {
            set_par_name( P_MIN_FLOW,  0, "P_MIN_FLOW" );
            set_par_name( P_MAX_FLOW,  0, "P_MAX_FLOW" );
            set_par_name( P_CZ,        0, "P_CZ" );
            set_par_name( P_DT,        0, "P_DT" );
            }

        virtual ~counter()
            {
            }

        float get_value();
        void  direct_set_value( float new_value );
        int   get_state();
        void  direct_on();
        void  direct_off();
        void  direct_set_state( int new_state );
        void  print() const;

        void  pause();
        void  start();
        void  reset();
        u_int get_quantity();
        float get_flow();

        void set_property( const char* field, device* dev );


        int set_cmd( const char *prop, u_int idx, double val )
            {
            switch ( prop[ 0 ] )
                {
            case 'F':
                flow_value = ( float ) val;
                break;

            default:
                return device::set_cmd( prop, idx, val );
                }

            return 0;
            }

        //Lua.
        void save_device_ex( char *buff )
            {
            sprintf( buff, "F=%.2f, ", get_flow() );
            }

    private:
        enum STATES
            {
            S_STOP,
            S_WORK,
            S_PAUSE,

            S_ERROR = -1,
            };

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 4,

            P_MIN_FLOW = 1,
            P_MAX_FLOW,
            P_CZ,
            P_DT,

            AI_FLOW_INDEX = 0,  ///< Индекс канала аналогового входа (поток).
            AI_Q_INDEX    = 1,  ///< Индекс канала аналогового входа (объем).

            MAX_VAL = 65535L,   ///< Максимальное значение счетчика.
            };

        u_int value;
        u_int last_read_value;

        STATES state;

        float flow_value;

        std::vector < device* > motors;
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

        /// @brief Получение устройства.
        device* get_device( int dev_type, const char *dev_name );

        /// @brief Получение устройства.
        device* get_device( u_int serial_dev_n )
            {
            if ( serial_dev_n < project_devices.size() )
            	{
                return project_devices[ serial_dev_n ];
            	}

            return &stub;
            }

        /// @brief Получение клапана по его номеру.
        i_DO_device* get_V( const char *dev_name );

        /// @brief Получение управляемого клапана по номеру.
        i_AO_device* get_VC( const char *dev_name );

        /// @brief Получение двигателя по номеру.
        i_DO_AO_device* get_M( const char *dev_name );

        /// @brief Получение уровня по номеру.
        i_DI_device* get_LS( const char *dev_name );

        /// @brief Получение расхода по номеру.
        i_DI_device* get_FS( const char *dev_name );

        /// @brief Получение датчика положения по номеру.
        i_DI_device* get_GS( const char *dev_name );

        /// @brief Получение аналогового входа по номеру.
        i_AI_device* get_AI( const char *dev_name );

        /// @brief Получение аналогового выхода по номеру.
        i_AO_device* get_AO( const char *dev_name );

        /// @brief Получение счетчика по номеру.
        i_counter* get_FQT( const char *dev_name );

        /// @brief Получение температуры по номеру.
        i_AI_device* get_TE( const char *dev_name );

        /// @brief Получение текущего уровня по номеру.
        i_AI_device* get_LT( const char *dev_name );

        /// @brief Получение обратной связи по номеру.
        i_DI_device* get_DI( const char *dev_name );

        /// @brief Получение кнопки по номеру.
        i_DI_device* get_SB( const char *dev_name );

        /// @brief Получение управляющего канала по номеру.
        i_DO_device* get_DO( const char *dev_name );

        /// @brief Получение звуковой сигнализации.
        i_DO_device* get_HA( const char *dev_name );

        /// @brief Получение световой сигнализации.
        i_DO_device* get_HL( const char *dev_name );

        /// @brief Получение текущей концентрации по номеру.
        i_AI_device* get_QT( const char *dev_name );

        /// @brief Получение единственного экземпляра класса.
        static device_manager* get_instance();

        /// @brief Отладочная печать объекта в консоль.
        void print() const;

        const char* get_name_in_Lua() const
            {
            return "Device manager";
            }

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

#ifdef RM_PAC
        int rm_save_device( char *buff );

        int rm_save_device_state( char *buff );
#endif // RM_PAC

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

        //int save_params_as_Lua_str( char* str )
        //    {
        //    str[ 0 ] = 0;

        //    for ( u_int i = 0; i < project_devices.size(); i++ )
        //        {
        //        project_devices[ i ]->save_params_as_Lua_str( str + strlen( str ) );
        //        }

        //    return 0;
        //    }

    protected:
        dev_stub stub;  ///< Устройство-заглушка, фиктивное устройство.

        struct range    ///< Диапазон устройств одного типа.
            {
            int start_pos;
            int end_pos;
            };

        /// Диапазоны устройств всех типов.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        int get_device_n( device::DEVICE_TYPE dev_type,
            const char *dev_name );

        std::vector< device* > project_devices; ///< Все устройства.

        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < device_manager > instance;

    public:
        // @brief Добавление устройства.
        //
        // Вызывается из Lua.
        wago_device* add_wago_device( int dev_type, int dev_sub_type,
            const char *dev_name, char * descr );
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

        /// @brief Количество таймеров.
        int get_count() const
            {
            return timers_cnt;
            }

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
i_DO_device* V( u_int dev_n );
i_DO_device* V( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение управляемого клапана по номеру.
///
/// @param number - номер клапана.
/// @return - клапан с заданным номером. Если нет такого клапана, возвращается
/// заглушка (@ref dev_stub).
i_AO_device* VC( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение двигателя по номеру.
///
/// @param number - номер двигателя.
/// @return - двигатель с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_AO_device* M( u_int dev_n );
i_DO_AO_device* M( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального уровня по номеру.
///
/// @param number - номер сигнального уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* LS( u_int dev_n );
i_DI_device* LS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального расхода по номеру.
///
/// @param number - номер сигнального расхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* FS( u_int dev_n );
i_DI_device* FS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового входа по номеру.
///
/// @param number - номер аналогового входа.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* AI( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового выхода по номеру.
///
/// @param number - номер аналогового выхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AO_device* AO( u_int dev_n );
i_AO_device* AO( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение счетчика по номеру.
///
/// @param number - номер счетчика.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_counter* FQT( u_int dev_n );
i_counter* FQT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение температуры по номеру.
///
/// @param number - номер температуры.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* TE( u_int dev_n );
i_AI_device* TE( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение текущего уровня по номеру.
///
/// @param number - номер текущего уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* LT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение датчика положения по номеру.
///
/// @param number - номер датчика положения.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* GS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение звуковой сигнализации по номеру.
///
/// @param number - номер.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* HA( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение световой сигнализации по номеру.
///
/// @param number - номер.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* HL( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение кнопки по номеру.
///
/// @param number - номер.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* SB( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение обратной связи по номеру.
///
/// @param number - номер обратной связи.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* DI( u_int dev_n );
i_DI_device* DI( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение канала управления по номеру.
///
/// @param number - номер канала управления.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* DO( u_int dev_n);
i_DO_device* DO( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение текущей концентрации по номеру.
///
/// @param number - номер текущей концентрации.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* QT( u_int dev_n );
i_AI_device* QT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение виртуального устройства.
///
/// @return - виртуальное устройство.
dev_stub* STUB();
//-----------------------------------------------------------------------------
/// @brief Получение устройства по номеру и типу.
///
/// @param type   - тип устройства.
/// @param s_number - порядковый номер устройства.
/// @return - устройство.
device* DEVICE( int s_number );
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
