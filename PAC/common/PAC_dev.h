/// @file PAC_dev.h
/// @brief Классы, которые реализуют функции передачи
/// состояния устройств PAC на сервер. Также реализованы классы для работы с
/// устройствами ( клапана, насосы,... ). Классы используются ТОЛЬКО в
/// контроллере ( PAC ).
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG_NO_IO_MODULES - простые устройства работают без модулей
/// ввода\вывода (состояния хранят в себе).
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
#include <algorithm>
#include <memory>

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "smart_ptr.h"

#include "dtime.h"
#include "bus_coupler_io.h"
#include "g_device.h"

#include "param_ex.h"

#include "PAC_info.h"

#include "iot_altivar.h"
#include "iot_wages_eth.h"
#include "log.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

class PID;
//-----------------------------------------------------------------------------
/// @brief Устройство c параметрами.
///
/// Параметры хранятся в энергонезависимой памяти (сохраняют значение после
/// перезагрузки PAC). Доступ к параметрам производится на основе номера и
/// смещения (итоговый индекс равен их сумме). Каждый параметр имеет имя.
class par_device
    {
    friend class device;
    friend class PID;

    public:
        /// @param par_cnt - количество параметров.
        par_device( u_int par_cnt );

        virtual ~par_device();

        /// @brief Сохранение в виде скрипта Lua.
        ///
        /// Сохраняем на основе названий параметров в виде таблицы:
        /// имя_параметра = 'значение'.
        ///
        /// @param str - строка, куда сохраняем.
        int save_device( char *str );

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
        /// @param idx - индекс параметра (с единицы).
        /// @param offset - смещение индекса.
        /// @param value - новое значение.
        virtual void set_par( u_int idx, u_int offset, float value );

        /// @brief Установка значения рабочего параметра.
        ///
        /// @param idx - индекс рабочего параметра (с единицы).
        /// @param value - новое значение.
        virtual void set_rt_par( u_int idx, float value )
            {
            }

        /// @brief Получение значения параметра.
        ///
        /// @param idx - индекс параметра.
        /// @param offset - смещение индекса.
        ///
        /// @return значение параметра.
        float get_par( u_int idx, u_int offset = 0 ) const;

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
            C_MAX_PAR_NAME_LENGTH = 25, ///< Максимальная длина имени параметра.
            };

        saved_params_float *par; ///< Параметры.
        char **par_name;         ///< Названия параметров.
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        virtual ~i_counter();

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

        /// @brief Получение абсолютного значения счетчика (без учета
        /// состояния паузы).
        virtual u_int get_abs_quantity() = 0;

        /// @brief Сброс абсолютного значения счетчика.
        virtual void  abs_reset() = 0;

        enum class STATES
            {
            S_WORK = 1,
            S_PAUSE,

            S_ERROR = -10,

            S_LOW_ERR = -12,
            S_HI_ERR = -13,
            };

    protected:
        /// @brief Получение времени ожидания работы насоса.
        virtual u_long get_pump_dt() const = 0;

        /// @brief Получение минимального расхода для учета работы связанных
        /// насосов.
        virtual float get_min_flow() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс противосмешивающего клапана (mixproof).
class i_mix_proof
    {
    public:

        /// @brief Открыть верхнее седло.
        virtual void open_upper_seat() = 0;

        /// @brief Открыть нижнее седло.
        virtual void open_lower_seat() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс весов.
class i_wages
    {
    public:
        /// @brief Тарировка.
        virtual void tare() = 0;
        ///@brief Возвращает вес в килограммах
        virtual float get_value() = 0;
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

        /// @brief немедленное выключение устройства
        virtual void direct_off() = 0;

        /// @brief немедленное выключение устройства c учетом ручного режима
        virtual void instant_off();

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

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() = 0;
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

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() = 0;
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

        /// @brief Расчет состояния на основе текущих данных от I/O.
        virtual void evaluate_io()
            {
            }

        enum CONSTANTS
            {
            C_MAX_NAME = 20
            };

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
            DT_WT,      ///< Тензорезистор.
            DT_PT,      ///< Давление (значение).
            DT_F,       ///< Автоматический выключатель.
            DT_REGULATOR, ///< ПИД-регулятор.
            DT_HLA,      ///< Сигнальная колонна.
            DT_CAM,      ///< Камера.
            DT_PDS,      ///< Датчик разности давления.
            DT_TS,       ///< Сигнальный датчик температуры. 

            C_DEVICE_TYPE_CNT, ///< Количество типов устройств.
            };

        static const char* const DEV_NAMES[ device::DEVICE_TYPE::C_DEVICE_TYPE_CNT ];

        /// Подтипы устройств.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = -1,      ///< Подтип не определен.

            //V
            DST_V_DO1 = 1,         ///< Клапан с одним каналом управления.
            DST_V_DO2,             ///< Клапан с двумя каналами управления.
            DST_V_DO1_DI1_FB_OFF,  ///< Клапан с одним каналом управления и одной обратной связью на закрытое состояние.
            DST_V_DO1_DI1_FB_ON,   ///< Клапан с одним каналом управления и одной обратной связью на открытое состояние.
            DST_V_DO1_DI2,         ///< Клапан с одним каналом управления и двумя обратными связями.
            DST_V_DO2_DI2,         ///< Клапан с двумя каналами управления и двумя обратными связями.
            DST_V_MIXPROOF,        ///< Клапан противосмешивающий.
            DST_V_AS_MIXPROOF,     ///< Клапан с двумя каналами управления и двумя обратными связями с AS интерфейсом (противосмешивающий).
            DST_V_BOTTOM_MIXPROOF, ///< Донный клапан.
            DST_V_AS_DO1_DI2,      ///< Клапан с одним каналом управления и двумя обратными связями с AS интерфейсом.
            V_DO2_DI2_BISTABLE,    ///< Клапан с двумя каналами управления и двумя обратными связями бистабильный.

            V_IOLINK_VTUG_DO1,     ///< IO-Link VTUG8 клапан с одним каналом управления.

            V_IOLINK_VTUG_DO1_FB_OFF, ///< IO-Link VTUG клапан с одним каналом управления и одной обратной связью (выключенное состояние).
            V_IOLINK_VTUG_DO1_FB_ON,  ///< IO-Link VTUG клапан с одним каналом управления и одной обратной связью (включенное состояние).

            V_IOLINK_MIXPROOF,        ///< Клапан с двумя каналами управления и двумя обратными связями с IO-Link интерфейсом (противосмешивающий).
            V_IOLINK_DO1_DI2,         ///< Клапан с одним каналом управления и двумя обратными связями с IO-Link интерфейсом (отсечной).
            V_IOLINK_VTUG_DO1_DI2,    ///< IO-Link VTUG клапан с одним каналом управления и двумя обратными связями.

            DST_V_VIRT,               ///< Виртуальный клапан.

            //VC
            DST_VC = 1,         ///< Клапан с аналоговым управлением.
            DST_VC_IOLINK,      ///< Клапан IO-LInk.

            DST_VC_VIRT,        ///< Виртуальный клапан.

            //LS
            DST_LS_MIN = 1,     ///< Подключение по схеме минимум.
            DST_LS_MAX,         ///< Подключение по схеме максимум.

            LS_IOLINK_MIN,      ///< IOLInk уровень. Подключение по схеме минимум.
            LS_IOLINK_MAX,      ///< IOLInk уровень. Подключение по схеме максимум.

            DST_LS_VIRT,        ///< Виртуальный датчик уровня.

            //TE
            DST_TE = 1,
            DST_TE_IOLINK,      ///< Температура IO-LInk без дополнительных параметров.
            DST_TE_VIRT,        ///< Виртуальная температура.
            DST_TE_ANALOG,      ///< Температура AI c заданием пределов значения.

            //M,
            DST_M = 1,          ///< Мотор без управления частотой вращения.
            DST_M_FREQ,         ///< Мотор с управлением частотой вращения.

            DST_M_REV,          ///< Мотор с реверсом без управления частотой вращения. Реверс включается совместно.
            DST_M_REV_FREQ,     ///< Мотор с реверсом с управлением частотой вращения. Реверс включается совместно.
            DST_M_REV_2,        ///< Мотор с реверсом без управления частотой вращения. Реверс включается отдельно.
            DST_M_REV_FREQ_2,   ///< Мотор с реверсом с управлением частотой вращения. Реверс включается отдельно.


            /// Мотор с реверсом. Реверс включается отдельно. Отдельный сигнал
            /// ошибки.
            M_REV_2_ERROR,

            /// Мотор с реверсом с управлением частотой вращения. Реверс
            /// включается отдельно. Отдельный сигнал ошибки.
            DST_M_REV_FREQ_2_ERROR,

            /// Мотор, управляемый частотником Altivar. Связь с частотником по Ethernet.
            /// Реверс и аварии опциональны.
            M_ATV,

            DST_M_VIRT,        ///< Виртуальный мотор.

            /// Мотор, управляемый частотником Altivar. Связь с частотником по Ethernet.
            /// Реверс и аварии опциональны. Расчёт линейной скорости.
            M_ATV_LINEAR,

            //FQT
            DST_FQT = 1,   ///< Счетчик.
            DST_FQT_F,     ///< Счетчик + расход.
            DST_FQT_F_OK,  ///< Счетчик + расход c диагностикой.
            DST_FQT_VIRT,  ///Виртуальный cчетчик (без привязки к модулям).
            DST_FQT_IOLINK,/// Счетчик IO-Link.

            //QT
            DST_QT = 1,   ///< Концентратомер.
            DST_QT_OK,    ///< Концентратомер c диагностикой.
            DST_QT_IOLINK,///Концентратомер IOLInk без дополнительных параметров.

            DST_QT_VIRT,  ///< Виртуальный концентратомер.

            //LT
            DST_LT = 1,    ///Текущий уровень без дополнительных параметров.
            DST_LT_CYL,    ///Текущий уровень для цилиндрического танка.
            DST_LT_CONE,   ///Текущий уровень для танка с конусом в основании.
            DST_LT_TRUNC,  ///Текущий уровень для танка с усеченным цилиндром в основании.

            DST_LT_IOLINK, ///Текущий IOLInk уровень без дополнительных параметров.

            DST_LT_VIRT,   ///< Виртуальный текущий уровень.

            //DO
            DST_DO = 1,    ///Обычный дискретный выход с привязкой к модулям
            DST_DO_VIRT,   ///Виртуальный дискретный выход(без привязки к модулям)

            //DI
            DST_DI = 1,    ///Обычный дискретный вход с привязкой к модулям
            DST_DI_VIRT,   ///Виртуальный дискретный вход(без привязки к модулям)

            //AO
            DST_AO = 1,    ///Обычный аналоговый выход с привязкой к модулям
            DST_AO_VIRT,   ///Виртуальный аналоговый выход(без привязки к модулям)

            //AI
            DST_AI = 1,    ///Обычный аналоговый вход с привязкой к модулям
            DST_AI_VIRT,   ///Виртуальный аналоговый вход(без привязки к модулям)

            //PT
            DST_PT = 1,      ///Обычный аналоговый датчик давления
            DST_PT_IOLINK,   ///Датчик давления IOLInk
            DST_PT_VIRT,     ///Виртуальный датчик давления

            //F
            DST_F = 1,       ///< Автоматический выключатель.
            DST_F_VIRT = 4,  ///< Виртуальный автоматический выключатель.

            //HLA
            DST_HLA = 1,  ///< Сигнальная колонна (красный, желтый, зеленый и сирена).
            DST_HLA_VIRT, ///< Виртуальная сигнальная колонна (без привязки к модулям).
            DST_HLA_IOLINK, ///< Сигнальная колонна IO-Link.

            //GS
            DST_GS = 1,  ///< Датчик положения.
            DST_GS_VIRT, ///< Виртуальный датчик положения (без привязки к модулям).

            //HA
            DST_HA = 1,  ///< Сирена.
            DST_HA_VIRT, ///< Виртуальная сирена (без привязки к модулям).

            //HL
            DST_HL = 1,  ///< Лампа.
            DST_HL_VIRT, ///< Виртуальная лампа (без привязки к модулям).

            //FS
            DST_FS = 1,  ///< Датчик наличия расхода.
            DST_FS_VIRT, ///< Виртуальный датчик наличия расхода (без привязки к модулям).

            //SB
            DST_SB = 1,  ///< Кнопка.
            DST_SB_VIRT, ///< Виртуальная кнопка (без привязки к модулям).

            //WT
            DST_WT = 1,  ///< Весы.
            DST_WT_VIRT, ///< Виртуальные весы.
            DST_WT_RS232,///< Весы c RS232 интерфейсом.
            DST_WT_ETH,  ///< Весы c интерфейсом ethernet.

            //CAM
            DST_CAM_DO1_DI2 = 1,///< C сигналом активации, результатом обработки и готовностью.
            DST_CAM_DO1_DI1,    ///< C сигналом активации и результатом обработки.
            DST_CAM_DO1_DI3,    ///< C сигналом активации, двумя результатами обработки и готовностью.

            //PDS
            DST_PDS = 1,
            DST_PDS_VIRT,

            //TS
            DST_TS = 1,
            DST_TS_VIRT,

            //DT_REGULATOR
            DST_REGULATOR_PID = 1,
            DST_REGULATOR_THLD,
            };

        device( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~device();

        const char* get_type_str() const;

        const char* get_type_name() const;

        const char *get_name() const
            {
            return name;
            }

        const char *get_description() const
            {
            return description;
            }

        virtual const char* get_error_description() const
            {
            return "обратная связь";
            }

        void set_descr( const char *new_description );

        virtual void set_article( const char* new_article );

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

        /// @brief Установка дополнительных свойств, значения которых -
        /// строки.
        ///
        /// Для использования в Lua.
        virtual void set_string_property( const char* field, const char* value );

    protected:
        /// @brief Сохранение дополнительных данных устройства в виде скрипта Lua.
        ///
        /// @param buff [out] - буфер записи строки.
        virtual int save_device_ex( char *buff )
            {
            buff[ 0 ] = 0;
            return 0;
            }

        u_int_4 s_number;            ///< Последовательный номер устройства.

        DEVICE_TYPE     type;        ///< Тип устройства.
        DEVICE_SUB_TYPE sub_type;    ///< Подтип устройства.

        bool get_manual_mode() const
            {
            return is_manual_mode;
            }

        char * article;           ///< Артикул изделия.

    private:
        bool is_manual_mode;      ///< Признак ручного режима.

        char name[ C_MAX_NAME ];
        char *description;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый класс для различных дискретных устройств.
class digital_io_device : public device,
    public io_device
    {
    public:
        digital_io_device( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~digital_io_device();

        float   get_value();
        void    direct_set_value( float new_value );
        void    direct_set_state( int new_state );

#ifdef DEBUG_NO_IO_MODULES
        /// @brief Получение состояния объекта.
        ///
        /// @return - состояние объекта.
        int  get_state();

        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        virtual void print() const;

#ifdef DEBUG_NO_IO_MODULES
    protected:
        int state;  ///< Состояние устройства.
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговыми входами/выходами.
///
/// Базовый класс для различных аналоговых устройств.
class analog_io_device : public device, public io_device
    {
    public:
        analog_io_device( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt ):
        device( dev_name, type, sub_type, par_cnt ),
            io_device( dev_name )
            {
            }

        void  direct_set_state( int new_state );
        int   get_state();

        virtual void  print() const;
        void  direct_on();
        void  direct_off();

#ifdef DEBUG_NO_IO_MODULES
        float get_value();
        void  direct_set_value( float new_value );

#else  // DEBUG_NO_IO_MODULES

        float get_value() = 0;

#endif // DEBUG_NO_IO_MODULES


#ifdef DEBUG_NO_IO_MODULES
    private:
        float value = .0f;    ///< Состояние устройства.
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом.
///
/// Это может быть клапан, насос, канал управления...
class DO1 : public digital_io_device
    {
    public:
        DO1( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type ):
        digital_io_device( dev_name, type, sub_type, 0 )
            {
            }

#ifndef DEBUG_NO_IO_MODULES
    public:
        int  get_state();
        void direct_on();
        void direct_off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами.
///
/// Это может быть клапан, насос...
class DO2 : public digital_io_device
    {
    public:
        DO2( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ):
        digital_io_device( dev_name, type, sub_type, par_cnt )
            {
            }

#ifndef DEBUG_NO_IO_MODULES
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
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
class valve_DO2_DI2_bistable;

/// @brief Устройство с обратными связями.
///
class valve: public digital_io_device
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

        /// @brief Конструктор для клапана без обратных связей.
        /// @param number - номер устройства.
        /// @param type - тип устройства.
        /// @param sub_type - подтип устройства.
        valve( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type );

        valve( bool is_on_fb, bool is_off_fb, const char *dev_name,
            device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type,
            int extra_params_cnt );

#ifdef _MSC_VER
#pragma region Отключение клапана с задержкой.
#endif
        void off();

        void on();

        /// @brief Выключение клапанов с задержкой.
        static void evaluate();
#ifdef _MSC_VER
#pragma endregion Отключение клапана с задержкой.
#endif

        /// @brief Получение значения включенного состояния.
        bool is_opened()
            {
            return get_on_fb_value() > 0;
            }

        /// @brief Получение значения выключенного состояния.
        bool is_closed()
            {
            return get_off_fb_value() > 0;
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
        int save_device_ex( char *buff );

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

            VX_OPENING = 40,
            VX_CLOSING,
            VX_OPENING_MANUAL,
            VX_CLOSING_MANUAL,

            VX_UNKNOWN = 100,            ///< Неизвестное состояние.
            };

        /// @brief Получение расширенного состояния клапана (учет обратной
        /// связи, ручного режима, ...).
        int get_state();

#ifdef DEBUG_NO_IO_MODULES
        int set_cmd( const char *prop, u_int idx, double val );
#endif // DEBUG_NO_IO_MODULES

        ///Состояние клапана без учета обратной связи.
        enum VALVE_STATE
            {
            V_LOWER_SEAT = 3, ///< Открыто нижнее седло.
            V_UPPER_SEAT = 2, ///< Открыто верхнее седло.

            V_ON  = 1,        ///< Включен.
            V_OFF = 0,        ///< Выключен.

            V_STOP = 5,       ///< Остановлен.
            };

        bool is_wash_seat_active() const
            {
            return wash_flag;
            }
        void set_seat_wash_state( bool wash_flag )
            {
            this->wash_flag = wash_flag;
            }
        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:

        /// @brief Получение состояния клапана без учета обратной связи.
        virtual VALVE_STATE get_valve_state() = 0;

        /// @brief Получение состояния обратной связи.
        virtual bool get_fb_state()
            {
            return true;
            }

        enum FB_STATE
            {
            FB_IS_AND_OFF = 0, ///< Обратная связь отключена.
            FB_IS_AND_ON,      ///< Обратная связь включена.
            };

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 2,///Количество дополнительных параметров.

            P_ON_TIME = 1,
            P_FB,
            };

    private:
        /// @brief Есть обратная связь на включенное состояние.
        bool is_on_fb;

        /// @brief Есть обратная связь на выключенное состояние.
        bool is_off_fb;

        bool on_fb;
        bool off_fb;
#ifdef _MSC_VER
#pragma region Отключение клапана с задержкой.
#endif
        /// @brief Вектор клапанов, ожидающих отключение.
        static std::vector< valve* > to_switch_off;

    protected:
        /// @brief Вектор бистабильных клапанов.
        static std::vector< valve_DO2_DI2_bistable* > v_bistable;

    private:
        /// @brief Определение завершения отключения клапана с задержкой.
        static bool is_switching_off_finished( valve *v )
            {
            return !v->is_switching_off;
            };

        bool is_switching_off; ///Выключается ли клапан с задержкой.
        u_long start_off_time; ///Время начала выключения клапана с задержкой.
        bool was_on_auto;      ///Признак включения клапана управляющей программой.
#ifdef _MSC_VER
#pragma endregion Отключение клапана с задержкой.
#endif

    protected:
        u_long start_switch_time;

    private:
        bool wash_flag;
    };
//-----------------------------------------------------------------------------
class virtual_valve: public valve
    {
    public:
        virtual_valve( const char* dev_name );

    protected:
        float value;
        int state;

    public:
        VALVE_STATE get_valve_state();

        virtual void direct_off();

        virtual void direct_set_value( float new_value );

        virtual float get_value();

        virtual void direct_set_state( int new_state );

        virtual void direct_on();

        virtual int get_state();
    };
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

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_IO_MODULES

        // Интерфейс valve для реализации получения расширенного состояния с
        // учетом всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
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
#endif // DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX );
            }

        int get_on_fb_value()
            {
            return false;
            }
#endif // DEBUG_NO_IO_MODULES
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

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_IO_MODULES
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

#ifndef DEBUG_NO_IO_MODULES
        int get_on_fb_value()
            {
            return get_DI( DI_INDEX );
            }

        int get_off_fb_value()
            {
            return false;
            }
#endif // DEBUG_NO_IO_MODULES
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

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
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
#endif // DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_2 );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_1 );
            }
#endif // DEBUG_NO_IO_MODULES
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

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_IO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            int o1 = get_DO( DO_INDEX_2 );

            return ( VALVE_STATE ) o1;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
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
#endif // DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_1 );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_2 );
            }
#endif // DEBUG_NO_IO_MODULES

    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями
/// бистабильный.
///
class valve_DO2_DI2_bistable : public valve
    {
    public:
        valve_DO2_DI2_bistable( const char *dev_name ) :
            valve( true, true, dev_name, DT_V, V_DO2_DI2_BISTABLE ),
            is_stoped( false ),
            is_opening( false ),
            is_closing( false )
            {
            v_bistable.push_back( this );
            }

        int evaluate()
            {
            int o = get_DI( DI_INDEX_OPEN );
            int c = get_DI( DI_INDEX_CLOSE );

            unsigned int t = (unsigned int) get_par( valve::P_ON_TIME, 0 );
            if ( get_DO( DO_INDEX_OPEN ) == 1 )
                {
                if ( o == 1 ||
                    get_delta_millisec( start_switch_time ) > t )
                    {
                    set_DO( DO_INDEX_OPEN, 0 );
                    }
                }
            if ( get_DO( DO_INDEX_CLOSE ) == 1 )
                {
                if ( c == 1 ||
                    get_delta_millisec( start_switch_time ) > t )
                    {
                    set_DO( DO_INDEX_CLOSE, 0 );
                    }
                }

            return 0;
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX_OPEN = 0,  ///< Индекс канала дискретного выхода Открыть.
            DO_INDEX_CLOSE,     ///< Индекс канала дискретного выхода Закрыть.

            DI_INDEX_OPEN = 0,  ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE,     ///< Индекс канала дискретного входа Закрыт.
            };

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on()
            {
            int o = get_DI( DI_INDEX_OPEN );

            if ( 0 == o && get_DO( DO_INDEX_OPEN ) == 0 )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_OPEN, 1 );
                set_DO( DO_INDEX_CLOSE, 0 );

                is_opening = true;
                is_closing = false;
                }
            }

        void direct_off()
            {
            int c = get_DI( DI_INDEX_CLOSE );
            if ( 0 == c && get_DO( DO_INDEX_CLOSE ) == 0 )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_OPEN, 0 );
                set_DO( DO_INDEX_CLOSE, 1 );

                is_opening = false;
                is_closing = true;
                }
            }

#endif // DEBUG_NO_IO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return (VALVE_STATE)digital_io_device::get_state();
#else
            int o = get_DI( DI_INDEX_OPEN );
            int c = get_DI( DI_INDEX_CLOSE );

            int o_do = get_DO( DO_INDEX_OPEN );
            int c_do = get_DO( DO_INDEX_CLOSE );

            if ( o == 1 ) return V_ON;
            if ( c == 1 ) return V_OFF;

            if ( o_do == 1 ) return V_ON;
            if ( c_do == 1 ) return V_OFF;

            if ( is_stoped ) return V_STOP;

            return V_OFF;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return true;
#else
            if ( is_stoped ) return true;

            int i0 = get_DI( DI_INDEX_OPEN );
            int i1 = get_DI( DI_INDEX_CLOSE );
            unsigned int t = (unsigned int) get_par( valve::P_ON_TIME, 0 );
            unsigned int dt = get_delta_millisec( start_switch_time );

            if ( i0 == 1 && i1 == 1 )
                {
                return false;
                }
            //Заклинило в закрытом положении.
            if ( is_opening && dt > t && i1 == 1 ) return false;

            //Заклинило в открытом положении.
            if ( is_closing && dt > t && i0 == 1 ) return false;

            if ( i0 == 1 || i1 == 1 ) return true;

            if ( dt < t ) return true;

            return false;
#endif // DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_CLOSE );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_OPEN );
            }
#endif // DEBUG_NO_IO_MODULES

        void direct_set_state( int new_state )
            {
#ifdef DEBUG_NO_IO_MODULES
            valve::direct_set_state( new_state );
#else
            int i0 = get_DI( DI_INDEX_OPEN );
            int i1 = get_DI( DI_INDEX_CLOSE );
            switch ( new_state )
                {
                case V_STOP:
                    is_opening = false;
                    is_closing = false;

                    //Если клапан полностью открыт\закрыт ничего не делаем.
                    if ( i0 == 1 || i1 == 1 )
                        {
                        return;
                        }

                    is_stoped = true;
                    if ( get_DO( DO_INDEX_OPEN ) == 1 )
                        {
                        set_DO( DO_INDEX_OPEN, 0 );
                        }

                    if ( get_DO( DO_INDEX_CLOSE ) == 1 )
                        {
                        set_DO( DO_INDEX_CLOSE, 0 );
                        }
                    break;

                default:
                    is_stoped = false;
                    valve::direct_set_state( new_state );
                    break;
                }
#endif //DEBUG_NO_IO_MODULES
            }

    private:
        bool is_stoped;
        bool is_opening;
        bool is_closing;
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

#ifndef DEBUG_NO_IO_MODULES
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        // всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return V_UPPER_SEAT;
            if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return V_LOWER_SEAT;

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
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
#endif // DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_CLOSE );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_OPEN );
            }
#endif // DEBUG_NO_IO_MODULES

    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Клапан AS-interface.
class valve_AS : public valve
    {
    public:
        valve_AS( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

        void print() const
            {
            printf( "%s [%u]\t", get_name(), AS_number );
            }

        void set_rt_par( u_int idx, float value )
            {
            switch ( idx )
                {
                case 1:
                    AS_number = ( u_int ) value;
                    break;

                default:
                    valve::set_rt_par( idx, value );
                    break;
                }
            }

        /// @brief Получение данных состояния устройства.
        char get_state_data( char* data )
            {
            if ( data == 0 )
                {
                return 0;
                }

            char state = 0;
            u_int offset = 0;

            if ( AS_number < 32 )		 //Eсли номер < 32).
                {
                offset = MAILBOX_OFFSET + AS_number / 2;
                state = data[ offset ];
                if ( AS_number % 2 == 0 )//Четный номер - старшие четыре бита.
                    {
                    state >>= 4;
                    }
                }
            else
                {
                u_int new_n = AS_number - 32;
                offset = MAILBOX_OFFSET + ( 32 / 2 ) + new_n / 2 + new_n % 2;
                state = data[ offset ];
                if ( AS_number % 2 == 1 )//Нечетный номер - старшие четыре бита.
                    {
                    state >>= 4;
                    }
                }

            return state;
            }

        char* get_data_with_offset( char* data )
            {
            if ( data == 0 )
                {
                return 0;
                }

            char* state;
            u_int offset = 0;

            if ( AS_number < 32 )		 //Eсли номер < 32).
                {
                offset = MAILBOX_OFFSET + AS_number / 2;
                }
            else
                {
                u_int new_n = AS_number - 32;
                offset = MAILBOX_OFFSET + ( 32 / 2 ) + new_n / 2 + new_n % 2;
                }

            state = data + offset;
            return state;
            }

        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            char* data = ( char* ) get_AO_read_data( AO_INDEX );
            char state = get_state_data( data );

            int o = ( state & C_OPEN_S1 ) > 0 ? 1 : 0;
            int l = ( state & C_OPEN_S2 ) > 0 ? 1 : 0;
            int u = ( state & C_OPEN_S3 ) > 0 ? 1 : 0;

            if ( o == 0 && u == 1 ) return V_UPPER_SEAT;
            if ( o == 0 && l == 1 ) return V_LOWER_SEAT;

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return true;
#else
            char* AO_data = ( char* ) get_AO_read_data( AO_INDEX );
            char AO_state = get_state_data( AO_data );

            int o = ( AO_state & C_OPEN_S1 ) > 0 ? 1 : 0;
            int l = ( AO_state & C_OPEN_S2 ) > 0 ? 1 : 0;
            int u = ( AO_state & C_OPEN_S3 ) > 0 ? 1 : 0;

            char* AI_data = ( char* ) get_AI_data( AI_INDEX );
            char AI_state = get_state_data( AI_data );

            int i0 = ( AI_state & S_CLOSED ) > 0 ? 1 : 0;
            int i1 = ( AI_state & S_OPENED ) > 0 ? 1 : 0;

            if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
                ( o == 1 && i1 == 1 && i0 == 0 ) )
                {
                return true;
                }

            if ( o == 0 && l == 1 ) return true;
            if ( o == 0 && u == 1 ) return true;

            if ( get_delta_millisec( start_switch_time ) <
                get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            char* AI_data = ( char* ) get_AI_data( AI_INDEX );
            char AI_state = get_state_data( AI_data );

            int i0 = AI_state & S_CLOSED;

            return i0 > 0 ? 1 : 0;
            }

        int get_on_fb_value()
            {
            char* AI_data = ( char* ) get_AI_data( AI_INDEX );
            char AI_state = get_state_data( AI_data );

            int i1 = AI_state & S_OPENED;

            return i1 > 0 ? 1 : 0;
            }

        void direct_off()
            {
            char* data = ( char* ) get_AO_write_data( AO_INDEX );
            char* write_state = get_data_with_offset( data );
            char read_state = get_state_data( data );

            int o = ( read_state & C_OPEN_S1 ) > 0 ? 1 : 0;

            if ( 1 == o )
                {
                start_switch_time = get_millisec();
                }

            int offset = 0;
            //Для первого 31-го устройства четный номер - старшие четыре
            //бита (1), для остальных устройств нечетный номер - старшие четыре
            //бита (2).
            if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    			//1
                    ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
                {
                offset = 4;
                }

            //Сбрасываем в ноль все четыре нужные бита.
            *write_state &= 0xF0 >> offset;
            }

        void direct_on()
            {
            char* data = ( char* ) get_AO_write_data( AO_INDEX );
            char* write_state = get_data_with_offset( data );
            char read_state = get_state_data( data );

            int o = ( read_state & C_OPEN_S1 ) > 0 ? 1 : 0;
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                }

            int offset = 0;
            //Для первого 31-го устройства четный номер - старшие четыре
            //бита (1), для остальных устройств нечетный номер - старшие четыре
            //бита (2).
            if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    			//1
                    ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
                {
                offset = 4;
                }
            *write_state |= C_OPEN_S1 << offset;
            *write_state &= ~( C_OPEN_S3 << offset );
            *write_state &= ~( C_OPEN_S2 << offset );

            //            if ( strcmp( get_name(), "H1V1" ) == 0 )
            //                {
            //                printf( "AO_INDEX = %d\n", AO_INDEX );
            //                printf( "AS_number = %d\n", AS_number);
            //
            //                printf( "*write_state = %d\n", ( int ) *write_state );
            //                }
            }
#endif // DEBUG_NO_IO_MODULES

        void direct_set_state( int new_state )
            {
#ifdef DEBUG_NO_IO_MODULES
            state = ( char ) new_state;
#else
            int offset = 0;
            //Для первого 31-го устройства четный номер - старшие четыре
            //бита (1), для остальных устройств нечетный номер - старшие четыре
            //бита (2).
            if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    			//1
                    ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
                {
                offset = 4;
                }

            switch ( new_state )
                {
                case V_OFF:
                    direct_off();
                    break;

                case V_ON:
                    direct_on();
                    break;

                case V_UPPER_SEAT:
                    {
                    direct_off();

                    char* data = ( char* ) get_AO_write_data( AO_INDEX );
                    char* write_state = get_data_with_offset( data );
                    char read_state = get_state_data( data );

                    int u = ( read_state & C_OPEN_S3 ) > 0 ? 1 : 0;
                    if ( 0 == u )
                        {
                        start_switch_time = get_millisec();
                        }
                    *write_state |= C_OPEN_S3 << offset;

                    break;
                    }

                case V_LOWER_SEAT:
                    {
                    direct_off();

                    char* data = ( char* ) get_AO_write_data( AO_INDEX );
                    char* write_state = get_data_with_offset( data );
                    char read_state = get_state_data( data );

                    int l = ( read_state & C_OPEN_S2 ) > 0 ? 1 : 0;
                    if ( 0 == l )
                        {
                        start_switch_time = get_millisec();
                        }
                    *write_state |= C_OPEN_S2 << offset;

                    break;
                    }

                default:
                    direct_on();
                    break;
                }
#endif //DEBUG_NO_IO_MODULES
            }

    protected:
        u_int AS_number;    ///< AS-номер устройства.

        enum CONSTANTS
            {
            AI_INDEX = 0,   ///< Индекс канала аналогового входа.
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.

            C_OPEN_S1 = 0x2,    ///< Открыть соленоид 1.
            C_OPEN_S2 = 0x4,    ///< Открыть соленоид 2.
            C_OPEN_S3 = 0x8,    ///< Открыть соленоид 3.

            S_CLOSED = 0x1,     ///< Клапан закрыт.
            S_OPENED = 0x2,     ///< Клапан открыт.

            MAILBOX_OFFSET = 8
            };
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-mixproof.
class valve_AS_mix_proof : public i_mix_proof,  public valve_AS
    {
    public:
        valve_AS_mix_proof( const char *dev_name );

        void open_upper_seat()
            {
            direct_set_state( V_UPPER_SEAT );
            }

        void open_lower_seat()
            {
            direct_set_state( V_LOWER_SEAT );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-i отсечной.
class valve_AS_DO1_DI2 : public valve_AS
    {
    public:
        valve_AS_DO1_DI2( const char *dev_name );

        void direct_set_state( int new_state );

        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return (VALVE_STATE) digital_io_device::get_state();
#else
            char* data = (char*) get_AO_read_data( AO_INDEX );
            char state = get_state_data( data );

            int o = ( state & C_OPEN_S1 ) > 0 ? 1 : 0;

            return (VALVE_STATE) o;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return true;
#else
            char* AO_data = (char*)get_AO_read_data( AO_INDEX );
            char AO_state = get_state_data( AO_data );

            int o = ( AO_state & C_OPEN_S1 ) > 0 ? 1 : 0;

            char* AI_data = (char*)get_AI_data( AI_INDEX );
            char AI_state = get_state_data( AI_data );

            int i0 = ( AI_state & S_CLOSED ) > 0 ? 1 : 0;
            int i1 = ( AI_state & S_OPENED ) > 0 ? 1 : 0;

            if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
                ( o == 1 && i1 == 1 && i0 == 0 ) )
                {
                start_err_time = get_millisec();
                return true;
                }

             if ( get_delta_millisec( start_switch_time ) <
                get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

             if ( get_delta_millisec( start_err_time ) <
                 get_par(valve::P_ON_TIME, 0))
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_IO_MODULES
            }

    private:
        u_long start_err_time;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан донный.
class valve_bottom_mix_proof : public i_mix_proof,  public valve
    {
    public:
        valve_bottom_mix_proof( const char* dev_name ) : valve(
            true, true, dev_name, DT_V, DST_V_BOTTOM_MIXPROOF ),
            is_closing_mini( 0 ),
            start_off_time( 0 )
            {
            }

        /// @brief Открыть верхнее седло. Не делаем ничего, так как верхнего
        /// седла нет.
        void open_upper_seat()
            {
            }

        /// @brief Открыть нижнее седло.
        void open_lower_seat()
            {
            direct_set_state( V_LOWER_SEAT );
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_MINI_V,///< Индекс канала дискретного выхода мини клапана.
            DO_INDEX_L,     ///< Индекс канала дискретного выхода нижнего седла.

            DI_INDEX_OPEN = 0, ///< Индекс канала дискретного входа Открыт.
            DI_INDEX_CLOSE,     ///< Индекс канала дискретного входа Закрыт.
            };

        void direct_set_state( int new_state )
            {
#ifdef DEBUG_NO_IO_MODULES
            state = ( char ) new_state;
#else
            switch ( new_state )
                {
                case V_OFF:
                    direct_off();
                    break;

                case V_ON:
                    direct_on();
                    break;

                case V_UPPER_SEAT: //Открываем микроклапан
                    direct_off();
                    is_closing_mini = 0;

                    if (0 == get_DO(DO_INDEX_MINI_V))
                        {
                        start_switch_time = get_millisec();
                        set_DO( DO_INDEX_MINI_V, 1);
                        }
                    break;

                case V_LOWER_SEAT:
                    direct_off();

                    if ( 0 == get_DO( DO_INDEX_L ) )
                        {
                        start_switch_time = get_millisec();
                        set_DO( DO_INDEX_L, 1 );
                        }
                    break;


                default:
                    direct_on();
                    break;
                }
#endif //DEBUG_NO_IO_MODULES
            }

#ifndef DEBUG_NO_IO_MODULES
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //Интерфейс для реализации получения расширенного состояния с учетом
        //всех вариантов (ручной режим, обратная связь, ...).
    protected:
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            int o = get_DO( DO_INDEX );

            if (o == 0 && get_DO(DO_INDEX_MINI_V) == 1) return V_UPPER_SEAT;

            if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return V_LOWER_SEAT;

            return ( VALVE_STATE ) o;
#endif // DEBUG_NO_IO_MODULES
            }

        bool get_fb_state()
            {
#ifdef DEBUG_NO_IO_MODULES
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

            if ( get_delta_millisec( start_switch_time ) <
                get_par( valve::P_ON_TIME, 0 ) )
                {
                return true;
                }

            return false;
#endif // DEBUG_NO_WAGO_MODULE
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_off_fb_value()
            {
            return get_DI( DI_INDEX_CLOSE );
            }

        int get_on_fb_value()
            {
            return get_DI( DI_INDEX_OPEN );
            }
#endif // DEBUG_NO_IO_MODULES

#ifdef _MSC_VER
#pragma region Выключение мини клапана с задержкой.
#endif
        /// @brief Вектор клапанов, ожидающих выключение.
        static std::vector< valve_bottom_mix_proof* > to_switch_off;

        u_long start_off_time; //Время начала открытия клапана.

        int is_closing_mini; //Мини клапан в режиме закрытия

    public:
        /// @brief Определение завершения отключения клапана с задержкой.
        static bool is_switching_off_finished( valve_bottom_mix_proof *v );

        /// @brief Выключение мини клапанов с задержкой.
        static void evaluate();

#ifdef _MSC_VER
#pragma endregion Выключение мини клапана с задержкой.
#endif
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link mixproof.
class valve_iolink_mix_proof : public i_mix_proof,  public valve
    {
    public:
        valve_iolink_mix_proof( const char* dev_name );

        ~valve_iolink_mix_proof();

        void open_upper_seat();

        void open_lower_seat();

        VALVE_STATE get_valve_state();

        int save_device_ex( char *buff );

        void evaluate_io();

        void set_rt_par( u_int idx, float value );

#ifndef DEBUG_NO_IO_MODULES
        int get_state();

        float get_value();

        bool get_fb_state();

        int get_off_fb_value();

        int get_on_fb_value();

        void direct_on();

        void direct_off();

        int set_cmd( const char *prop, u_int idx, double val );

        void direct_set_state( int new_state );

#endif // DEBUG_NO_IO_MODULES

    private:
        struct in_data
            {
            int16_t  pos;
            uint16_t de_en  : 1; //De-Energized
            bool main       : 1; //Main energized position
            bool usl        : 1; //Upper Seat Lift energized position
            bool lsp        : 1; //Lower Seat Push energized position
            bool st         : 1; //Current Valve state
            uint16_t unused : 3;
            uint16_t err    : 5;
            };

        struct out_data_swapped   //Swapped low and high byte for easer processing
            {
            uint16_t unused1 : 4;
            bool sv1         : 1; //Main valve activation
            bool sv2         : 1; //Upper seat lift activation
            bool sv3         : 1; //Lower Seat Push energized position
            bool wink        : 1; //Visual indication
            uint16_t unused2 : 8;
            };

        in_data*  in_info = new in_data;
        static out_data_swapped stub_out_info;
        out_data_swapped* out_info = &stub_out_info;

        bool blink = false;     //Visual indication

        //Дополнительное смещение, так как область AO занимает 1 байт.
        int extra_offset = 0;

        enum class CONSTANTS
            {
            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link отсечной ALfaLaval.
class valve_iolink_shut_off_thinktop : public valve
    {
    public:
        valve_iolink_shut_off_thinktop( const char* dev_name );

        ~valve_iolink_shut_off_thinktop();

        VALVE_STATE get_valve_state();

        int save_device_ex( char* buff );

        void evaluate_io();

        void set_rt_par( u_int idx, float value );

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        bool get_fb_state();

        int get_off_fb_value();

        int get_on_fb_value();

        void direct_on();

        void direct_off();

        int set_cmd( const char* prop, u_int idx, double val );

        void direct_set_state( int new_state );

#endif // DEBUG_NO_IO_MODULES

    private:
        struct in_data
            {
            int16_t  pos;
            uint16_t de_en  : 1; //De-Energized
            bool main       : 1; //Main energized position
            bool usl        : 1; //Upper Seat Lift energized position
            bool lsp        : 1; //Lower Seat Push energized position
            bool st         : 1; //Current Valve state
            uint16_t unused : 3;
            uint16_t err    : 5;
            };

        struct out_data_swapped   //Swapped low and high byte for easer processing
            {
            uint16_t unused1 : 4;
            bool sv1         : 1; //Main valve activation
            bool sv2         : 1; //Upper seat lift activation
            bool sv3         : 1; //Lower Seat Push energized position
            bool wink        : 1; //Visual indication
            uint16_t unused2 : 8;
            };

        in_data* in_info = new in_data;
        static out_data_swapped stub_out_info;
        out_data_swapped* out_info = &stub_out_info;

        bool blink = false;     //Visual indication

        //Дополнительное смещение, так как область AO занимает 1 байт.
        int extra_offset = 0;

        enum class CONSTANTS
            {
            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-Link отсечной Definox.
class valve_iolink_shut_off_sorio : public valve
    {
    public:
        valve_iolink_shut_off_sorio( const char* dev_name );

        VALVE_STATE get_valve_state();

        int save_device_ex( char* buff );

        void evaluate_io();

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        bool get_fb_state();

        int get_off_fb_value();

        int get_on_fb_value();

        void direct_on();

        void direct_off();

        int set_cmd( const char* prop, u_int idx, double val );

        void direct_set_state( int new_state );

#endif // DEBUG_NO_IO_MODULES

    private:
        struct in_data
            {
            int16_t  pos;
            bool de_en       : 1; //De-Energized
            bool main        : 1; //Main energized position
            uint16_t unused1 : 2;
            uint16_t status  : 5;
            bool sv1         : 1; //Current state of solenoid 1
            uint16_t unused2 : 2;
            uint16_t err     : 5;
            };

        struct out_data_swapped   //Swapped low and high byte for easer processing
            {
            bool sv1         : 1; //Solenoid valve 1 activation
            uint16_t unused1 : 2;
            bool wink        : 1; //Visual indication
            bool ext1        : 1; //External RGB LED #1
            bool ext2        : 1; //External RGB LED #2
            bool ext3        : 1; //External RGB LED #3
            uint16_t unused2 : 1;
            };

        in_data* in_info = new in_data;
        out_data_swapped* out_info = 0;

        bool blink = false;     //Visual indication
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления.
class valve_iolink_vtug : public valve
    {
    public:
        valve_iolink_vtug( const char *dev_name,
            device::DEVICE_SUB_TYPE sub_type );

        valve_iolink_vtug( bool is_on_fb, bool is_off_fb, const char *dev_name,
            device::DEVICE_SUB_TYPE sub_type);

        void set_rt_par( u_int idx, float value );

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();

        void direct_off();

		int get_state() override;
#endif // DEBUG_NO_IO_MODULES


    protected:
        /// @brief Получение данных состояния устройства.
        char get_state_data( char* data );

        VALVE_STATE get_valve_state();

        /// @brief Получение состояния обратной связи.
        bool get_fb_state();

        u_int vtug_number;        ///< Номер устройства.
        u_int vtug_io_size = 1;   ///< Размер области, в словах.

        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления и обратной связью.
class valve_iolink_vtug_on : public valve_iolink_vtug
    {
    public:
        valve_iolink_vtug_on(const char* dev_name);

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,     ///< Индекс канала дискретного входа.
            };

    protected:
#ifndef DEBUG_NO_IO_MODULES
        /// @brief Получение состояния обратной связи.
        bool get_fb_state();

        int get_on_fb_value();

        int get_off_fb_value();
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления и обратной связью.
class valve_iolink_vtug_off : public valve_iolink_vtug
    {
    public:
        valve_iolink_vtug_off(const char* dev_name);

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,     ///< Индекс канала дискретного входа.
            };

    protected:
#ifndef DEBUG_NO_IO_MODULES
        /// @brief Получение состояния обратной связи.
        bool get_fb_state();

        int get_on_fb_value();

        int get_off_fb_value();
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан IO-link VTUG с одним каналом управления и 2-я обратными связями.
class valve_iolink_vtug_DO2 : public valve_iolink_vtug
    {
    public:
        valve_iolink_vtug_DO2( const char* dev_name );

    private:
        enum CONSTANTS
            {
            DI_INDEX_ON = 0,     ///< Индекс канала дискретного входа.
            DI_INDEX_OFF,
            };

    protected:
#ifndef DEBUG_NO_IO_MODULES
        /// @brief Получение состояния обратной связи.
        bool get_fb_state();

        int get_on_fb_value();

        int get_off_fb_value();
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI1 : public analog_io_device
    {
    public:
        AI1( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

#ifdef DEBUG_NO_IO_MODULES
        float get_value();

        void direct_set_state( int new_state )
            {
            st = new_state;
            }
#endif // DEBUG_NO_IO_MODULES

#ifndef DEBUG_NO_IO_MODULES
        int get_state()
            {
            if ( get_AI( C_AI_INDEX, 0, 0 ) == -1. )
                {
                return -2;
                }
            if ( get_AI( C_AI_INDEX, 0, 0 ) == -2. )
                {
                return -3;
                }
            return 1;
            }
#else
        int get_state()
            {
            return st;
            }
#endif

        virtual int get_params_count() const
            {
            return ADDITIONAL_PARAM_COUNT;
            }

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

    protected:
#ifdef DEBUG_NO_IO_MODULES
        int st;
#endif

#ifndef DEBUG_NO_IO_MODULES
    public:
        float get_value();
        void  direct_set_value( float new_value );

#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Температура.
class temperature_e : public AI1
    {
    public:
        temperature_e( const char *dev_name ): AI1( dev_name, DT_TE, DST_TE,
            ADDITIONAL_PARAM_COUNT )
            {
            start_param_idx = AI1::get_params_count();
            set_par_name( P_ERR_T,  start_param_idx, "P_ERR_T" );
            }

        float get_value()
            {
#ifdef DEBUG_NO_IO_MODULES
            float v = analog_io_device::get_value();
            return -1000 == v ? get_par( P_ERR_T, start_param_idx ) :
                AI1::get_value();
#else
            float v = get_AI( C_AI_INDEX, 0, 0 );
            return -1000 == v ? get_par( P_ERR_T, start_param_idx ) :
                get_par( P_ZERO_ADJUST_COEFF, 0 ) + v;
#endif
            }

#ifndef DEBUG_NO_IO_MODULES
        int get_state()
            {
            float v = get_AI( C_AI_INDEX, 0, 0 );
            if ( -1000 == v )
                {
                return -1;
                }

            return 1;
            }
#endif

    private:
        u_int start_param_idx;
        enum CONSTANTS
            {
            P_ERR_T = 1,                ///< Аварийное значение температуры.

            ADDITIONAL_PARAM_COUNT = 1, ///< Количество параметров.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Температура.
class temperature_e_analog : public AI1
    {
    public:
        temperature_e_analog( const char* dev_name );

        float get_value();

#ifndef DEBUG_NO_IO_MODULES
        int get_state();
#endif

    private:
        u_int start_param_idx;
        enum CONSTANTS
            {
            P_ERR_T = 1,    ///< Аварийное значение температуры.
            P_MIN_V,        ///< Минимальное значение.
            P_MAX_V,        ///< Максимальное значение.

            LAST_PARAM_IDX,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик температуры IO-Link.
class temperature_e_iolink : public AI1
    {
    public:
        temperature_e_iolink( const char *dev_name );

        ~temperature_e_iolink();

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

#else
        float get_value();
#endif

    private:
        struct TE_data
            {
            int16_t v = 0;
            };

        TE_data *info;
		u_int start_param_idx;
		enum CONSTANTS
			{
			P_ERR_T = 1,                ///< Аварийное значение температуры.

			ADDITIONAL_PARAM_COUNT = 1, ///< Количество параметров.
			};
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс текущего уровня.
class level: public AI1
    {
    public:
        level( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt );

        int get_volume();
        virtual int calc_volume();

        int save_device_ex( char* buff );

        float get_max_val();
        float get_min_val();

        int get_params_count() const
            {
            return start_param_idx + LAST_PARAM_IDX - 1;
            }

    protected:
        enum CONSTANTS
            {
            P_ERR = 1,       ///< Аварийное значение уровня.

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень.
class level_e : public level
    {
    public:
        level_e( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень c для танка цилиндрической формы.
class level_e_cyl : public level
    {
    public:
        level_e_cyl( const char* dev_name );

        int calc_volume();

    private:
        enum CONSTANTS
            {
            P_MAX_P = 1, ///< Индекс параметра давление настройки датчика (бар).
            P_R,         ///< Индекс параметра радиуса танка (м).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень c для цилиндрического танка с конусным основанием.
class level_e_cone : public level
    {
    public:
        level_e_cone( const char* dev_name );

         int calc_volume();

    private:
        enum CONSTANTS
            {
            P_MAX_P = 1, ///< Индекс параметра давление настройки датчика (бар).
            P_R,         ///< Индекс параметра радиуса танка (м).
            P_H_CONE,    ///< Индекс параметра высоты конуса танка (м).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief Текущее давление.
class pressure_e : public AI1
    {
    public:
        pressure_e( const char *dev_name ): AI1( dev_name, DT_PT, DST_NONE,
            ADDITIONAL_PARAM_COUNT )
            {
            start_param_idx = AI1::get_params_count();
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
/// @brief Текущее давление.
class pressure_e_iolink : public analog_io_device
    {
    public:
        pressure_e_iolink( const char* dev_name );

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        int get_state();

        void direct_set_value( float new_value )
            {
            }
#endif
        void set_article( const char* new_article );

        enum ARTICLE
            {
            DEFAULT,
            IFM_PM1704,
            IFM_PM1705,
            IFM_PM1707,
            IFM_PM1708,
            IFM_PM1709,
            IFM_PM1715,

            IFM_PI2715,
            IFM_PI2794,
            IFM_PI2797,

            FES_8001446,
            };

        static void evaluate_io( const char *name, char* data, ARTICLE n_article, float& v,
            int& st );
        static void read_article( const char* article, ARTICLE& n_article,
            device* dev  );

        void evaluate_io();

        struct PT_data
            {
            uint16_t st1 :1;
            uint16_t st2 :1;
            int16_t  v   :14;
            };

        struct ex_PT_data
            {
            int16_t v;
            uint16_t  unused : 4;
            uint16_t  status : 4;
            int16_t reserved : 8;
            };

    private:
        ARTICLE n_article;

        enum CONSTANTS
            {
            C_AI_INDEX = 0,     ///< Индекс канала аналогового входа.

            P_ERR,              ///< Аварийное значение.

            LAST_PARAM_IDX,
            };

        float v;
        int st;
    };
//-----------------------------------------------------------------------------
/// @brief Автоматический выключатель.
class circuit_breaker : public analog_io_device
    {
    public:
        circuit_breaker(const char* dev_name);

        int save_device_ex(char* buff);

        int set_cmd(const char* prop, u_int idx, double val);

        void direct_set_value(float v);

        void direct_on();

        void direct_off();

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        int get_state();
#endif

        void evaluate_io();

        struct F_data_in
            {
            bool err_ch4 : 1;
            bool err_ch3 : 1;
            bool err_ch2 : 1;
            bool err_ch1 : 1;
            bool st_ch4 : 1;
            bool st_ch3 : 1;
            bool st_ch2 : 1;
            bool st_ch1 : 1;

            uint16_t nominal_current_ch2 : 4;
            uint16_t nominal_current_ch1 : 4;
            uint16_t nominal_current_ch4 : 4;
            uint16_t nominal_current_ch3 : 4;

            uint16_t load_current_ch1 : 8;
            uint16_t load_current_ch2 : 8;
            uint16_t load_current_ch3 : 8;
            uint16_t load_current_ch4 : 8;
            uint16_t v : 8;
            };

        struct F_data_out
            {
            bool switch_ch1 : 1;
            bool switch_ch2 : 1;
            bool switch_ch3 : 1;
            bool switch_ch4 : 1;
            uint16_t reserved : 3;
            bool valid_flag : 1;

            uint16_t nominal_current_ch2 : 4;
            uint16_t nominal_current_ch1 : 4;
            uint16_t nominal_current_ch4 : 4;
            uint16_t nominal_current_ch3 : 4;

            F_data_out();
            };

    private:
        enum CONSTANTS
            {
            C_AI_INDEX = 0,
            };

        bool is_read_OK;

        float v;
        int st;
        int err;
        int m;

        F_data_in in_info;
        static F_data_out stub_out_info;
        F_data_out* out_info = &stub_out_info;
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня IO-Link.
class level_e_iolink : public level
    {
    public:
        level_e_iolink( const char* dev_name );

        int calc_volume();

#ifndef DEBUG_NO_IO_MODULES
        float get_value();
        int get_state();
#endif

        void set_article( const char* new_article );
        void evaluate_io();

        void set_string_property(const char* field, const char* value) override;

    private:
        pressure_e_iolink::ARTICLE n_article;

    private:
        enum CONSTANTS
            {
            P_MAX_P = 1, ///< Индекс параметра давление настройки датчика (бар).
            P_R,         ///< Индекс параметра радиуса танка (м).
            P_H_CONE,    ///< Индекс параметра высоты конуса танка (м).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;

        int st;
        float v;

        i_AI_device* PT_extra;
    };
//-----------------------------------------------------------------------------
/// @brief Концентрация.
class concentration_e : public AI1
    {
    public:
        concentration_e( const char* dev_name, DEVICE_SUB_TYPE sub_type ) :
            AI1( dev_name, DT_QT, sub_type, ADDITIONAL_PARAM_COUNT )
            {
#ifdef DEBUG_NO_IO_MODULES
            st = 1;
#endif
            start_param_idx = AI1::get_params_count();
            set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
            set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
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
/// @brief Концентрация с сигналом диагностики.
class concentration_e_ok : public concentration_e
    {
    public:
        concentration_e_ok( const char *dev_name ) : concentration_e( dev_name,
            DST_QT_OK )
            {
            }

        int get_state()
            {
#ifndef DEBUG_NO_IO_MODULES
            int i = get_DI( DI_INDEX );

            return i == 1 ? concentration_e::get_state() : -1;
#else
            return concentration_e::get_state();
#endif
            }

        int save_device_ex( char *buff )
            {
            int res = 0;
#ifdef DEBUG_NO_IO_MODULES
            res = sprintf( buff, "OK=1, " );
#else
            res = sprintf( buff, "OK=%d, ", get_DI( DI_INDEX ) );
#endif //DEBUG_NO_IO_MODULES
            return res;
            }

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,         ///< Индекс канала дискретного входа.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик концентрации IO-Link.
class concentration_e_iolink : public analog_io_device
    {
    public:
        concentration_e_iolink(const char* dev_name);

        ~concentration_e_iolink();

        int save_device_ex( char *buff );

        float get_temperature() const;

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        int get_state();

        void direct_set_value( float new_value )
            {
            }
#endif // DEBUG_NO_IO_MODULES


        void evaluate_io();

    private:

#pragma pack(push,1)
        struct QT_data
            {
            unsigned char              :4;
            unsigned char status       :4;
            unsigned char tmp;
            unsigned int  temperature;
            unsigned char tmp1;
            unsigned char tmp2;
            unsigned int  conductivity;
            };
#pragma pack(pop)

        QT_data* info;

    private:
        enum CONSTANTS
            {
            C_AI_INDEX = 0,     ///< Индекс канала аналогового входа.

            P_ERR,              ///< Аварийное значение.

            LAST_PARAM_IDX,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство аналогового входа.
class analog_input : public AI1
    {
    public:
        analog_input( const char *dev_name ): AI1( dev_name, DT_AI, DST_NONE,
            ADDITIONAL_PARAM_COUNT )
            {
            start_param_idx = AI1::get_params_count();
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
class virtual_wages : public device, public i_wages
    {
    public:
        virtual_wages( const char* dev_name );

    protected:
        float value;
        int state;

    public:

        virtual void direct_off();

        virtual void direct_set_value( float new_value );

        virtual float get_value();

        virtual void direct_set_state( int new_state );

        virtual void direct_on();

        virtual int get_state();

        virtual void tare();
    };
//-----------------------------------------------------------------------------
class wages_RS232 : public analog_io_device, public i_wages
    {
    public:
        wages_RS232( const char* dev_name );

        float get_value() override;

        int get_state() override;

        void evaluate_io() override;

#ifndef DEBUG_NO_IO_MODULES
        void direct_set_value( float new_value ) override;
#endif // DEBUG_NO_IO_MODULES

        void tare() override;

        void set_command( int new_state );

        float get_value_from_wages();

    private:
        enum class CONSTANTS
            {
            C_AIAO_INDEX = 0,   ///< Индекс канала аналоговых данных.

            P_CZ = 1,           ///< Сдвиг нуля.
            LAST_PARAM_IDX,
            };

        enum class STATES
            {
            TOGGLE_COMMAND = 2,
            BUFFER_MOD = 0,
            READ_CHARACTER = 12288,
            };

        int state;
        float value;
    };
//-----------------------------------------------------------------------------
class wages_eth : public device, public i_wages
    {
    public:
        explicit wages_eth( const char* dev_name );

        float get_value() override;

        int get_state() override;

        void evaluate_io() override;

        void direct_set_value( float new_value ) override;
        
        void direct_set_state( int state ) override;

        void direct_off() override;
        
        void direct_on() override;

        void tare() override;

        void set_string_property( const char* field, const char* value ) override;

        void direct_set_tcp_buff( const char* new_value, size_t size,
            int new_status );

    private:
        auto_smart_ptr < iot_wages_eth > weth;

        enum class CONSTANTS
        {
            C_AIAO_INDEX = 0,   ///< Индекс канала аналоговых данных.

            P_CZ,           ///< Сдвиг нуля.
            LAST_PARAM_IDX,
        };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик веса
class wages : public analog_io_device, public i_wages
    {
    public:
        wages( const char *dev_name);

        void tare();
        float get_weight();

    protected:

        enum CONSTANTS
            {
            P_NOMINAL_W = 1,    ///< Номинальная нагрузка.
            P_RKP = 2,          ///< Рабочий коэффициент передачи
            P_C0 = 3,           ///< Коррекция нуля
            P_DT = 4,           ///< Коэффициент фильтра

            ADDITIONAL_PARAM_COUNT = 4, ///< Количество параметров.

            C_AI_Ud = 0,             ///< Индекс канала Ud(милливольты).
            C_AI_Uref = 1,           ///< Индекс канала Uref(вольты).
            };

        enum WAGES_STATES
            {
            S_NONE = 0,
            S_TARE = 1,
            };

        float weight;
        unsigned long filter_time;

#ifdef DEBUG_NO_IO_MODULES
        float get_value();
        void  direct_set_value( float new_value );
#endif // DEBUG_NO_IO_MODULES

#ifndef DEBUG_NO_IO_MODULES
    public:
        float get_value();
        void direct_set_state( int new_state );
        void  direct_set_value( float new_value )
            {
            return;
            }

#endif // DEBUG_NO_IO_MODULES
    public:
        int get_state()
            {
            return 0;
            }

        int save_device_ex( char *buff )
            {
            return sprintf( buff, "W=%.3f, ", get_value() );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство без привязки к модулям ввода-вывода
class virtual_device : public device
    {
    public:
        virtual_device( const char *dev_name, device::DEVICE_TYPE dev_type,
            device::DEVICE_SUB_TYPE dev_sub_type);

    protected:

        float value;
        int state;
        bool level_logic_invert;

    public:

        virtual void direct_off();

        virtual void direct_set_value( float new_value );

        virtual float get_value();

        virtual void direct_set_state( int new_state );

        virtual void direct_on();

        virtual int get_state();

        virtual bool is_active();

        virtual void set_rt_par(unsigned int idx, float value);
     };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство без привязки к модулям ввода-вывода
class virtual_counter : public device, public i_counter
    {
    public:
        virtual_counter( const char *dev_name );

        float get_value();

        void direct_set_value( float new_value );

        int get_state();

        void direct_on();

        void  direct_off();

        void direct_set_state( int new_state );

        void pause();

        void start();

        void reset();

        u_int get_quantity();

        float get_flow();

        /// @brief Получение абсолютного значения счетчика (без учета
        /// состояния паузы).
        u_int get_abs_quantity();

        /// @brief Сброс абсолютного значения счетчика.
        void  abs_reset();

        int set_cmd( const char *prop, u_int idx, double val );

        void set( u_int value, u_int abs_value, float flow );

        void eval( u_int read_value, u_int abs_read_value, float read_flow );

        //Lua.
        int save_device_ex( char *buff );

        u_long get_pump_dt() const override;
        float get_min_flow() const override;

    protected:
        STATES state;        

    private:
        float flow_value;

        u_int value;
        u_int last_read_value;

        u_int abs_value;       ///< Абсолютное значение (не становится на паузу).
        u_int abs_last_read_value;

        bool is_first_read;         ///< Флаг первой установки значения.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым выходом.
///
/// Это может быть управляемый клапан...
class AO1 : public analog_io_device
    {
    public:
        AO1( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt ):
        analog_io_device( dev_name, type, sub_type, par_cnt )
            {
            }

        virtual float get_min_value() = 0;
        virtual float get_max_value() = 0;

#ifndef DEBUG_NO_IO_MODULES
        float get_value();
        void  direct_set_value( float new_value );
#endif // DEBUG_NO_IO_MODULES

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
            set_par_name( P_MIN_VALUE, 0, "P_MIN_V" );
            set_par_name( P_MAX_VALUE, 0, "P_MAX_V" );
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
        analog_valve( const char *dev_name ): AO1( dev_name, DT_VC, DST_VC, 0 )
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
/// @brief Управляемый клапан IO-Link.
class analog_valve_iolink : public AO1
    {
    public:
        analog_valve_iolink( const char* dev_name );

        ~analog_valve_iolink();

        void evaluate_io();

        float get_min_value();

        float get_max_value();

        int save_device_ex( char* buff );

        enum class CONSTANTS
            {
            FULL_CLOSED = 0,
            FULL_OPENED = 100,
            };

#ifndef DEBUG_NO_IO_MODULES
        void direct_on();

        void direct_off();

        void direct_set_value( float new_value );

        float get_value();

        int set_cmd( const char* prop, u_int idx, double val );

        int get_state();
#endif

    private:
        struct in_data
            {
            float position;         //Valve position in percent
            float setpoint;         //Used setpoint in percent
            uint8_t namur_state;
            uint8_t status : 6;
            bool opened : 1;        //True = Opened, False = Not opened
            bool closed : 1;        //True = Closed, False = Not closed
            };

#pragma pack(push,1)
        struct out_data
            {
            float position;     //Cyclic CMD setpoint in percent
            uint8_t unused : 7;
            bool wink : 1;      //Visual indication
            };
#pragma pack(pop)

        in_data* in_info = new in_data;
        static out_data stub_out_info;
        out_data* out_info = &stub_out_info;

        bool blink = false;     //Visual indication
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным входом.
///
/// Это может быть обратная связь, расход (есть/нет)...
class DI1 : public digital_io_device
    {
    public:
        DI1( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt, int current_state_init_val = 0 ):
        digital_io_device( dev_name, type, sub_type,
            ADDITIONAL_PARAMS_COUNT + par_cnt ),
            current_state( current_state_init_val ),
            time( 0 )
            {
            set_par_name( P_DT,  0, "P_DT" );
            }

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

        int get_state();
#else
        /// @brief Получение состояния объекта.
        ///
        /// @return - состояние объекта.
        int  get_state();
#endif // DEBUG_NO_IO_MODULES


    private:
        int current_state;
        u_int_4 time;

        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< Индекс канала дискретного входа.

            ADDITIONAL_PARAMS_COUNT = 1,

            P_DT = 1,
            };
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

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_IO_MODULES

    protected:
        /// @brief Получение состояния клапана без учета обратной связи.
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            return ( VALVE_STATE ) get_DO( DO_INDEX );
#endif // DEBUG_NO_IO_MODULES
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
class i_motor : public device
    {
    public:
        i_motor( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
            int params_count );

        /// @brief Включение мотора в реверсном направлении.
        void reverse();

        /// @brief Получение линейной скорости (например, приводимого в
        // движение конвейра).
        virtual float get_linear_speed() const;

        /// @brief Получение текущего тока мотора
        virtual float get_amperage() const;
    };
//-----------------------------------------------------------------------------
class virtual_motor : public i_motor
    {
    public:
        virtual_motor( const char* dev_name );

    protected:
        float value;
        int state;

    public:

        virtual void direct_off();

        virtual void direct_set_value( float new_value );

        virtual float get_value();

        virtual void direct_set_state( int new_state );

        virtual void direct_on();

        virtual int get_state();

    };
//-----------------------------------------------------------------------------
/// @brief Электродвигатель (мешалка, насос).
class motor : public i_motor, public io_device
    {
    public:
        motor( const char *dev_name, device::DEVICE_SUB_TYPE sub_type ):
            i_motor( dev_name, sub_type, ADDITIONAL_PARAM_COUNT ),
            io_device( dev_name ),
            start_switch_time( get_millisec() )
#ifdef DEBUG_NO_IO_MODULES
            ,state( 0 ),
            freq( 0 )
#endif // DEBUG_NO_IO_MODULES
            {
            set_par_name( P_ON_TIME,  0, "P_ON_TIME" );
            }

        int save_device_ex( char *buff );

        float get_value();

        void direct_set_value( float value );

        void direct_set_state( int new_state );

        int  get_state();

        void direct_on();

        void direct_off();

        virtual void print() const
            {
            device::print();
            }

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 1,

            C_MIN_VALUE = 0,
            C_MAX_VALUE = 100,

            P_ON_TIME = 1,    ///< Индекс параметра времени включения (мсек).

            DO_INDEX = 0,         ///< Индекс канала дискретного выхода.
            DO_INDEX_REVERSE = 1, ///< Индекс канала дискретного выхода реверса.

            DI_INDEX       = 0,   ///< Индекс канала дискретного входа.
                                  //   Или
            DI_INDEX_ERROR = 0,   ///< Индекс канала дискретного входа ошибки.

            AO_INDEX = 0,     ///< Индекс канала аналогового выхода.
            };

        u_long start_switch_time;

#ifdef DEBUG_NO_IO_MODULES
        char  state;  ///< Состояние устройства.

        float freq;   ///< Состояние устройства (частота).
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// @brief Электродвигатель, управляемый частотным преобразователем altivar с
/// интерфейсной платой Ethernet.
class motor_altivar : public i_motor, public io_device
{
public:
    motor_altivar(const char *dev_name, device::DEVICE_SUB_TYPE sub_type,
        u_int par_cnt = 0 );

    int save_device_ex(char *buff);

    float get_value();

    void direct_set_value(float value);

    void direct_set_state(int new_state);

    int  get_state();

    void direct_on();

    void direct_off();

    void set_string_property(const char* field, const char* value) override;

    virtual void print() const;

    virtual int get_params_count() const;

    float get_amperage() const override;

#ifdef DEBUG_NO_IO_MODULES
    int set_cmd( const char* prop, u_int idx, double val ) override;
#endif // DEBUG_NO_IO_MODULES

protected:
    altivar_node* atv;

#ifdef DEBUG_NO_IO_MODULES
    float freq = .0f;
    char state = 0;
    int reverse = 0;
    int rpm = 0;
    int est = 0;
    float amperage = .0f;
#endif // DEBUG_NO_IO_MODULES

private:
    enum CONSTANTS
    {
        ADDITIONAL_PARAM_COUNT = 1,

        C_MIN_VALUE = 0,
        C_MAX_VALUE = 100,

        P_ON_TIME = 1,          ///< Индекс параметра времени включения (мсек).

        DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
        DO_INDEX_REVERSE = 1,   ///< Индекс канала дискретного выхода реверса.

        DI_INDEX = 0,           ///< Индекс канала дискретного входа.
        //   Или
        DI_INDEX_ERROR = 0,     ///< Индекс канала дискретного входа ошибки.

        AO_INDEX = 0,           ///< Индекс канала аналогового выхода.
    };

    u_long start_switch_time;
};
//-----------------------------------------------------------------------------
/// @brief Электродвигатель, управляемый частотным преобразователем altivar с
/// интерфейсной платой Ethernet c расчетом линейной скорости.
class motor_altivar_linear : public motor_altivar
    {
    public:
        motor_altivar_linear( const char* dev_name );

        /// @brief Получение линейной скорости.
        float get_linear_speed() const;

    private:
        int start_param_idx;

        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 2,

            P_SHAFT_DIAMETER = 1,   ///< Диаметр вала (м).
            P_TRANSFER_RATIO,       ///< Передаточный коэффициент.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня.
class level_s : public DI1
    {
    public:
        level_s( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

        bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня IO-Link.
class level_s_iolink : public analog_io_device
    {
    public:
        level_s_iolink( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        int get_state();

        void direct_set_value( float new_value )
            {
            }
#endif

        bool is_active();

        void evaluate_io();

    private:
        int current_state;
        u_int_4 time;

        enum ARTICLE
            {
            DEFAULT,
            IFM_LMT100,
            IFM_LMT102,
            IFM_LMT104,
            IFM_LMT105,

            EH_FTL33,
            };
        ARTICLE n_article;

        void set_article( const char* new_article );

        struct LS_data
            {
            uint16_t st1 :1;
            uint16_t st2 :1;
            int16_t  v   :14;
            };
        struct rev_LS_data
            {
            int16_t  v   : 14;
            uint16_t st1 : 1;
            uint16_t st2 : 1;
            };

        float v;
        int st;

    private:
        enum CONSTANTS
            {
            C_AI_INDEX = 0,     ///< Индекс канала аналогового входа.

            P_DT,
            P_ERR,              ///< Аварийное значение уровня.

            LAST_PARAM_IDX,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора расхода.
class flow_s : public DI1
    {
    public:
        flow_s( const char *dev_name ): DI1( dev_name, DT_FS, DST_NONE,
            0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик положения.
class state_s : public DI1
    {
    public:
        state_s( const char *dev_name ): DI1( dev_name, DT_GS, DST_NONE,
            0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик разности давления.
class diff_pressure : public DI1
    {
    public:
        diff_pressure( const char* dev_name ) : DI1( dev_name, DT_PDS,
            DST_PDS_VIRT, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Сигнальный датчик температуры.
class temperature_signal : public DI1
    {
    public:
        temperature_signal( const char* dev_name ) : DI1( dev_name, DT_TS,
            DST_TS_VIRT, 0 )
            {
            }
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
/// @brief Базовый счетчик.
class base_counter: public i_counter, public device, public io_device
    {
    public:
        base_counter( const char* dev_name, DEVICE_SUB_TYPE sub_type,
            int extra_par_cnt );

        void evaluate_io();

        void print() const override;

        /// @brief Приостановка работы счетчика.
        virtual void pause();

        /// @brief Возобновление работы счетчика.
        virtual void start();

        /// @brief Сброс счетчика и остановка счета.
        ///
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset();

        /// @brief Сброс абсолютного значения счетчика.
        void abs_reset();

        /// @brief Сброс счетчика и продолжение счета.
        void restart();

        /// @brief Получение состояния работы счетчика.
        virtual int get_state();

        void direct_on();

        void direct_off();

        float get_value();

        void direct_set_state( int new_state );

        void direct_set_value( float new_value );

        virtual void set_property( const char* field, device* dev );

        /// @brief Получение значение счетчика от устройства.
        virtual float get_raw_value() const = 0;

        /// @brief Получение максимального значение счетчика от устройства.
        virtual float get_max_raw_value() const = 0;

        void calculate_quantity( float& value, float& last_read_value,
            bool& is_first_read );

        /// @brief Получение значения счетчика (c учетом паузы).
        u_int get_quantity();

        /// @brief Получение абсолютного значения счетчика (без учета паузы).
        u_int get_abs_quantity();

        int set_cmd( const char* prop, u_int idx, double val );

        int save_device_ex( char* buff );

        const char* get_error_description() const override;

    protected:
        float get_abs_value() const
            {
            return abs_value;
            }

    private:
        void set_abs_value( float new_value );

        const int MAX_OVERFLOW = 300;   ///< Максимальное переполнение за цикл.

        STATES state = STATES::S_WORK;

        u_int_4 start_pump_working_time = 0;
        u_int_4 counter_prev_value = 0;

        std::vector < device* > motors;

        bool is_first_read = true;      ///< Флаг первой установки значения.
        float value = .0f;
        float last_read_value = 0.f;

        bool abs_is_first_read = true;
        float abs_value = 0.f;  ///< Абсолютное значение (не становится на паузу).
        float abs_last_read_value = 0.f;
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик.
class counter : public base_counter
    {
    public:
        counter( const char *dev_name,
            DEVICE_SUB_TYPE sub_type = device::DST_FQT,
            int extra_par_cnt = ADDITIONAL_PARAMS_COUNT );

        float get_raw_value() const override;

        float get_max_raw_value() const override;

        float get_flow() override;

        u_long get_pump_dt() const override;
        float get_min_flow() const override;
    private:

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 0,

            AI_Q_INDEX    = 0,  ///< Индекс канала аналогового входа (объем).
            };
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Счетчик.
class counter_f : public counter
    {
    public:
        counter_f( const char *dev_name );

        int get_state();

        float get_flow() override;

        int save_device_ex( char* buff );

        int set_cmd( const char* prop, u_int idx, double val );

        u_long get_pump_dt() const override;
        float get_min_flow() const override;

    private:
        enum CONSTANTS
            {
            P_MIN_FLOW = 1,
            P_MAX_FLOW,
            P_CZ,
            P_DT,
            P_ERR_MIN_FLOW,
            LAST_PARAM_IDX,

            AI_FLOW_INDEX = 1,  ///< Индекс канала аналогового входа (поток).
            };

        float flow_value = 0.f;
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик c диагностикой.
class counter_f_ok : public counter_f
    {
    public:
        counter_f_ok( const char *dev_name );

        //Lua.
        int save_device_ex( char *buff );

        int get_state();

    private:

        enum CONSTANTS
            {
            DI_INDEX = 0,  ///< Индекс канала дискретного входа (диагностики).
            };
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик IO-Link.
class counter_iolink : public base_counter
    {
    public:
        counter_iolink( const char* dev_name );

        void evaluate_io();

        float get_temperature() const;

        int save_device_ex( char* buff );

        int get_state();

        u_long get_pump_dt() const override;
        float get_min_flow() const override;

        float get_raw_value() const override;

        float get_max_raw_value() const override;

        float get_flow() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        u_int get_quantity() override;

        u_int get_abs_quantity() override;

        float get_value() override;

        static const int mL_in_L = 1000;

    private:
        enum class CONSTANTS
            {
            AI_INDEX = 0,   ///< Индекс канала аналогового входа.

            MAX_OVERFLOW = 300, ///< Максимальное переполнение за цикл   

            P_CZ = 1,
            P_DT,
            P_ERR_MIN_FLOW,

            LAST_PARAM_IDX,
            };

        struct in_data
            {
            float totalizer;    //Quantity meter which continuously totals the
                                //volumetric flow since the last reset.
            int16_t flow;       //Current flow.

            uint16_t out2 : 1;  //Status depends on [OU2].
            uint16_t out1 : 1;  //Status depends on [OU1].
            int16_t temperature : 14;   //Current temperature.
            };

        in_data in_info{ 0, 0, 0, 0, 0 };
    };
//-----------------------------------------------------------------------------
/// @brief Сигнальная колонна.
///
/// Служит для уведомления оператора о событиях.
/// Используется три лампочки – зеленая, желтая, красная и сирена:
///     1. Зеленый цвет – есть активная операция (связанного аппарата или
///     агрегата).
///     2. Мигающий красный цвет – авария, тревога (неподавленная). Частота
///     мигания 2 Гц. Дополнительно включается сирена, которую можно отключить
///     проекта (клик по значку) либо она также отключится после подтверждения
///     тревог, пропадания аварий.
///     3. Мигающий желтый свет – неподтвержденное сообщение. Частота мигания
///     0.5 Гц.
class signal_column : public device, public io_device
    {
    public:
        signal_column( const char* dev_name, DEVICE_SUB_TYPE sub_type,
            int red_lamp_channel = 0, int yellow_lamp_channel = 0,
            int green_lamp_channel = 0, int blue_lamp_channel = 0,
            int siren_channel = 0 );

        void turn_off_red();
        void turn_off_yellow();
        void turn_off_green();
        void turn_off_blue();

        void turn_on_red();
        void turn_on_yellow();
        void turn_on_green();
        void turn_on_blue();

        void normal_blink_red();
        void normal_blink_yellow();
        void normal_blink_green();
        void normal_blink_blue();

        void slow_blink_red();
        void slow_blink_yellow();
        void slow_blink_green();
        void slow_blink_blue();

        void turn_on_siren();
        void turn_off_siren();

        enum STATE
            {
            TURN_OFF,

            TURN_ON,

            LIGHTS_OFF,

            GREEN_ON,
            YELLOW_ON,
            RED_ON,

            GREEN_OFF,
            YELLOW_OFF,
            RED_OFF,

            GREEN_NORMAL_BLINK,
            YELLOW_NORMAL_BLINK,
            RED_NORMAL_BLINK,

            GREEN_SLOW_BLINK,
            YELLOW_SLOW_BLINK,
            RED_SLOW_BLINK,

            SIREN_ON,
            SIREN_OFF,
            };

        void set_rt_par( u_int idx, float value );

        void direct_set_state( int new_state );
        void direct_off();
        void direct_on();

        void direct_set_value( float new_value );
        int get_state();
        float get_value();

        int save_device_ex( char* buff );

#ifdef _MSC_VER
#pragma region Сигнализация о событиях
#endif
        /// @brief Расчет состояния на основе текущих данных от I/O.
        void evaluate_io();

        void show_error_exists();

        void show_message_exists();

        void show_batch_is_not_running();

        void show_batch_is_running();

        void show_operation_is_not_running();

        void show_operation_is_running();

        void show_idle();

    private:
        enum class show_states
            {
            idle,

            error_exists,
            message_exists,

            batch_is_not_running,
            batch_is_running,

            operation_is_not_running,
            operation_is_running
            };

        show_states show_state;

#ifdef _MSC_VER
#pragma endregion
#endif

    protected:
        enum class DO_state
            {
            OFF,
            ON
            };

        const char* RED_LAMP = "red lamp";
        const char* GREEN_LAMP = "green lamp";
        const char* YELLOW_LAMP = "yellow lamp";
        const char* BLUE_LAMP = "blue lamp";
        const char* SIREN = "siren";

        virtual void process_DO( u_int n, DO_state state, const char* name ) = 0;

        ///Тип мигания (>0 - реализуем сами, 0 - встроенный в сирену).
        int is_const_red;

        int red_lamp_channel;
        int yellow_lamp_channel;
        int green_lamp_channel;
        int blue_lamp_channel;
        int siren_channel;

        enum class CONSTANTS
            {
            NORMAL_BLINK_TIME = 1000 / 2 / 2,                 //2 Гц
            SLOW_BLINK_TIME = (int) (1000 / 0.5f / 2),        //0.5 Гц
            };

        enum class STEP
            {
            off = 0,
            on,
            blink_off,
            blink_on,
            };

        struct state_info
            {
            STEP step;
            unsigned long start_blink_time;
            unsigned long start_wait_time;

            state_info();
            };

        state_info green;
        state_info yellow;
        state_info red;
        state_info blue;

        void blink( int lamp_DO, state_info& info, u_int delay_time );

        STEP siren_step;
    };
//-----------------------------------------------------------------------------
/// @brief Сигнальная колонна с дискретным подключением.
    class signal_column_discrete : public signal_column
        {
        public:
            signal_column_discrete( const char* dev_name,
                int red_lamp_channel = 1, int yellow_lamp_channel = 2,
                int green_lamp_channel = 3, int blue_lamp_channel = 0,
                int siren_channel = 4 );

        protected:
            void process_DO( u_int n, DO_state state, const char* name ) override;
        };
//-----------------------------------------------------------------------------
/// @brief Сигнальная колонна с IO-Link.
///
/// Служит для уведомления оператора о событиях.
class signal_column_iolink : public signal_column
    {
    public:
        signal_column_iolink( const char* dev_name );

        void set_string_property( const char* field, const char* value );

    protected:
        void process_DO( u_int n, DO_state state, const char* name ) override;

    private:
        void evaluate_io();

        struct out_data
            {
            uint16_t unused1 : 8;
            bool switch_ch1 : 1;
            bool switch_ch2 : 1;
            bool switch_ch3 : 1;
            bool switch_ch4 : 1;
            bool switch_ch5 : 1;
            uint16_t unused2 : 3;
            };

        static out_data stub_out_info;
        out_data *out_info = &stub_out_info;
    };
//-----------------------------------------------------------------------------
/// @brief Камера.
///
/// Служит для получения событий о распозновании объекта.
class i_camera
    {
    public:
        /// @brief Получение статуса событий от камеры.
        virtual int get_result( int n = 1 ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Камера.
///
/// Служит для получения событий о распозновании объекта.
class camera : public i_camera, public device, public io_device
    {
    public:
        camera( const char* dev_name, DEVICE_SUB_TYPE sub_type,
            int params_count = 0 );

        void direct_set_state( int new_state );

        void direct_off();

        void direct_on();

        void direct_set_value( float new_value );

        int get_state();

        float get_value();

        int save_device_ex( char* buff );

        int set_cmd( const char* prop, u_int idx, double val );

        void set_string_property( const char* field, const char* value );

        int get_result( int n = 1 );

        /// @brief Получение состояние готовности.
        virtual bool is_ready() const;

    protected:
        bool is_cam_ready;
        int result;
        int state;

        enum class CONSTANTS
            {
            INDEX_DO = 0,

            INDEX_DI_RES_1 = 0,
            INDEX_DI_READY,
            INDEX_DI_RES_2,
            };

    private:
        std::string ip;
    };
//-----------------------------------------------------------------------------
/// @brief Камера.
///
/// Служит для получения событий о распозновании объекта.
class camera_DI2 : public camera
    {
    public:
        camera_DI2( const char* dev_name, DEVICE_SUB_TYPE sub_type );

        int get_state();

        void evaluate_io() override;

    protected:
        u_int start_switch_time;

    private:
        enum class PARAMS
            {
            P_READY_TIME = 1,

            PARAMS_CNT,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Камера.
///
/// Служит для получения событий о распозновании объекта.
class camera_DI3 : public camera_DI2
    {
    public:
        camera_DI3( const char* dev_name );

        void evaluate_io();

        int get_result( int n = 1 );

    private:
        int result_2;
    };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство.
///
/// Необходимо для возвращения результата поиска устройства с несуществующим
/// номером. Методы данного класса ничего не делают.
class dev_stub : public i_counter, public valve, public i_wages,
    public camera, public signal_column
    {
    public:
        dev_stub();

        u_int_4 get_serial_n() const;
        void    print() const;

        float get_value();
        void direct_set_value( float new_value );

        void off();
        void on();

        void direct_on();
        void direct_off();
        void direct_set_state( int new_state );

        void set_value( float new_value );
        void set_state( int new_state );

        int get_state();

        VALVE_STATE get_valve_state();

        void    pause();
        void    start();
        void    reset();
        u_int   get_quantity();
        float   get_flow();

        u_long get_pump_dt() const;
        float get_min_flow() const;

        u_int get_abs_quantity();
        void  abs_reset();

        void tare();

        void process_DO( u_int n, DO_state state, const char* name ) override;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class threshold_regulator :public device, public i_Lua_save_device
    {
    public:
        enum class PARAM
            {
            P_IS_REVERSE = 1,	        ///Обратного (реверсивного) действия.
            P_DELTA,

            PARAMS_COUNT
            };

        explicit threshold_regulator( const char* name );

        int get_state() override;

        void direct_off() override;

        void direct_set_state( int new_state ) override;

        void direct_on() override;

        const char* get_name_in_Lua() const override;

        float get_value() override;

        void direct_set_value( float val ) override;

        int save_device( char* buff ) override;

        void set_string_property( const char* field, const char* value ) override;

    private:
        enum class STATE
            {
            OFF,
            ON,
            };

        STATE state = STATE::OFF;
        int out_state = 0;
        float set_value = 0;

        device* sensor = nullptr;
        device* actuator = nullptr;
    };
//-----------------------------------------------------------------------------
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
        device* get_device( const char* dev_name );

        /// @brief Получение устройства.
        device* get_device( u_int serial_dev_n )
            {
            if ( serial_dev_n < project_devices.size() )
                {
                return project_devices[ serial_dev_n ];
                }

            return get_stub_device();
            }

        /// @brief Получение клапана по его номеру.
        i_DO_device* get_V( const char *dev_name );

        /// @brief Получение управляемого клапана по номеру.
        i_AO_device* get_VC( const char *dev_name );

        /// @brief Получение двигателя по номеру.
        i_motor* get_M( const char *dev_name );

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
        virtual_counter* get_virtual_FQT( const char *dev_name );

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

        /// @brief Получение текущего давления по номеру.
        i_AI_device* get_PT( const char *dev_name );

        /// @brief Получение текущей концентрации по номеру.
        i_AI_device* get_QT( const char *dev_name );

        /// @brief Получение весов по номеру.
        wages* get_WT( const char *dev_name );

        /// @brief Получение регулятора по имени.
        i_DO_AO_device* get_C( const char* dev_name );

        /// @brief Получение сигнальной колонны по имени.
        signal_column* get_HLA( const char* dev_name );

        /// @brief Получение камеры по имени.
        camera* get_CAM( const char* dev_name );

        /// @brief Получение датчика разности давления по имени.
        i_DI_device* get_PDS( const char* dev_name );

        /// @brief Получение автоматического выключателя по имени.
        i_DO_AO_device* get_F(const char* dev_name);

        /// @brief Получение сигнального датчика температуры по имени.
        i_DI_device* get_TS( const char* dev_name );

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

        device* get_stub_device()
            {
            return dynamic_cast<device*>( dynamic_cast<valve*>( &stub ) );
            }

        int init_params();

        int init_rt_params();

        void evaluate_io()
            {
            for ( u_int i = 0; i < project_devices.size(); i++ )
                {
                project_devices[ i ]->evaluate_io();
                }
            }

#ifdef __BORLANDC__
#pragma option -w-inl
#endif // __BORLANDC__
        int save_device( char *buff );

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

    protected:
        char is_first_device[ device::C_DEVICE_TYPE_CNT ] = { 0 };

        dev_stub stub;  ///< Устройство-заглушка, фиктивное устройство.

        struct range    ///< Диапазон устройств одного типа.
            {
            int start_pos;
            int end_pos;

            range(): start_pos( -1 ), end_pos( -1 )
                {
                }
            };

        /// Диапазоны устройств всех типов.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        int get_device_n( device::DEVICE_TYPE dev_type,
            const char *dev_name );

        int get_device_n( const char* dev_name );

        std::vector< device* > project_devices; ///< Все устройства.

        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < device_manager > instance;

    public:
        // @brief Добавление устройства.
        //
        // Вызывается из Lua.
        io_device* add_io_device( int dev_type, int dev_sub_type,
            const char *dev_name, const char * descr, const char* article );

        // @brief Очистка списка устройств.
        //
        // Для тестирования.
        void clear_io_devices();
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
valve* V( const char *dev_name );
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
i_motor* M( u_int dev_n );
i_motor* M( const char *dev_name );
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

virtual_counter* virtual_FQT( const char *dev_name );
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
level* LT( const char *dev_name );
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
/// @brief Получение текущего давления по номеру.
///
/// @param number - номер текущего давления.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* PT( u_int dev_n );
i_AI_device* PT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение текущей концентрации по номеру.
///
/// @param number - номер текущей концентрации.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* QT( u_int dev_n );
i_AI_device* QT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение весов по номеру.
///
/// @param number - номер весов.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
wages* WT( u_int dev_n );
wages* WT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение автоматического выключателя по номеру.
///
/// @param number - номер автоматического выключателя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_AO_device* F(u_int dev_n);
i_DO_AO_device* F(const char* dev_name);
//-----------------------------------------------------------------------------
/// @brief Получение регулятора по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_AO_device* C( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнальной колонны по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
signal_column* HLA( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение камеры по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
camera* CAM( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение датчика разности давления.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* PDS( const char* dev_name );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального датчика температуры по имени.
///
/// @param dev_name - имя.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* TS( const char* dev_name );
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

#ifdef WIN_OS
#pragma warning(pop)
#endif // WIN_OS

#endif // PAC_DEVICES_H
