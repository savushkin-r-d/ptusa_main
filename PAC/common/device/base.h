/// @brief Классы, которые реализуют функции передачи
/// состояния устройств PAC на сервер. Также реализованы классы для работы с
/// устройствами (клапаны, насосы, ...). Классы используются ТОЛЬКО в
/// контроллере (PAC).
/// 

#pragma once
#include <array>

#include "s_types.h"
#include "param_ex.h"
#include "analog_emulator.h"
#include "bus_coupler_io.h"

class PID;

const size_t MAX_COPY_SIZE = 2000;
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
        explicit par_device( u_int par_cnt );

        virtual ~par_device();
        
        /// Unmovable classe.
        par_device& operator=( par_device&& ) = delete;

        /// @brief Сохранение в виде скрипта Lua.
        ///
        /// Сохраняем на основе названий параметров в виде таблицы:
        /// имя_параметра = 'значение'.
        ///
        /// @param prefix - префикс перед строкой скрипта (обычно символ
        /// табуляции - для визуального форматирования текста).
        /// @param buff [out] - буфер записи строки.
        virtual int save_device( char* buff, const char* prefix = "" );

        /// @brief Выполнение команда (установка значения параметра).
        ///
        /// @param name - имя команды (модифицируемого параметра).
        /// @param val - новое значение.
        ///
        /// @return 0 - ок.
        /// @return 1 - ошибка, параметр с таким именем не найден.
        int set_par_by_name( const char* name, double val );

        /// @brief Установка значения параметра.
        ///
        /// @param idx - индекс параметра (с единицы).
        /// @param offset - смещение индекса.
        /// @param value - новое значение.
        void set_par( u_int idx, u_int offset, float value );

        /// @brief Установка значения рабочего параметра.
        ///
        /// @param idx - индекс рабочего параметра (с единицы).
        /// @param value - новое значение.
        virtual void set_rt_par( u_int idx, float value )
            {
            // Ничего не делаем.
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
        void set_err_par( saved_params_u_int_4* new_err_par )
            {
            err_par = new_err_par;
            }

        float get_par( int idx ) const
            {
            return par[ 0 ][ idx ];
            }

    private:
        /// @brief Ошибки устройства.
        saved_params_u_int_4* err_par = nullptr;

        enum CONSTANTS
            {
            C_MAX_PAR_NAME_LENGTH = 25, ///< Максимальная длина имени параметра.
            };

        saved_params_float* par = nullptr; ///< Параметры.
        char** par_name = nullptr;         ///< Названия параметров.
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        virtual ~i_counter() = default;

        /// @brief Приостановка работы счетчика.
        virtual void pause() = 0;

        /// @brief Возобновление работы счетчика.
        virtual void start() = 0;

        /// @brief Сброс счетчика и остановка счета.
        ///
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset() = 0;

        /// @brief Сброс счетчика и продолжение счета.
        virtual void restart();

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

        enum DAY_CTR
            {
            DAY_T1,
            DAY_T2
            };

        /// @brief Приостановка работы дневного счетчика.
        virtual void pause_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) = 0;

        /// @brief Возобновление работы дневного счетчика.
        virtual void start_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) = 0;

        enum class STATES
            {
            S_WORK = 1,
            S_PAUSE,

            S_PUMP_ERROR = -9,  // Ошибка связанного насоса.
            S_FLOW_ERROR = -10, // Ошибка расхода (самотёка).

            S_LOW_ERR = -12,
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
        virtual ~i_mix_proof() = default;

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
        virtual ~i_wages() = default;

        /// @brief Тарировка.
        virtual void tare() = 0;
        ///@brief Возвращает вес в килограммах
        virtual float get_value() = 0;
        ///@brief Возвращает состояние
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного входа.
///
/// Обратная связь, предельный уровень и т.д. являются примерами таких
/// устройств.
class i_DI_device : public i_cmd_device
    {
    public:
        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() = 0;

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
class i_DO_device : public i_DI_device
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
class i_AI_device : public i_cmd_device
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
class i_AO_device : public i_AI_device
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
class i_DO_AO_device : public i_AO_device, public i_DO_device
    {
    public:
        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        void off() override = 0;

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        int get_state() override = 0;
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
        int set_cmd( const char* prop, u_int idx, double val ) override;

        /// @brief Выполнение команды.
        ///
        /// Запись в свойство объекта строки.
        ///
        /// @param prop - свойство объекта.
        /// @param idx  - индекс свойства.
        /// @param val  - значение.
        int set_cmd( const char* prop, u_int idx, const char* val ) override;

        /// @brief Сохранение устройства в виде скрипта Lua.
        ///
        /// @param prefix - префикс перед строкой скрипта (обычно символ
        /// табуляции - для визуального форматирования текста).
        /// @param buff [out] - буфер записи строки.
        int save_device( char* buff, const char* prefix = "" ) override;

        /// @brief Расчет состояния на основе текущих данных от I/O.
        virtual void evaluate_io();

        enum CONSTANTS
            {
            C_MAX_NAME = 30,
            C_MAX_DESCRIPTION = 100
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
            DT_G,        ///< Блок питания.
            DT_WATCHDOG, ///< Устройство проверки связи.
            DT_EY,       ///< Конвертер IO-Link.
            DT_NODE,     ///< Узел сетевых настроек.

            C_DEVICE_TYPE_CNT, ///< Количество типов устройств.
            };

        static const std::array<const char*,
            device::DEVICE_TYPE::C_DEVICE_TYPE_CNT> DEV_NAMES;

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

            DST_V_MINI_FLUSHING,      ///< Клапан с мини-клапаном промывки.

            ///< Противосмешивающий клапан (включение от IO-Link
            ///< пневмооострова) с тремя каналами управления.
            V_IOL_TERMINAL_MIXPROOF_DO3,

            ///< Противосмешивающий клапан (включение от IO-Link
            ///< пневмооострова) с тремя каналами управления и двумя обратными
            ///< связами.
            V_IOL_TERMINAL_MIXPROOF_DO3_DI2,

            //VC
            DST_VC = 1,         ///< Клапан с аналоговым управлением.
            DST_VC_IOLINK,      ///< Клапан IO-LInk.

            DST_VC_VIRT,        ///< Виртуальный клапан.
            DST_VC_EY,          ///< Клапан с аналоговым управлением, подключаемый к конвертеру.

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
            DST_FQT = 1,        ///< Счетчик.
            DST_FQT_F,          ///< Счетчик + расход.
            ///< Резерв.
            DST_FQT_VIRT = 4,   ///Виртуальный cчетчик (без привязки к модулям).
            DST_FQT_IOLINK,     /// Счетчик IO-Link.

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
            DST_GS = 1,     ///< Датчик положения.
            DST_GS_VIRT,    ///< Виртуальный датчик положения (без привязки к модулям).
            DST_GS_INVERSE, ///< Датчик положения инверсный (0 - активное значение).

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
            DST_WT = 1,     ///< Весы.
            DST_WT_VIRT,    ///< Виртуальные весы.
            DST_WT_RS232,   ///< Весы c RS232 интерфейсом.
            DST_WT_ETH,     ///< Весы c интерфейсом ethernet.
            DST_WT_PXC_AXL, ///< Весы c подключением к модулю Phoenix Axioline.

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

            //DT_G
            DST_G_IOL_4 = 1,    ///< 4 канала.
            DST_G_IOL_8,        ///< 8 каналов.

            //DT_WATCHDOG
            DST_WATCHDOG = 1,

            //DT_EY
            DST_CONV_AO2 = 1,    ///< Конвертер IO-Link -> AO (2 канала).

            //DT_NODE
            DST_NODE = 1,        ///< Узел сетевых настроек.
            };

        device( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        ~device() override;

        const char* get_type_str() const;

        const char* get_type_name() const;

        const char* get_name() const
            {
            return name;
            }

        const char* get_description() const
            {
            return description;
            }

        virtual const char* get_error_description()
            {
            if ( auto err_id = get_error_id(); err_id < 0 )
                {
                return "обратная связь";
                }

            return "нет ошибок";
            }

        /// @brief Получение ошибки (активной или ранее возникшей).
        virtual int get_error_id()
            {
            if ( auto st = get_state(); st < 0 )
                {
                prev_error_state = st;
                }
            return prev_error_state;
            }

        virtual void set_descr( const char* new_description );

        virtual void set_article( const char* new_article );

        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        void direct_off() override;

        void direct_on() override;

        void direct_set_state( int new_state ) override;

        void direct_set_value( float new_value ) override;

        /// @brief Выключение устройства с учетом ручного режима.
        void off() override;

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        int get_state() override;

        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде дробного числа.
        float get_value() override;

        /// @brief Вывод объекта в консоль.
        ///
        /// Для использования в отладочных целях.
        void print() const override;

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
        device::DEVICE_TYPE get_type() const
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
        virtual void set_property( const char* field, device* dev );

        /// @brief Установка дополнительных свойств, значения которых -
        /// строки.
        ///
        /// Для использования в Lua.
        virtual void set_string_property( const char* field, const char* new_value );

        /// @brief Сохранение дополнительных данных устройства в виде скрипта Lua.
        ///
        /// @param buff [out] - буфер записи строки.
        virtual int save_device_ex( char* buff )
            {
            buff[ 0 ] = 0;
            return 0;
            }

        bool get_manual_mode() const override
            {
            return is_manual_mode;
            }

        const char* get_article() const
            {
            return article;
            }

        void param_emulator( float math_expec, float stddev );

        bool is_emulation() const;

        void set_emulation( bool new_emulation_state );

        analog_emulator& get_emulator();

        /// @brief Получение максимальной длины имени устройства (с учётом 
        /// символа завершения строки).
        static int get_max_name_length()
            {
            return C_MAX_NAME;
            }

        /// @brief Получение максимальной длины описания устройства (с учётом 
        /// символа завершения строки).
        static int get_max_description_length()
            {
            return C_MAX_DESCRIPTION;
            }

    private:
        u_int_4 s_number = 0;        ///< Последовательный номер устройства.

        DEVICE_TYPE     type;        ///< Тип устройства.
        DEVICE_SUB_TYPE sub_type;    ///< Подтип устройства.

        char* article;               ///< Артикул изделия.

        bool is_manual_mode = false; ///< Признак ручного режима.

        char name[ C_MAX_NAME + 1 ];
        char* description;

        bool emulation = false;
        analog_emulator emulator;

        int state = 0;      ///< Состояние устройства.
        float value = .0f;  ///< Значение устройства.

        int prev_error_state = 0; //< Значение ошибки.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый класс для различных дискретных устройств.
class digital_io_device : public device,
    public io_device
    {
    public:
        digital_io_device( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        ~digital_io_device() override = default;

        void print() const override;

        void direct_set_state( int new_state ) override;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговыми входами/выходами.
///
/// Базовый класс для различных аналоговых устройств.
class analog_io_device : public device, public io_device
    {
    public:
        analog_io_device( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        void print() const override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        int save_device_ex( char* buff ) override;

        float get_value() override;
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
        // движение конвейера).
        virtual float get_linear_speed() const;

        /// @brief Получение текущего тока мотора
        virtual float get_amperage() const;
    };
//-----------------------------------------------------------------------------
/// @brief Камера.
///
/// Служит для получения событий о распозновании объекта.
class i_camera
    {
    public:
        virtual ~i_camera() = default;

        /// @brief Получение статуса событий от камеры.
        virtual int get_result( int n = 1 ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым выходом.
///
/// Это может быть управляемый клапан...
class AO1 : public analog_io_device
    {
    public:
        AO1( const char* dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt );

        virtual float get_min_value() const = 0;
        virtual float get_max_value() const = 0;

        float get_value() override;
        void  direct_set_value( float new_value ) override;

    protected:
        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство без привязки к модулям ввода-вывода
class virtual_counter : public device, public i_counter
    {
    public:
        explicit virtual_counter( const char* dev_name );

        int get_state() override;

        void direct_on() override;

        void direct_off() override;

        void direct_set_state( int new_state ) override;

        void pause() override;

        void start() override;

        void reset() override;

        u_int get_quantity() override;

        float get_flow() override;

        /// @brief Получение абсолютного значения счетчика (без учета
        /// состояния паузы).
        u_int get_abs_quantity() override;

        /// @brief Сброс абсолютного значения счетчика.
        void abs_reset() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        void set( u_int new_value, u_int new_abs_value, float flow );

        void eval( u_int read_value, u_int abs_read_value, float read_flow );

        //Lua.
        int save_device_ex( char* buff ) override;

        u_long get_pump_dt() const override;
        float get_min_flow() const override;

        void pause_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) override;
        void start_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) override;

    private:
        float flow_value = 0.f;

        u_int last_read_value = 0;

        u_int abs_value = 0; ///< Абсолютное значение (не становится на паузу).
        u_int abs_last_read_value = 0;

        bool is_first_read = true; ///< Флаг первой установки значения.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI1 : public analog_io_device
    {
    public:
        AI1( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        int get_state() override;

        const char* get_error_description() override;

        virtual int get_params_count() const;

        float get_value() override;

        void  direct_set_value( float new_value ) override;

        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val();

        /// @brief Получение минимального значения выхода устройства.
        virtual float get_min_val();

    protected:
        enum CONSTANTS
            {
            P_ZERO_ADJUST_COEFF = 1,    ///< Сдвиг нуля.

            ADDITIONAL_PARAM_COUNT = 1, ///< Количество параметров.

            C_AI_INDEX = 0,             ///< Индекс канала аналогового входа.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс текущего уровня.
class level : public AI1
    {
    public:
        level( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt );

        int get_volume();
        virtual int calc_volume();

        int save_device_ex( char* buff ) override;

        float get_max_val() override;
        float get_min_val() override;

        int get_params_count() const override;

        int get_start_param_idx() const;

        enum CONSTANTS
            {
            P_ERR = 1,       ///< Аварийное значение уровня.

            LAST_PARAM_IDX,
            };

    private:
        u_int start_param_idx;
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

        enum CMD
            {
            TURN_OFF,

            TURN_ON,

            LIGHTS_OFF,

            GREEN_ON,
            YELLOW_ON,
            RED_ON,
            BLUE_ON,

            GREEN_OFF,
            YELLOW_OFF,
            RED_OFF,
            BLUE_OFF,

            GREEN_NORMAL_BLINK,
            YELLOW_NORMAL_BLINK,
            RED_NORMAL_BLINK,
            BLUE_NORMAL_BLINK,

            GREEN_SLOW_BLINK,
            YELLOW_SLOW_BLINK,
            RED_SLOW_BLINK,
            BLUE_SLOW_BLINK,

            SIREN_ON,
            SIREN_OFF,
            };

        void set_rt_par( u_int idx, float value ) override;

        void direct_set_state( int new_state ) override;
        void direct_off() override;
        void direct_on() override;

        int get_state() override;

        int save_device_ex( char* buff ) override;

#ifdef _MSC_VER
#pragma region Сигнализация о событиях
#endif
        /// @brief Расчет состояния на основе текущих данных от I/O.
        void evaluate_io() override;

        void show_error_exists();

        void show_message_exists();

        void show_batch_is_not_running();

        void show_batch_is_running();

        void show_operation_is_not_running();

        void show_operation_is_running();

        void show_idle();

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

        show_states show_state = show_states::idle;

#ifdef _MSC_VER
#pragma endregion
#endif

        enum class DO_state
            {
            OFF,
            ON
            };

    protected:
        const char* RED_LAMP = "red lamp";
        const char* GREEN_LAMP = "green lamp";
        const char* YELLOW_LAMP = "yellow lamp";
        const char* BLUE_LAMP = "blue lamp";
        const char* SIREN = "siren";

        virtual void process_DO( u_int n, DO_state state, const char* name ) = 0;

        ///Тип мигания (>0 - реализуем сами, 0 - встроенный в сирену).
        int is_const_red = 0;

        int red_lamp_channel;
        int yellow_lamp_channel;
        int green_lamp_channel;
        int blue_lamp_channel;
        int siren_channel;

        enum class CONSTANTS
            {
            NORMAL_BLINK_TIME = 1000 / 2 / 2,                 //2 Гц
            SLOW_BLINK_TIME = (int)( 1000 / 0.5f / 2 ),       //0.5 Гц
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
            STEP step = STEP::off;
            uint32_t start_blink_time = 0;
            uint32_t start_wait_time = 0;
            };

        state_info green;
        state_info yellow;
        state_info red;
        state_info blue;

        void blink( int lamp_DO, state_info& info, u_int delay_time );

        STEP siren_step = STEP::off;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с интерфейсом подключения IO-Link.
class io_link_device
    {        
    public:
        const char* get_error_description( int error_id ) const;
    };
