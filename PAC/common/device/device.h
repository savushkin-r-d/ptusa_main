/// @file device.h
/// @brief Классы, которые реализуют функции передачи
/// состояния устройств PAC на сервер. Также реализованы классы для работы с
/// устройствами ( клапана, насосы,... ). Классы используются ТОЛЬКО в
/// контроллере ( PAC ).
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include "base.h"
#include "valve.h"

#include <string.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <memory>

#include "smart_ptr.h"

#include "dtime.h"
#include "g_device.h"

#include "param_ex.h"

#include "PAC_info.h"

#include "iot_altivar.h"
#include "iot_wages_eth.h"
#include "log.h"


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

        int  get_state() override;
        void direct_on() override;
        void direct_off() override;

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Температура.
class temperature_e : public AI1
    {
    public:
        explicit temperature_e( const char* dev_name );

        float get_value() override;

        int get_state() override;

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
        explicit temperature_e_analog( const char* dev_name );

        float get_value() override;

        int get_state() override;

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
        explicit temperature_e_iolink( const char *dev_name );

        ~temperature_e_iolink() override;

        float get_value() override;

    private:
        struct TE_data
            {
            int16_t v = 0;
            };

        TE_data *info = new TE_data();
		u_int start_param_idx;
        
		enum CONSTANTS
			{
			P_ERR_T = 1,                ///< Аварийное значение температуры.

			ADDITIONAL_PARAM_COUNT = 1, ///< Количество параметров.
			};
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень.
class level_e : public level
    {
    public:
        explicit level_e( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень c для танка цилиндрической формы.
class level_e_cyl : public level
    {
    public:
        explicit level_e_cyl( const char* dev_name );

        int calc_volume() override;

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
        explicit level_e_cone( const char* dev_name );

         int calc_volume() override;

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
        explicit pressure_e( const char* dev_name );

        float get_max_val() override;
        float get_min_val() override;

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
        explicit pressure_e_iolink( const char* dev_name );

        float get_value() override;

        int get_state() override;

        void set_article( const char* new_article ) override;

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
            const device* dev  );

        void evaluate_io() override;

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
        ARTICLE n_article = ARTICLE::DEFAULT;

        enum CONSTANTS
            {
            C_AI_INDEX = 0,     ///< Индекс канала аналогового входа.

            P_ERR,              ///< Аварийное значение.

            LAST_PARAM_IDX,
            };

        float v = .0f;
        int st = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Автоматический выключатель.
class circuit_breaker : public analog_io_device
    {
    public:
        explicit circuit_breaker(const char* dev_name);

        int save_device_ex(char* buff) override;

        int set_cmd(const char* prop, u_int idx, double val) override;

        void direct_set_value(float v) override;

        void direct_on() override;

        void direct_off() override;

        float get_value() override;

        int get_state() override;

        void evaluate_io() override;

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

        bool is_read_OK = false;

        float v = .0f;
        int st = 0;
        int err = 0;
        int m = 0;

        F_data_in in_info{};
        static F_data_out stub_out_info;
        F_data_out* out_info = &stub_out_info;
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня IO-Link.
class level_e_iolink : public level
    {
    public:
        explicit level_e_iolink( const char* dev_name );

        int calc_volume() override;

        float get_value() override;
        int get_state() override;

        void set_article( const char* new_article ) override;
        void evaluate_io() override;

        void set_string_property(const char* field, const char* value) override;

    private:
        pressure_e_iolink::ARTICLE n_article = pressure_e_iolink::ARTICLE::DEFAULT;

    private:
        enum CONSTANTS
            {
            P_MAX_P = 1, ///< Индекс параметра давление настройки датчика (бар).
            P_R,         ///< Индекс параметра радиуса танка (м).
            P_H_CONE,    ///< Индекс параметра высоты конуса танка (м).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;

        int st = 0;
        float v = .0f;

        i_AI_device* PT_extra = nullptr;
    };
//-----------------------------------------------------------------------------
/// @brief Концентрация.
class concentration_e : public AI1
    {
    public:
        concentration_e( const char* dev_name, DEVICE_SUB_TYPE sub_type );

        float get_max_val() override;
        float get_min_val() override;

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
        explicit concentration_e_ok( const char* dev_name );

        int get_state() override;

        int save_device_ex( char* buff ) override;

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
        explicit concentration_e_iolink(const char* dev_name);

        ~concentration_e_iolink() override;

        int save_device_ex( char *buff ) override;

        float get_temperature() const;

        float get_value() override;

        int get_state() override;

        void evaluate_io() override;

    private:

#pragma pack(push,1)
        struct QT_data
            {
            unsigned char              :4;
            unsigned char status       :4;
            unsigned char tmp;
            unsigned int  temperature = 0;
            unsigned char tmp1;
            unsigned char tmp2;
            unsigned int  conductivity;
            };
#pragma pack(pop)

        QT_data* info = new QT_data();

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
        explicit analog_input( const char* dev_name );

        float get_max_val() override;
        float get_min_val() override;

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
        explicit virtual_wages( const char* dev_name );

        void direct_off() override;

        float get_value() override;

        void direct_set_value(float new_value) override;

        void direct_set_state( int new_state ) override;

        void direct_on() override;

        int get_state() override;

        void tare() override;

    private:
        float value = .0f;
        int state = 0;
    };
//-----------------------------------------------------------------------------
class wages_RS232 : public analog_io_device, public i_wages
    {
    public:
        explicit wages_RS232( const char* dev_name );

        float get_value() override;

        int get_state() override;

        void evaluate_io() override;

        void direct_set_value( float new_value ) override;

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

        int state = 1;
        float value = .0f;
    };
//-----------------------------------------------------------------------------
class wages_eth : public analog_io_device, public i_wages
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
class wages_pxc_axl : public analog_io_device, public i_wages
    {
    public:
        explicit wages_pxc_axl( const char* dev_name );

        void evaluate_io() override;

        void tare() override;
        void reset_tare();

        float get_value() override;

        int get_state() override;

        void direct_set_state( int new_state ) override;

        void direct_set_value( float new_value ) override;

        enum class ERR_VALUES : unsigned int
            {
            ERR_OVERRANGE = 0x80000001, // Measuring range exceeded (overrange).
            ERR_WIRE_BREAK = 0x80000002,            // Wire break.
            ERR_SHORT_CIRCUIT = 0x80000003,         // Short-circuit.
            ERR_INVALID_VALUE = 0x80000004,         // Measured value is invalid.
            ERR_FAULTY_SUPPLY_VOLTAGE = 0x80000020, // Faulty supply voltage.
            ERR_FAULTY_DEVICE = 0x80000040,         // Device faulty.
            ERR_UNDERRANGE = 0x80000080 // Below measuring range (underrange).
            };

        enum class ERR_STATES
            {
            ERR_OVERRANGE = -1,     // Measuring range exceeded (overrange).
            ERR_WIRE_BREAK = -2,            // Wire break.
            ERR_SHORT_CIRCUIT = -3,         // Short-circuit.
            ERR_INVALID_VALUE = -4,         // Measured value is invalid.
            ERR_FAULTY_SUPPLY_VOLTAGE = -5, // Faulty supply voltage.
            ERR_FAULTY_DEVICE = -6,         // Device faulty.
            ERR_UNDERRANGE = -7     // Below measuring range (underrange).
            };

        enum class IO_CMDS
            {
            TARE_BIT_IDX       = 12 - 8,
            RESET_TARE_BIT_IDX = 13 - 8,

            S_TARE = 1,
            };

        enum class CMDS
            {
            TARE = 1,
            RESET_TARE = 2,
            };

        enum class CONSTANTS
            {
            C_AIAO_INDEX = 0,   ///< Индекс канала аналоговых данных.

            C_TARE_TIME = 5000, ///< Время ожидания установления тары.

            P_DT = 1,       ///< Пороговый фильтр времени.
            P_CZ,           ///< Сдвиг нуля.
            P_K,            ///< Коэффициент пропорциональности.
            LAST_PARAM_IDX,
            };
    
    private:
        float w = .0f;
        int st = 0;

        unsigned long tare_time = 0;
        unsigned long reset_tare_time = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Датчик веса
class wages : public analog_io_device, public i_wages
    {
    public:
        explicit wages( const char *dev_name);

        void tare() override;
        float get_weight();

        float get_value() override;

        void direct_set_state( int new_state ) override;

        void direct_set_value( float new_value ) override;

        int get_state() override;

        int save_device_ex( char* buff ) override;

    private:
        float weight = 0.f;
        unsigned long filter_time = get_millisec();

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
    };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство без привязки к модулям ввода-вывода
class virtual_device : public device
    {
    public:
        virtual_device( const char *dev_name, device::DEVICE_TYPE dev_type,
            device::DEVICE_SUB_TYPE dev_sub_type);

        void direct_off() override;

        void direct_set_value( float new_value ) override;

        float get_value() override;

        void direct_set_state( int new_state ) override;

        void direct_on() override;

        int get_state() override;

        bool is_active() override;

        void set_rt_par(unsigned int idx, float value) override;

    private:
        float value = .0f;
        int state = 0;
        bool level_logic_invert = false;
     };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть управляемый клапан...
class analog_output : public AO1
    {
    public:
        explicit analog_output( const char* dev_name );

        float get_min_value() const override;

        float get_max_value() const override;

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 2,

            P_MIN_VALUE = 1,   ///< Индекс параметра минимального значения.
            P_MAX_VALUE,       ///< Индекс параметра максимального значения.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным входом.
///
/// Это может быть обратная связь, расход (есть/нет)...
class DI1 : public digital_io_device
    {
    public:
        DI1( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt,
            int current_state_init_val = 0 );

        void direct_on() override;

        void direct_off() override;

        int get_state() override;

    private:
        int current_state;
        u_int_4 time = 0;

        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< Индекс канала дискретного входа.

            ADDITIONAL_PARAMS_COUNT = 1,

            P_DT = 1,
            };
    };
//-----------------------------------------------------------------------------
class virtual_motor : public i_motor
    {
    public:
        explicit virtual_motor( const char* dev_name );

        void direct_off() override;

        void direct_set_value( float new_value ) override;

        float get_value() override;

        void direct_set_state( int new_state ) override;

        void direct_on() override;

        int get_state() override;

    private:
        float value = .0f;
        int state = 0;

    };
//-----------------------------------------------------------------------------
/// @brief Электродвигатель (мешалка, насос).
class motor : public i_motor, public io_device
    {
    public:
        motor( const char* dev_name, device::DEVICE_SUB_TYPE sub_type );

        int save_device_ex( char *buff ) override;

        float get_value()  override;

        void direct_set_value( float new_value ) override;

        void direct_set_state( int new_state ) override;

        int get_state() override;

        void direct_on() override;

        void direct_off() override;

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

        u_long start_switch_time = get_millisec();
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

    int save_device_ex(char *buff) override;

    float get_value() override;

    void direct_set_value(float value) override;

    void direct_set_state(int new_state) override;

    int  get_state() override;

    void direct_on() override;

    void direct_off() override;

    void set_string_property(const char* field, const char* value) override;

    void print() const override;

    virtual int get_params_count() const;

    float get_amperage() const override;

    int set_cmd( const char* prop, u_int idx, double val ) override;

    float get_rpm() const
        {
        return (float)rpm;
        }

    altivar_node* get_atv() const
        {
        return atv;
        }

private:
    altivar_node* atv = nullptr;

    float freq = .0f;
    int reverse = 0;
    int rpm = 0;
    int est = 0;
    float amperage = .0f;

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

    u_long start_switch_time = get_millisec();
};
//-----------------------------------------------------------------------------
/// @brief Электродвигатель, управляемый частотным преобразователем altivar с
/// интерфейсной платой Ethernet c расчетом линейной скорости.
class motor_altivar_linear : public motor_altivar
    {
    public:
        explicit motor_altivar_linear( const char* dev_name );

        /// @brief Получение линейной скорости.
        float get_linear_speed() const override;

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

        bool is_active() override;
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня IO-Link.
class level_s_iolink : public analog_io_device
    {
    public:
        level_s_iolink( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

        float get_value() override;

        int get_state() override;

        bool is_active() override;

        void evaluate_io() override;

        void set_article( const char* new_article ) override;

    private:
        int current_state;
        u_int_4 time = get_millisec();

        enum ARTICLE
            {
            DEFAULT,
            IFM_LMT100,
            IFM_LMT102,
            IFM_LMT104,
            IFM_LMT105,

            EH_FTL33,
            };
        ARTICLE n_article = ARTICLE::DEFAULT;

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

        float v = .0f;
        int st = 0;

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
        explicit flow_s( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Датчик положения.
class state_s : public DI1
    {
    public:
        explicit state_s( const char *dev_name ): DI1( dev_name, DT_GS, DST_GS,
            0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик положения инверсный.
class state_s_inverse : public DI1
    {
    public:
        explicit state_s_inverse( const char* dev_name ) : DI1( dev_name, DT_GS,
            DST_GS_INVERSE, 0, 1 )
            {
            }

        bool is_active() override
            {
            return get_state() == 0 ? true : false;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик разности давления.
class diff_pressure : public DI1
    {
    public:
        explicit diff_pressure( const char* dev_name ) : DI1( dev_name, DT_PDS,
            DST_PDS_VIRT, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Сигнальный датчик температуры.
class temperature_signal : public DI1
    {
    public:
        explicit temperature_signal( const char* dev_name ) : DI1( dev_name, DT_TS,
            DST_TS_VIRT, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик дискретного входа связи.
class DI_signal : public DI1
    {
    public:
        explicit DI_signal( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Кнопка.
class button : public DI1
    {
    public:
        explicit button( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Дискретный сигнал управления.
class DO_signal : public DO1
    {
    public:
        explicit DO_signal( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Аварийная звуковая сигнализация.
class siren : public DO1
    {
    public:
        explicit siren( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Аварийная световая сигнализация.
class lamp : public DO1
    {
    public:
        explicit lamp( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief Базовый счетчик.
class base_counter: public i_counter, public device, public io_device
    {
    public:
        base_counter( const char* dev_name, DEVICE_SUB_TYPE sub_type,
            int extra_par_cnt );

        void evaluate_io() override;

        void print() const override;

        /// @brief Приостановка работы счетчика.
        void pause() override;

        /// @brief Возобновление работы счетчика.
        void start() override;

        /// @brief Сброс счетчика и остановка счета.
        ///
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        void reset() override;

        /// @brief Сброс абсолютного значения счетчика.
        void abs_reset() override;

        /// @brief Сброс счетчика и продолжение счета.
        void restart() override;

        /// @brief Получение состояния работы счетчика.
        int get_state() override;

        void direct_on() override;

        void direct_off() override;

        float get_value() override;

        void direct_set_state( int new_state ) override;

        void direct_set_value( float new_value ) override;

        void set_property( const char* field, device* dev ) override;

        /// @brief Получение значение счетчика от устройства.
        virtual float get_raw_value() const = 0;

        /// @brief Получение максимального значение счетчика от устройства.
        virtual float get_max_raw_value() const = 0;

        float calculate_quantity( float& value, float& last_read_value,
            bool& is_first_read ) const;

        /// @brief Получение значения счетчика (c учетом паузы).
        u_int get_quantity() override;

        /// @brief Получение абсолютного значения счетчика (без учета паузы).
        u_int get_abs_quantity() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        int save_device_ex( char* buff ) override;

        const char* get_error_description() override;

        /// @brief Приостановка работы дневного счетчика.
        void pause_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) override;

        /// @brief Возобновление работы дневного счетчика.
        void start_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) override;

    protected:
        float get_abs_value() const
            {
            return abs_value;
            }

    private:
        void set_abs_value( float new_value );

        const int MAX_OVERFLOW = 300;   ///< Максимальное переполнение за цикл.

        STATES prev_error_state = STATES::S_WORK;

        u_int_4 start_pump_working_time = 0;
        u_int_4 counter_prev_value = 0;

        std::vector < device* > motors;

        bool is_first_read = true;      ///< Флаг первой установки значения.
        float value = .0f;
        float last_read_value = 0.f;

        bool abs_is_first_read = true;
        float abs_value = 0.f;  ///< Абсолютное значение (не становится на паузу).
        float abs_last_read_value = 0.f;

        STATES day_t1_state = STATES::S_WORK;
        float day_t1_value = .0f;               // Текущее значение за день.
        float prev_day_t1_value = .0f;
        STATES day_t2_state = STATES::S_WORK;
        float day_t2_value = .0f;
        float prev_day_t2_value = .0f;

        int c_day = get_time().tm_yday;         // Текущий день.
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
        explicit counter_f( const char *dev_name );

        int get_state() override;

        float get_flow() override;

        int save_device_ex( char* buff ) override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

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
/// @brief Счетчик IO-Link.
class counter_iolink : public base_counter
    {
    public:
        explicit counter_iolink( const char* dev_name );

        void evaluate_io() override;

        float get_temperature() const;

        int save_device_ex( char* buff ) override;

        int get_state() override;

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
        explicit signal_column_iolink( const char* dev_name );

        void set_string_property( const char* field, const char* value ) override;

        void evaluate_io() override;
    
    private:
        void process_DO( u_int n, DO_state state, const char* name ) override;

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
class camera : public i_camera, public device, public io_device
    {
    public:
        camera( const char* dev_name, DEVICE_SUB_TYPE sub_type,
            int params_count = 0, bool is_ready = true );

        void direct_set_state( int new_state ) override;

        void direct_off() override;

        void direct_on() override;

        int save_device_ex( char* buff ) override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

        void set_string_property( const char* field, const char* value ) override;

        int get_result( int n = 1 ) override;

        /// @brief Получение состояние готовности.
        virtual bool is_ready() const;

    protected:
        bool is_cam_ready;
        int result = 0;

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

        void evaluate_io() override;

    protected:
        u_int start_switch_time = get_millisec();

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
        explicit camera_DI3( const char* dev_name );

        void evaluate_io() override;

        int get_result( int n = 1 ) override;

    private:
        int result_2 = 0;
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

        u_int_4 get_serial_n() const override;
        void    print() const override;

        float get_value() override;
        void direct_set_value( float new_value ) override;

        void off() override;
        void on() override;

        void direct_on() override;
        void direct_off() override;
        void direct_set_state( int new_state ) override;

        void set_value( float new_value ) override;
        void set_state( int new_state ) override;

        int get_state() override;

        VALVE_STATE get_valve_state() override;

        void    pause() override;
        void    start() override;
        void    reset() override;
        u_int   get_quantity() override;
        float   get_flow() override;

        u_long get_pump_dt() const override;
        float get_min_flow() const override;

        u_int get_abs_quantity() override;
        void  abs_reset() override;

        void pause_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) override;
        void start_daily( DAY_CTR n = DAY_CTR::DAY_T1 ) override;

        void tare() override;

        void process_DO( u_int n, DO_state state, const char* name ) override;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class threshold_regulator :public device, public i_Lua_save_device
    {
    public:
        enum class PARAM
            {
            P_is_reverse = 1,	        ///Обратного (реверсивного) действия.
            P_delta,

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
/// @brief Блок питания.
class power_unit : public analog_io_device
    {
    public:
        power_unit( const char* dev_name, 
            device::DEVICE_SUB_TYPE sub_type = device::DEVICE_SUB_TYPE::DST_G_IOL_4 );

        void direct_on() override;
        void direct_off() override;

        float get_value() override;
        int get_state() override;

        void direct_set_value( float val ) override;

        void evaluate_io() override;

        int save_device_ex( char* buff ) override;

        int set_cmd( const char* prop, u_int idx, double val ) override;

#ifdef PTUSA_TEST
        void set_cmd_time( unsigned long t )
            {
            cmd_time = t;
            }

        static unsigned int WAIT_DATA_TIME; // Ожидание записи данных, мс.
        static unsigned int WAIT_CMD_TIME;  // Ожидание записи команды, мс.
#endif

    private:
        /// Синхронизация выходной области на основе данных из входной области.
        void sync_pdout();

        float v = .0f;  // Sum of output currents.
        int st = 0;     // Status - хотя бы один из каналов активен.
        int err = 0;    // DC status.

        enum CONSTANTS
            {
            C_AIAO_INDEX = 0,   ///< Индекс канала аналоговых данных.
            };
        
        bool is_processing_cmd = false;
        unsigned long cmd_time = 0;

#pragma pack(push, 1)
        struct process_data_in
            {
            uint8_t out_voltage_2 : 2;          // Byte 0.
            bool                  : 1;
            bool out_power_90     : 1;
            bool DC_not_OK        : 1;
            uint8_t               : 0;

            uint8_t out_voltage;                // Byte 1.

            uint8_t sum_currents_2 : 2;         // Byte 2.
            uint8_t                : 0;
            uint8_t sum_currents   : 8;         // Byte 3.

            int8_t status_ch4 : 2;              // Byte 4.
            int8_t status_ch3 : 2;
            int8_t status_ch2 : 2;
            int8_t status_ch1 : 2;
            int8_t status_ch8 : 2;              // Byte 5.
            int8_t status_ch7 : 2;     
            int8_t status_ch6 : 2;
            int8_t status_ch5 : 2;

            uint8_t nominal_current_ch2 : 3;    // Byte 6.
            uint8_t nominal_current_ch1 : 3;
            uint8_t                     : 2;
            uint8_t nominal_current_ch4 : 3;    // Byte 7.
            uint8_t nominal_current_ch3 : 3;
            uint8_t                     : 2;
            uint8_t nominal_current_ch6 : 3;    // Byte 8.
            uint8_t nominal_current_ch5 : 3;
            uint8_t                     : 2;
            uint8_t nominal_current_ch8 : 3;    // Byte 9.
            uint8_t nominal_current_ch7 : 3;
            uint8_t                     : 2;    

            uint8_t load_current_ch1 : 8;       // Byte 10.
            uint8_t load_current_ch2 : 8;       // Byte 11.
            uint8_t load_current_ch3 : 8;       // Byte 12.
            uint8_t load_current_ch4 : 8;       // Byte 13.
            uint8_t load_current_ch5 : 8;       // Byte 14.
            uint8_t load_current_ch6 : 8;       // Byte 15.
            uint8_t load_current_ch7 : 8;       // Byte 16.
            uint8_t load_current_ch8 : 8;       // Byte 17.
            };
#pragma pack(pop)

#pragma pack(push, 1)
        struct process_data_out
            {
            bool channel_status_after_reset : 1;    // Byte 0.
            uint8_t                         : 6;
            bool valid_flag                 : 1;

            uint8_t nominal_current_ch2 : 3;        // Byte 1.
            uint8_t nominal_current_ch1 : 3;
            uint8_t                     : 0;
            uint8_t nominal_current_ch4 : 3;        // Byte 2.
            uint8_t nominal_current_ch3 : 3;
            uint8_t                     : 0;
            uint8_t nominal_current_ch6 : 3;        // Byte 3.
            uint8_t nominal_current_ch5 : 3;
            uint8_t                     : 0;
            uint8_t nominal_current_ch8 : 3;        // Byte 4.
            uint8_t nominal_current_ch7 : 3;
            uint8_t                     : 0;

            bool prioritization : 1;                // Byte 5.
            uint8_t             : 0;

            bool switch_ch8 : 1;                    // Byte 6.
            bool switch_ch7 : 1;
            bool switch_ch6 : 1;
            bool switch_ch5 : 1;
            bool switch_ch4 : 1;
            bool switch_ch3 : 1;
            bool switch_ch2 : 1;
            bool switch_ch1 : 1;
            };
#pragma pack(pop)

#ifndef PTUSA_TEST
        static unsigned int WAIT_DATA_TIME; // Ожидание записи данных, мс.
        static unsigned int WAIT_CMD_TIME;  // Ожидание записи команды, мс.
#endif

        process_data_in p_data_in;

        static process_data_out stub_p_data_out;
        process_data_out* p_data_out = &stub_p_data_out;
    };
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
        u_long  last_time = 0;          ///< Время, когда таймер был запущен/остановлен.
        u_long  work_time = 0;          ///< Время работы таймера.

        STATE   state = STATE::S_STOP;  ///< Состояние.
        u_long  countdown_time = 0;     ///< Задание таймера.
    };
//-----------------------------------------------------------------------------
/// @brief таймер.
///
/// Реализация группы таймеров.
class timer_manager
    {
    public:
        /// @param timers_count - количество таймеров в группе.
        explicit timer_manager( u_int timers_count );

        ~timer_manager();

        /// @brief Безопасное получение таймера по индексу.
        ///
        /// @param index - индекс таймера.
        ///
        /// @return - таймер с нужным индексом, заглушка - в случае выхода за
        /// диапазон.
        timer* operator[] ( unsigned int index );

        /// @brief Количество таймеров.
        int get_count() const;

    private:
        u_int   timers_cnt;         ///< Количество таймеров.
        timer   *timers = nullptr;  ///< Таймеры.

        timer   stub;               ///< Заглушка.
    };

#endif // PAC_DEVICES_H
