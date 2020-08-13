/// @file PAC_dev.h
/// @brief ������, ������� ��������� ������� ��������
/// ��������� ��������� PAC �� ������. ����� ����������� ������ ��� ������ �
/// ������������ ( �������, ������,... ). ������ ������������ ������ �
/// ����������� ( PAC ).
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c DEBUG_NO_IO_MODULES - ������� ���������� �������� ��� �������
/// �����\������ (��������� ������ � ����).
///
/// @par ������� ������:
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

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "smart_ptr.h"

#include "dtime.h"
#include "bus_coupler_io.h"
#include "g_device.h"

#include "param_ex.h"

#include "PAC_info.h"

#include "iot_altivar.h"
#include "log.h"

//-----------------------------------------------------------------------------
/// @brief ���������� c �����������.
///
/// ��������� �������� � ����������������� ������ (��������� �������� �����
/// ������������ PAC). ������ � ���������� ������������ �� ������ ������ �
/// �������� (�������� ������ ����� �� �����). ������ �������� ����� ���.
class par_device
    {
    friend class device;

    public:
        /// @param par_cnt - ���������� ����������.
        par_device( u_int par_cnt );

        virtual ~par_device();

        /// @brief ���������� � ���� ������� Lua.
        ///
        /// ��������� �� ������ �������� ���������� � ���� �������:
        /// ���_��������� = '��������'.
        ///
        /// @param str - ������, ���� ���������.
        int save_device( char *str );

        /// @brief ���������� ������� (��������� �������� ���������).
        ///
        /// @param name - ��� ������� (��������������� ���������).
        /// @param val - ����� ��������.
        ///
        /// @return 0 - ��.
        /// @return 1 - ������, �������� � ����� ������ �� ������.
        int set_cmd( const char *name, double val );

        /// @brief ��������� �������� ���������.
        ///
        /// @param idx - ������ ��������� (� �������).
        /// @param offset - �������� �������.
        /// @param value - ����� ��������.
        virtual void set_par( u_int idx, u_int offset, float value );

        /// @brief ��������� �������� �������� ���������.
        ///
        /// @param idx - ������ �������� ��������� (� �������).
        /// @param value - ����� ��������.
        virtual void set_rt_par( u_int idx, float value )
            {
            }

        /// @brief ��������� �������� ���������.
        ///
        /// @param idx - ������ ���������.
        /// @param offset - �������� �������.
        ///
        /// @return �������� ���������.
        float get_par( u_int idx, u_int offset );

        /// @brief ������� ����� ���������.
        ///
        /// @param idx - ������ ���������.
        /// @param offset - �������� �������.
        /// @param name - ��� ���������.
        void set_par_name( u_int idx, u_int offset, const char* name );

        /// @brief ��������� ���������� ��� �������� �������� ������ ����������.
        saved_params_u_int_4* get_err_par() const
            {
            return err_par;
            }

        /// @brief ��������� ���������� ��� �������� �������� ������ ����������.
        void set_err_par( saved_params_u_int_4* err_par )
            {
            this->err_par = err_par;
            }

    protected:
        /// @brief ������ ����������.
        saved_params_u_int_4 *err_par;

        enum CONSTANTS
            {
            C_MAX_PAR_NAME_LENGTH = 20, ///< ������������ ����� ����� ���������.
            };

        saved_params_float *par; ///< ���������.
        char **par_name;         ///< �������� ����������.
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ��������.
class i_counter
    {
    public:
        /// @brief ������������ ������ ��������.
        virtual void pause() = 0;

        /// @brief ������������� ������ ��������.
        virtual void start() = 0;

        /// @brief ����� �������� � ��������� �����.
        ///
        /// ����� ������ ��� ����������� ������ ���������� ������� @ref start().
        virtual void reset() = 0;

        /// @brief ����� �������� � ����������� �����.
        void restart();

        /// @brief ��������� �������� ��������.
        virtual u_int get_quantity() = 0;

        /// @brief ��������� �������� ��������.
        virtual float get_flow() = 0;

        /// @brief ��������� ��������� ������ ��������.
        virtual int get_state() = 0;

        /// @brief ��������� ����������� �������� �������� (��� �����
        /// ��������� �����).
        virtual u_int get_abs_quantity() = 0;

        /// @brief ����� ����������� �������� ��������.
        virtual void  abs_reset() = 0;

        virtual ~i_counter();

        enum STATES
            {
            S_STOP,
            S_WORK,
            S_PAUSE,

            S_ERROR = -1,

            S_LOW_ERR = -2,
            S_HI_ERR = -3,
            };
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ������������������� ������� (mixproof).
class i_mix_proof
    {
    public:

        /// @brief ������� ������� �����.
        virtual void open_upper_seat() = 0;

        /// @brief ������� ������ �����.
        virtual void open_lower_seat() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ��������� �����.
class i_wages
    {
    public:
        /// @brief ���������.
        virtual void tare() = 0;
        ///@brief ���������� ��� � �����������
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ����������� �����.
///
/// �������� �����, ���������� ������� � �.�. �������� ��������� �����
/// ���������.
class i_DI_device: public i_cmd_device
    {
    public:
        /// @brief ��������� ��������� ����������.
        ///
        /// @return ��������� ���������� � ���� ������ �����.
        virtual int get_state () = 0;

        /// @brief �������� ��������� ���������.
        ///
        /// ��� ������ - �������� �� ��� ���.
        ///
        /// @return 1 - ���������� �������.
        /// @return 0 - ���������� ���������.
        virtual bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ����������� ������.
///
/// ������, ������� � �.�. �������� ��������� ����� ���������.
class i_DO_device: public i_DI_device
    {
    public:
        /// @brief ��������� ���������� � ������ ������� ������.
        virtual void on();

        virtual void print() const = 0;

        /// @brief ���������� ���������� � ������ ������� ������.
        virtual void off() = 0;

        /// @brief ����������� ���������� ����������
        virtual void direct_off() = 0;

        /// @brief ����������� ���������� ���������� c ������ ������� ������
        virtual void instant_off();

        /// @brief ��������� ������ ��������� ���������� � ������ ������� ������.
        ///
        /// @param new_state - ����� ��������� ����������.
        virtual void set_state( int new_state );

    protected:
        /// @brief ��������� ������ ��������� ����������.
        ///
        /// @param new_state - ����� ��������� �������.
        virtual void direct_set_state( int new_state ) = 0;

        /// @brief ��������� ����������.
        ///
        /// ��������� ���������� � �������� ���������. ��� ������� ��� ��������
        /// ��� �������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void direct_on() = 0;

        /// @brief ������� �� ������ �����.
        ///
        /// � ������ ������ ���������� ����������� �� �������� �� �������
        /// (����������� ��������� �� ������ �� ����������).
        ///
        /// @return true - ������ ����� �������.
        /// @return false - ������ ����� ��������.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� �� ������ ����������� �����.
///
/// �����������, ������ � �.�. �������� ��������� ����� ���������.
class i_AI_device: public i_cmd_device
    {
    public:
        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� �������� �����.
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ����������� ������.
///
/// ���������� ����� ���������� � �.�. �������� ��������� ����� ���������.
class i_AO_device: public i_AI_device
    {
    public:
        /// @brief ���������� ���������� � ������ ������� ������.
        virtual void off() = 0;

        /// @brief ��������� �������� ��������� ���������� � ������ ������� ������.
        virtual void set_value( float new_value );

    protected:
        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @param new_value - ����� ��������� ����������.
        virtual void direct_set_value( float new_value ) = 0;

        /// @brief ������� �� ������ �����.
        ///
        /// � ������ ������ ���������� ����������� �� �������� �� �������
        /// (����������� ��������� �� ������ �� ����������).
        ///
        /// @return true - ������ ����� �������.
        /// @return false - ������ ����� ��������.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ���������� ��� � �����������, ��� � ����������� ��������.
class i_DO_AO_device: public i_AO_device, public i_DO_device
    {
    public:
        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void off() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ����� �������������� �������� ����������, ������� ������������ �
/// �������.
class device : public i_DO_AO_device, public par_device
    {
    public:
        /// @brief ���������� �������.
        ///
        /// ������ � �������� ������� �������� �����.
        ///
        /// @param prop - �������� �������.
        /// @param idx  - ������ ��������.
        /// @param val  - ��������.
        virtual int set_cmd( const char *prop, u_int idx, double val );

        /// @brief ���������� �������.
        ///
        /// ������ � �������� ������� ������.
        ///
        /// @param prop - �������� �������.
        /// @param idx  - ������ ��������.
        /// @param val  - ��������.
        virtual int set_cmd( const char *prop, u_int idx, char *val );

        /// @brief ���������� ���������� � ���� ������� Lua.
        ///
        /// @param prefix - ������� ����� ������� ������� (������ ������
        /// ��������� - ��� ����������� �������������� ������).
        /// @param buff [out] - ����� ������ ������.
        virtual int save_device( char *buff, const char *prefix );

#ifdef RM_PAC
        /// @brief ���������� ��������� ���������� � ���� ������� Lua.
        ///
        /// @param prefix - ������� ����� ������� ������� (������ ��������
        /// �������).
        /// @param buff [out] - ����� ������ ������.
        virtual int rm_save_device_state( char *buff, const char *prefix );
#endif // RM_PAC

        /// @brief ������ ��������� �� ������ ������� ������ �� I/O.
        virtual void evaluate_io()
            {
            }

        enum CONSTANTS
            {     
            C_MAX_NAME = 20
            };

        static const char DEV_NAMES[][ 5 ];

        /// ���� ���������.
        enum DEVICE_TYPE
            {
            DT_NONE = -1,      ///< ��� �� ���������.

            DT_V = 0,   ///< ������.
            DT_VC,      ///< ����������� ������.
            DT_M,       ///< ���������.
            DT_LS,      ///< ������� (����/���).
            DT_TE,      ///< �����������.
            DT_FS,      ///< ������ (����/���).
            DT_GS,      ///< ������ ���������.
            DT_FQT,     ///< �������.
            DT_LT,      ///< ������� (��������).
            DT_QT,      ///< ������������.

            DT_HA,      ///< ��������� �������� ������������.
            DT_HL,      ///< ��������� �������� ������������.
            DT_SB,      ///< ������.
            DT_DI,      ///< ���������� ������� ������.
            DT_DO,      ///< ���������� �������� ������.
            DT_AI,      ///< ���������� ������� ������.
            DT_AO,      ///< ���������� �������� ������.
            DT_WT,      ///< �������������.
            DT_PT,      ///< �������� (��������).
            DT_F,       ///< �������������� �����������.

            C_DEVICE_TYPE_CNT, ///< ���������� ����� ���������.
            };

        /// ������� ���������.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = -1,      ///< ������ �� ���������.

            //V
            DST_V_DO1 = 1,         ///< ������ � ����� ������� ����������.
            DST_V_DO2,             ///< ������ � ����� �������� ����������.
            DST_V_DO1_DI1_FB_OFF,  ///< ������ � ����� ������� ���������� � ����� �������� ������ �� �������� ���������.
            DST_V_DO1_DI1_FB_ON,   ///< ������ � ����� ������� ���������� � ����� �������� ������ �� �������� ���������.
            DST_V_DO1_DI2,         ///< ������ � ����� ������� ���������� � ����� ��������� �������.
            DST_V_DO2_DI2,         ///< ������ � ����� �������� ���������� � ����� ��������� �������.
            DST_V_MIXPROOF,        ///< ������ ������������������.
            DST_V_AS_MIXPROOF,     ///< ������ � ����� �������� ���������� � ����� ��������� ������� � AS ����������� (������������������).
            DST_V_BOTTOM_MIXPROOF, ///< ������ ������.
            DST_V_AS_DO1_DI2,      ///< ������ � ����� ������� ���������� � ����� ��������� ������� � AS �����������.
            V_DO2_DI2_BISTABLE,    ///< ������ � ����� �������� ���������� � ����� ��������� ������� ������������.

            V_IOLINK_VTUG_DO1,     ///< IO-Link VTUG8 ������ � ����� ������� ����������.

            V_IOLINK_VTUG_DO1_FB_OFF, ///< IO-Link VTUG ������ � ����� ������� ���������� � ����� �������� ������ (����������� ���������).
            V_IOLINK_VTUG_DO1_FB_ON,  ///< IO-Link VTUG ������ � ����� ������� ���������� � ����� �������� ������ (���������� ���������).

            V_IOLINK_MIXPROOF,        ///< ������ � ����� �������� ���������� � ����� ��������� ������� � IO-Link ����������� (������������������).
            V_IOLINK_DO1_DI2,         ///< ������ � ����� ������� ���������� � ����� ��������� ������� � IO-Link ����������� (��������).

            //LS
            DST_LS_MIN = 1,     ///< ����������� �� ����� �������.
            DST_LS_MAX,         ///< ����������� �� ����� ��������.

            LS_IOLINK_MIN,      ///< IOLInk �������. ����������� �� ����� �������.
            LS_IOLINK_MAX,      ///< IOLInk �������. ����������� �� ����� ��������.

            DST_LS_VIRT,        ///< ����������� ������ ������.

            //TE
            DST_TE = 1,
            DST_TE_IOLINK,      ///����������� IO-LInk ��� �������������� ����������.

            //M,
            DST_M = 1,          ///< ����� ��� ���������� �������� ��������.
            DST_M_FREQ,         ///< ����� � ����������� �������� ��������.

            DST_M_REV,          ///< ����� � �������� ��� ���������� �������� ��������. ������ ���������� ���������.
            DST_M_REV_FREQ,     ///< ����� � �������� � ����������� �������� ��������. ������ ���������� ���������.
            DST_M_REV_2,        ///< ����� � �������� ��� ���������� �������� ��������. ������ ���������� ��������.
            DST_M_REV_FREQ_2,   ///< ����� � �������� � ����������� �������� ��������. ������ ���������� ��������.


            /// ����� � ��������. ������ ���������� ��������. ��������� ������
            /// ������.
            M_REV_2_ERROR,

            /// ����� � �������� � ����������� �������� ��������. ������
            /// ���������� ��������. ��������� ������ ������.
            DST_M_REV_FREQ_2_ERROR,

            /// �����, ����������� ����������� Altivar. ����� � ����������� �� Ethernet.
            /// ������ � ������ �����������.
            M_ATV,


            //FQT
            DST_FQT = 1,   ///< �������.
            DST_FQT_F,     ///< ������� + ������.
            DST_FQT_F_OK,  ///< ������� + ������ c ������������.
            DST_FQT_VIRT,  ///����������� c������ (��� �������� � �������).

            //QT
            DST_QT = 1,   ///< ��������������.
            DST_QT_OK,    ///< �������������� c ������������.
            DST_QT_IOLINK,///�������������� IOLInk ��� �������������� ����������.

            //LT
            DST_LT = 1,    ///������� ������� ��� �������������� ����������.
            DST_LT_CYL,    ///������� ������� ��� ��������������� �����.
            DST_LT_CONE,   ///������� ������� ��� ����� � ������� � ���������.
            DST_LT_TRUNC,  ///������� ������� ��� ����� � ��������� ��������� � ���������.

            DST_LT_IOLINK, ///������� IOLInk ������� ��� �������������� ����������.

            DST_LT_VIRT,   ///< ����������� ������� �������.

            //DO
            DST_DO = 1,    ///������� ���������� ����� � ��������� � �������
            DST_DO_VIRT,   ///����������� ���������� �����(��� �������� � �������)

            //DI
            DST_DI = 1,    ///������� ���������� ���� � ��������� � �������
            DST_DI_VIRT,   ///����������� ���������� ����(��� �������� � �������)

            //AO
            DST_AO = 1,    ///������� ���������� ����� � ��������� � �������
            DST_AO_VIRT,   ///����������� ���������� �����(��� �������� � �������)

            //AI
            DST_AI = 1,    ///������� ���������� ���� � ��������� � �������
            DST_AI_VIRT,   ///����������� ���������� ����(��� �������� � �������)

            //PT
            DST_PT = 1,      ///������� ���������� ������ ��������
            DST_PT_IOLINK,   ///������ �������� IOLInk

            //F
            DST_F = 1,       ///< �������������� �����������.
            };

        device( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~device();

        const char *get_type_name() const
            {
            switch ( type )
                {
                case DT_V:
                    return "������";
                case DT_VC:
                    return "����������� ������";
                case DT_M:
                    return "���������";

                case DT_LS:
                    return "�������";
                case DT_TE:
                    return "�����������";
                case DT_FS:
                    return "������";
                case DT_GS:
                    return "������ ���������";
                case DT_FQT:
                    return "�������";
                case DT_LT:
                    return "�������";
                case DT_QT:
                    return "������������";
                case DT_HA:
                    return "��������� �������� ������������";
                case DT_HL:
                    return "��������� �������� ������������";
                case DT_SB:
                    return "������";
                case DT_DI:
                    return "���������� ������� ������";
                case DT_DO:
                    return "���������� �������� ������";
                case DT_AI:
                    return "���������� ������� ������";
                case DT_AO:
                    return "���������� �������� ������";
                case DT_WT:
                    return "�������������";
                case DT_PT:
                    return "��������";
                case DT_F:
                    return "�������������� �����������";
                default:
                    return "???";
                }

            return "???";
            }

        const char *get_name() const
            {
            return name;
            }

        const char *get_description() const
            {
            return description;
            }

        void set_descr( const char *new_description );

        virtual void set_article( const char* new_article );

        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void direct_off() = 0;

        /// @brief ���������� ���������� � ������ ������� ������.
        void off();

        /// @brief ����� ������� � �������.
        ///
        /// ��� ������������� � ���������� �����.
        virtual void print() const;

        /// @brief ��������� ����������� ������ ����������.
        ///
        /// @return - ����� ����������.
        virtual u_int_4 get_serial_n() const
            {
            return s_number;
            }

        /// @brief ��������� ����������� ������ ����������.
        ///
        /// @return - ����� ����������.
        void set_serial_n( u_int_4 s_n )
            {
            s_number = s_n;
            }

        /// @brief ��������� ���� ����������.
        int get_type() const
            {
            return type;
            }

        /// @brief ��������� ������� ����������.
        int get_sub_type() const
            {
            return sub_type;
            }

        /// @brief ��������� �������������� �������, �������� ������� -
        /// ����������.
        ///
        /// ��� ������������� � Lua.
        virtual void set_property( const char* field, device* dev )
            {
            }

        /// @brief ��������� �������������� �������, �������� ������� -
        /// ������.
        ///
        /// ��� ������������� � Lua.
        virtual void set_string_property(const char* field, const char* value)
            {
            }

    protected:
        /// @brief ���������� �������������� ������ ���������� � ���� ������� Lua.
        ///
        /// @param buff [out] - ����� ������ ������.
        virtual int save_device_ex( char *buff )
            {
            return 0;
            }

        u_int_4 s_number;            ///< ���������������� ����� ����������.

        DEVICE_TYPE     type;        ///< ��� ����������.
        DEVICE_SUB_TYPE sub_type;    ///< ������ ����������.

        bool get_manual_mode() const
            {
            return is_manual_mode;
            }

        char * article;           ///< ������� �������.

    private:
        bool is_manual_mode;      ///< ������� ������� ������.

        char name[ C_MAX_NAME ];
        char *description;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����������� �������/��������.
///
/// ������� ����� ��� ��������� ���������� ���������.
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
        /// @brief ��������� ��������� �������.
        ///
        /// @return - ��������� �������.
        int  get_state();

        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        virtual void print() const;

#ifdef DEBUG_NO_IO_MODULES
    protected:
        int state;  ///< ��������� ����������.
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����������� �������/��������.
///
/// ������� ����� ��� ��������� ���������� ���������.
class analog_io_device : public device, public io_device
    {
    public:
        analog_io_device( const char *dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt ):
        device( dev_name, type, sub_type, par_cnt ),
            io_device( dev_name )
#ifdef DEBUG_NO_IO_MODULES
            ,value( 0 )
#endif  // DEBUG_NO_IO_MODULES
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
        float value;    ///< ��������� ����������.
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ���������� �������.
///
/// ��� ����� ���� ������, �����, ����� ����������...
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
            DO_INDEX = 0,   ///< ������ ������ ����������� ������.
            };
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ����������� ��������.
///
/// ��� ����� ���� ������, �����...
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
            DO_INDEX_1 = 0, ///< ������ ������ ����������� ������ �1.
            DO_INDEX_2,     ///< ������ ������ ����������� ������ �2.
            };
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
class valve_DO2_DI2_bistable;

/// @brief ���������� � ��������� �������.
///
class valve: public digital_io_device
    {
    public:
        /// @param is_on_fb - ������������ �������� ����� �� ���������� ���������.
        /// @param is_off_fb - ������������ �������� ����� �� ����������� ���������.
        /// @param number - ����� ����������.
        /// @param type - ��� ����������.
        /// @param sub_type - ������ ����������.
        valve( bool is_on_fb, bool is_off_fb,
            const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type );

        /// @brief ����������� ��� ������� ��� �������� ������.
        /// @param number - ����� ����������.
        /// @param type - ��� ����������.
        /// @param sub_type - ������ ����������.
        valve( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type );

        valve( bool is_on_fb, bool is_off_fb, const char *dev_name,
            device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type,
            int extra_params_cnt );

#ifdef _MSC_VER
#pragma region ���������� ������� � ���������.
#endif
        void off();

        void on();

        /// @brief ���������� �������� � ���������.
        static void evaluate();
#ifdef _MSC_VER
#pragma endregion ���������� ������� � ���������.
#endif

        /// @brief ��������� �������� �������� ����� �� ���������� ���������.
        virtual int get_on_fb_value()
            {
            return on_fb;
            }

        /// @brief ��������� �������� �������� ����� �� ����������� ���������.
        virtual int get_off_fb_value()
            {
            return off_fb;
            }

        /// @brief ���������� �������������� ������.
        int save_device_ex( char *buff );

        //��������� ������� (�����������).
        enum VALVE_STATE_EX
            {
            VX_ON_FB_OFF  = 11,  ///< �������, �� ���������.
            VX_OFF_FB_OFF = 10,  ///< ��������, �� ���������.

            VX_UPPER_SEAT_FB_OFF = 32, ///< ������� ������� �����, �� ���������.
            VX_LOWER_SEAT_FB_OFF = 33, ///< ������� ������ �����, �� ���������.

            VX_UPPER_SEAT = 2,   ///< ������� ������� �����.
            VX_LOWER_SEAT = 3,   ///< ������� ������ �����.

            VX_ON_FB_OK  = 1,    ///< �������, �� ��.
            VX_OFF_FB_OK = 0,    ///< ��������, �� ��.

            VX_ON_FB_ERR  = -2,  ///< �������, ������ ��.
            VX_OFF_FB_ERR = -3,  ///< ��������, ������ ��.

            //���� ������� ������.
            VX_ON_FB_OFF_MANUAL  = 31,   ///< ������� �������, �� ���������.
            VX_OFF_FB_OFF_MANUAL = 30,   ///< �������� �������, �� ���������.

            VX_LOWER_SEAT_MANUAL = 23,   ///< ������� ������� ������ �����.
            VX_UPPER_SEAT_MANUAL = 22,   ///< ������� ������� ������� �����.

            VX_ON_FB_OK_MANUAL  = 21,    ///< ������� �������, �� ��.
            VX_OFF_FB_OK_MANUAL = 20,    ///< �������� �������, �� ��.

            VX_ON_FB_ERR_MANUAL  = -12,  ///< ������� �������, ������ ��.
            VX_OFF_FB_ERR_MANUAL = -13,  ///< �������� �������, ������ ��.


            VX_UNKNOWN = 100,            ///< ����������� ���������.
            };

        /// @brief ��������� ������������ ��������� ������� (���� ��������
        /// �����, ������� ������, ...).
        int get_state();

#ifdef DEBUG_NO_IO_MODULES
        int set_cmd( const char *prop, u_int idx, double val );
#endif // DEBUG_NO_IO_MODULES

        ///��������� ������� ��� ����� �������� �����.
        enum VALVE_STATE
            {
            V_LOWER_SEAT = 3, ///< ������� ������ �����.
            V_UPPER_SEAT = 2, ///< ������� ������� �����.

            V_ON  = 1,        ///< �������.
            V_OFF = 0,        ///< ��������.

            V_STOP = 5,       ///< ����������.
            };

        bool is_wash_seat_active() const
            {
            return wash_flag;
            }
        void set_seat_wash_state( bool wash_flag )
            {
            this->wash_flag = wash_flag;
            }
        //��������� ��� ���������� ��������� ������������ ��������� � ������
        // ���� ��������� (������ �����, �������� �����, ...).
    protected:

        /// @brief ��������� ��������� ������� ��� ����� �������� �����.
        virtual VALVE_STATE get_valve_state() = 0;

        /// @brief ��������� ��������� �������� �����.
        virtual bool get_fb_state()
            {
            return true;
            }

        enum FB_STATE
            {
            FB_IS_AND_OFF = 0, ///< �������� ����� ���������.
            FB_IS_AND_ON,      ///< �������� ����� ��������.
            };

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 2,///���������� �������������� ����������.

            P_ON_TIME = 1,
            P_FB,
            };

    private:
        /// @brief ���� �������� ����� �� ���������� ���������.
        bool is_on_fb;

        /// @brief ���� �������� ����� �� ����������� ���������.
        bool is_off_fb;

        bool on_fb;
        bool off_fb;
#ifdef _MSC_VER
#pragma region ���������� ������� � ���������.
#endif
        /// @brief ������ ��������, ��������� ����������.
        static std::vector< valve* > to_switch_off;

    protected:
        /// @brief ������ ������������ ��������.
        static std::vector< valve_DO2_DI2_bistable* > v_bistable;

    private:
        /// @brief ����������� ���������� ���������� ������� � ���������.
        static bool is_switching_off_finished( valve *v )
            {
            return !v->is_switching_off;
            };

        bool is_switching_off; ///����������� �� ������ � ���������.
        u_long start_off_time; ///����� ������ ���������� ������� � ���������.
        bool was_on_auto;      ///������� ��������� ������� ����������� ����������.
#ifdef _MSC_VER
#pragma endregion ���������� ������� � ���������.
#endif

    protected:
        u_long start_switch_time;

    private:
        bool wash_flag;
    };
//-----------------------------------------------------------------------------
/// @brief ����������� ����������.
///
/// ���������� ��� ����������� ���������� ������ ���������� � ��������������
/// �������. ������ ������� ������ ������ �� ������.
class dev_stub : public i_counter, public valve
    {
    public:
        dev_stub() : valve( "STUB", DT_NONE, DST_NONE )
            {
            }

        u_int_4 get_serial_n() const;
        void    print() const;

        float   get_value();
        void    direct_set_value( float new_value );

        void    direct_on();
        void    direct_off();
        void    direct_set_state( int new_state );

        VALVE_STATE get_valve_state();
        int get_state()
            {
            return 0;
            }

        void    pause();
        void    start();
        void    reset();
        u_int   get_quantity();
        float   get_flow();

        u_int get_abs_quantity();
        void  abs_reset();
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ����� ���������� ������� � ����� ���������� ������.
///
class valve_DO1_DI1_off : public valve
    {
    public:
        valve_DO1_DI1_off( const char *dev_name );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< ������ ������ ����������� ������.
            DI_INDEX = 0,           ///< ������ ������ ����������� �����.
            };

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_IO_MODULES

        // ��������� valve ��� ���������� ��������� ������������ ��������� �
        // ������ ���� ��������� (������ �����, �������� �����, ...).
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
/// @brief ������ � ����� ���������� ������� � ����� ���������� ������.
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
            DO_INDEX = 0,           ///< ������ ������ ����������� ������.
            DI_INDEX = 0,           ///< ������ ������ ����������� �����.
            };

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //��������� ��� ���������� ��������� ������������ ��������� � ������
        // ���� ��������� (������ �����, �������� �����, ...).
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
/// @brief ������ � ����� ������� ���������� � ����� ��������� �������.
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
            DO_INDEX = 0,           ///< ������ ������ ����������� ������.

            DI_INDEX_1 = 0,         ///< ������ �1 ������ ����������� �����.
            DI_INDEX_2,             ///< ������ �2 ������ ����������� �����.
            };

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //��������� ��� ���������� ��������� ������������ ��������� � ������
        // ���� ��������� (������ �����, �������� �����, ...).
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
/// @brief ������ � ����� �������� ���������� � ����� ��������� �������.
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
            DO_INDEX_1 = 0,         ///< ������ �1 ������ ����������� ������.
            DO_INDEX_2,             ///< ������ �2 ������ ����������� ������.

            DI_INDEX_1 = 0,         ///< ������ �1 ������ ����������� �����.
            DI_INDEX_2,             ///< ������ �2 ������ ����������� �����.
            };

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_IO_MODULES

        //��������� ��� ���������� ��������� ������������ ��������� � ������
        // ���� ��������� (������ �����, �������� �����, ...).
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
/// @brief ������ � ����� �������� ���������� � ����� ��������� �������
/// ������������.
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
            DO_INDEX_OPEN = 0,  ///< ������ ������ ����������� ������ �������.
            DO_INDEX_CLOSE,     ///< ������ ������ ����������� ������ �������.

            DI_INDEX_OPEN = 0,  ///< ������ ������ ����������� ����� ������.
            DI_INDEX_CLOSE,     ///< ������ ������ ����������� ����� ������.
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

        //��������� ��� ���������� ��������� ������������ ��������� � ������
        // ���� ��������� (������ �����, �������� �����, ...).
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
            //��������� � �������� ���������.
            if ( is_opening && dt > t && i1 == 1 ) return false;

            //��������� � �������� ���������.
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

                    //���� ������ ��������� ������\������ ������ �� ������.
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
/// @brief ������ mixproof.
class valve_mix_proof : public i_mix_proof,  public valve
    {
    public:
        valve_mix_proof( const char *dev_name
            ): valve( true, true, dev_name, DT_V, DST_V_MIXPROOF )
            {
            }

        /// @brief ������� ������� �����.
        void open_upper_seat();

        /// @brief ������� ������ �����.
        void open_lower_seat();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< ������ ������ ����������� ������.
            DO_INDEX_U,     ///< ������ ������ ����������� ������ �������� �����.
            DO_INDEX_L,     ///< ������ ������ ����������� ������ ������� �����.

            DI_INDEX_OPEN = 0, ///< ������ ������ ����������� ����� ������.
            DI_INDEX_CLOSE,     ///< ������ ������ ����������� ����� ������.
            };

        void direct_set_state( int new_state );

#ifndef DEBUG_NO_IO_MODULES
        void direct_on();
        void direct_off();
#endif // DEBUG_NO_IO_MODULES

        //��������� ��� ���������� ��������� ������������ ��������� � ������
        // ���� ��������� (������ �����, �������� �����, ...).
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
/// @brief ������ AS-interface.
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

        /// @brief ��������� ������ ��������� ����������.
        char get_state_data( char* data )
            {
            if ( data == 0 )
                {
                return 0;
                }

            char state = 0;
            u_int offset = 0;

            if ( AS_number < 32 )		 //E��� ����� < 32).
                {
                offset = MAILBOX_OFFSET + AS_number / 2;
                state = data[ offset ];
                if ( AS_number % 2 == 0 )//������ ����� - ������� ������ ����.
                    {
                    state >>= 4;
                    }
                }
            else
                {
                u_int new_n = AS_number - 32;
                offset = MAILBOX_OFFSET + ( 32 / 2 ) + new_n / 2 + new_n % 2;
                state = data[ offset ];
                if ( AS_number % 2 == 1 )//�������� ����� - ������� ������ ����.
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

            if ( AS_number < 32 )		 //E��� ����� < 32).
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
            //��� ������� 31-�� ���������� ������ ����� - ������� ������
            //���� (1), ��� ��������� ��������� �������� ����� - ������� ������
            //���� (2).
            if ( ( AS_number < 32 && AS_number % 2 == 0 ) ||    			//1
                    ( AS_number >= 32 && AS_number % 2 == 1 ) )				//2
                {
                offset = 4;
                }

            //���������� � ���� ��� ������ ������ ����.
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
            //��� ������� 31-�� ���������� ������ ����� - ������� ������
            //���� (1), ��� ��������� ��������� �������� ����� - ������� ������
            //���� (2).
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
            //��� ������� 31-�� ���������� ������ ����� - ������� ������
            //���� (1), ��� ��������� ��������� �������� ����� - ������� ������
            //���� (2).
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
        u_int AS_number;    ///< AS-����� ����������.

        enum CONSTANTS
            {
            AI_INDEX = 0,   ///< ������ ������ ����������� �����.
            AO_INDEX = 0,   ///< ������ ������ ����������� ������.

            C_OPEN_S1 = 0x2,    ///< ������� �������� 1.
            C_OPEN_S2 = 0x4,    ///< ������� �������� 2.
            C_OPEN_S3 = 0x8,    ///< ������� �������� 3.

            S_CLOSED = 0x1,     ///< ������ ������.
            S_OPENED = 0x2,     ///< ������ ������.

            MAILBOX_OFFSET = 8
            };
    };
//-----------------------------------------------------------------------------
/// @brief ������ AS-mixproof.
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
/// @brief ������ AS-i ��������.
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

             if ( get_delta_millisec( start_err_time ) < 500 )
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
/// @brief ������ ������.
class valve_bottom_mix_proof : public i_mix_proof,  public valve
    {
    public:
        valve_bottom_mix_proof( const char *dev_name
            ): valve( true, true, dev_name, DT_V, DST_V_BOTTOM_MIXPROOF ),
            is_closing_mini(0)
            {
            }

        /// @brief ������� ������� �����. �� ������ ������, ��� ��� ��������
        /// ����� ���.
        void open_upper_seat()
            {
            }

        /// @brief ������� ������ �����.
        void open_lower_seat()
            {
            direct_set_state( V_LOWER_SEAT );
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< ������ ������ ����������� ������.
            DO_INDEX_MINI_V,///< ������ ������ ����������� ������ ���� �������.
            DO_INDEX_L,     ///< ������ ������ ����������� ������ ������� �����.

            DI_INDEX_OPEN = 0, ///< ������ ������ ����������� ����� ������.
            DI_INDEX_CLOSE,     ///< ������ ������ ����������� ����� ������.
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

                case V_UPPER_SEAT: //��������� �����������
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

        //��������� ��� ���������� ��������� ������������ ��������� � ������
        //���� ��������� (������ �����, �������� �����, ...).
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
#pragma region ���������� ���� ������� � ���������.
#endif
        /// @brief ������ ��������, ��������� ����������.
        static std::vector< valve_bottom_mix_proof* > to_switch_off;

        u_long start_off_time; //����� ������ �������� �������.

        int is_closing_mini; //���� ������ � ������ ��������

    public:
        /// @brief ����������� ���������� ���������� ������� � ���������.
        static bool is_switching_off_finished( valve_bottom_mix_proof *v );

        /// @brief ���������� ���� �������� � ���������.
        static void evaluate();

#ifdef _MSC_VER
#pragma endregion ���������� ���� ������� � ���������.
#endif
    };
//-----------------------------------------------------------------------------
/// @brief ������ IO-Link mixproof.
class valve_iolink_mix_proof : public i_mix_proof,  public valve
    {
    public:
        valve_iolink_mix_proof( const char* dev_name );

        void open_upper_seat();

        void open_lower_seat();

        VALVE_STATE get_valve_state();

        int save_device_ex( char *buff );

        void evaluate_io();

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
        out_data_swapped* out_info;

        bool blink = false;     //Visual indication
    };
//-----------------------------------------------------------------------------
/// @brief ������ IO-Link ��������.
class valve_iolink_shut_off : public valve
    {
    public:
        valve_iolink_shut_off( const char* dev_name );

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
        out_data_swapped* out_info = 0;

        bool blink = false;     //Visual indication
    };    
//-----------------------------------------------------------------------------
/// @brief ������ IO-link VTUG � ����� ������� ����������.
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
        /// @brief ��������� ������ ��������� ����������.
        char get_state_data( char* data );

        VALVE_STATE get_valve_state();

        /// @brief ��������� ��������� �������� �����.
        bool get_fb_state();

        u_int vtug_number;        ///< ����� ����������.
        u_int vtug_io_size = 1;   ///< ������ �������, � ������.

        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< ������ ������ ����������� ������.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ������ IO-link VTUG � ����� ������� ���������� � �������� ������.
class valve_iolink_vtug_on : public valve_iolink_vtug
    {
    public:
        valve_iolink_vtug_on(const char* dev_name);

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,     ///< ������ ������ ����������� �����.
            };

    protected:
#ifndef DEBUG_NO_IO_MODULES
        /// @brief ��������� ��������� �������� �����.
        bool get_fb_state();

        int get_on_fb_value();

        int get_off_fb_value();
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief ������ IO-link VTUG � ����� ������� ���������� � �������� ������.
class valve_iolink_vtug_off : public valve_iolink_vtug
    {
    public:
        valve_iolink_vtug_off(const char* dev_name);

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,     ///< ������ ������ ����������� �����.
            };

    protected:
#ifndef DEBUG_NO_IO_MODULES
        /// @brief ��������� ��������� �������� �����.
        bool get_fb_state();

        int get_on_fb_value();

        int get_off_fb_value();
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ���������� ������.
///
/// ��� ����� ���� �����������, ������ (��������)...
class AI1 : public analog_io_device
    {
    public:
        AI1( const char *dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt, u_int *start_par_idx );

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

    protected:
        enum CONSTANTS
            {
            P_ZERO_ADJUST_COEFF = 1,    ///< ����� ����.

            ADDITIONAL_PARAM_COUNT = 1, ///< ���������� ����������.

            C_AI_INDEX = 0,             ///< ������ ������ ����������� �����.
            };

        /// @brief ��������� ������������� �������� ������ ����������.
        virtual float get_max_val()
            {
            return 0;
            }

        /// @brief ��������� ������������ �������� ������ ����������.
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
/// @brief �����������.
class temperature_e : public AI1
    {
    public:
        temperature_e( const char *dev_name ): AI1( dev_name, DT_TE, DST_TE,
            ADDITIONAL_PARAM_COUNT, &start_param_idx )
            {
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
            P_ERR_T = 1,                ///< ��������� �������� �����������.

            ADDITIONAL_PARAM_COUNT = 1, ///< ���������� ����������.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ������ ����������� IO-Link.
class temperature_e_iolink : public AI1
    {
    public:
        temperature_e_iolink( const char *dev_name );

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
			P_ERR_T = 1,                ///< ��������� �������� �����������.

			ADDITIONAL_PARAM_COUNT = 1, ///< ���������� ����������.
			};
    };
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������.
class level: public AI1
    {
    public:
        level( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt, u_int* start_par_idx );

        int get_volume();
        virtual int calc_volume();

        int save_device_ex( char* buff );

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            P_ERR = 1,       ///< ��������� �������� ������.

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief ������� �������.
class level_e : public level
    {
    public:
        level_e( const char* dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief ������� ������� c ��� ����� �������������� �����.
class level_e_cyl : public level
    {
    public:
        level_e_cyl( const char* dev_name );

        int calc_volume();

    private:
        enum CONSTANTS
            {
            P_MAX_P = 1, ///< ������ ��������� �������� ��������� ������� (���).
            P_R,         ///< ������ ��������� ������� ����� (�).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief ������� ������� c ��� ��������������� ����� � �������� ����������.
class level_e_cone : public level
    {
    public:
        level_e_cone( const char* dev_name );

         int calc_volume();

    private:
        enum CONSTANTS
            {
            P_MAX_P = 1, ///< ������ ��������� �������� ��������� ������� (���).
            P_R,         ///< ������ ��������� ������� ����� (�).
            P_H_CONE,    ///< ������ ��������� ������ ������ ����� (�).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief ������� ��������.
class pressure_e : public AI1
    {
    public:
        pressure_e( const char *dev_name ): AI1( dev_name, DT_PT, DST_NONE,
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

            P_MIN_V = 1,   ///< ������ ��������� ������������ ��������.
            P_MAX_V,       ///< ������ ��������� ������������� ��������.
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief ������� ��������.
class pressure_e_iolink : public AI1
    {
    public:
        pressure_e_iolink( const char* dev_name );

        float get_max_val();
        float get_min_val();

#ifndef DEBUG_NO_IO_MODULES
        float get_value();
        int get_state();
#endif

        void set_article( const char* new_article );

        enum class ARTICLE
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
            ADDITIONAL_PARAM_COUNT = 2,

            P_MIN_V = 1,   ///< ������ ��������� ������������ ��������.
            P_MAX_V,       ///< ������ ��������� ������������� ��������.
            };

        u_int start_param_idx;

        float v;
        int st;
    };
//-----------------------------------------------------------------------------
/// @brief �������������� �����������.
class circuit_breaker : public analog_io_device
    {
    public:
        circuit_breaker( const char* dev_name );

        int save_device_ex( char *buff );

        int set_cmd( const char *prop, u_int idx, double val );

        void direct_set_value( float v );

        void direct_on();

        void direct_off();

        float get_value();

        int get_state();

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
            C_AI_INDEX = 0,             ///< ������ ������ ����������� �����.
            };

        bool is_read_OK;

        float v;
        int st;
        int err;
        int m;

        F_data_in in_info;
        F_data_out *out_info;
    };
//-----------------------------------------------------------------------------
/// @brief ������ ������������� ������ IO-Link.
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
            P_MAX_P = 1, ///< ������ ��������� �������� ��������� ������� (���).
            P_R,         ///< ������ ��������� ������� ����� (�).
            P_H_CONE,    ///< ������ ��������� ������ ������ ����� (�).

            LAST_PARAM_IDX,
            };

        u_int start_param_idx;

        int st;
        float v;

        i_AI_device* PT_extra;
    };
//-----------------------------------------------------------------------------
/// @brief ������������.
class concentration_e : public AI1
    {
    public:
        concentration_e( const char *dev_name, DEVICE_SUB_TYPE sub_type ): AI1(
            dev_name, DT_QT, sub_type, ADDITIONAL_PARAM_COUNT, &start_param_idx )
            {
#ifdef DEBUG_NO_IO_MODULES
            st = 1;
#endif
            set_par_name( P_MIN_V,  start_param_idx, "P_MIN_V" );
            set_par_name( P_MAX_V,  start_param_idx, "P_MAX_V" );
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            ADDITIONAL_PARAM_COUNT = 2,

            P_MIN_V = 1,   ///< ������ ��������� ������������ ��������.
            P_MAX_V,       ///< ������ ��������� ������������� ��������.
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief ������������ � �������� �����������.
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
            DI_INDEX = 0,         ///< ������ ������ ����������� �����.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ������ ������������ IO-Link.
class concentration_e_iolink : public AI1
    {
    public:
        concentration_e_iolink(const char* dev_name);

        int save_device_ex( char *buff );

        float get_temperature() const;

#ifdef DEBUG_NO_IO_MODULES
        float get_value();
#else
        float get_value();

        int get_state();
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
    };
//-----------------------------------------------------------------------------
/// @brief ���������� ����������� �����.
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

            P_MIN_V = 1,   ///< ������ ��������� ������������ ��������.
            P_MAX_V,       ///< ������ ��������� ������������� ��������.
            };

        u_int start_param_idx;
    };
//-----------------------------------------------------------------------------
/// @brief ������ ����
class wages : public analog_io_device, public i_wages
    {
    public:
        wages( const char *dev_name);

        void tare();
        float get_weight();

    protected:

        enum CONSTANTS
            {
            P_NOMINAL_W = 1,    ///< ����������� ��������.
            P_RKP = 2,          ///< ������� ����������� ��������
            P_C0 = 3,           ///< ��������� ����
            P_DT = 4,           ///< ����������� �������

            ADDITIONAL_PARAM_COUNT = 4, ///< ���������� ����������.

            C_AI_Ud = 0,             ///< ������ ������ Ud(�����������).
            C_AI_Uref = 1,           ///< ������ ������ Uref(������).
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
/// @brief ����������� ���������� ��� �������� � ������� �����-������
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
/// @brief ����������� ���������� ��� �������� � ������� �����-������
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

        /// @brief ��������� ����������� �������� �������� (��� �����
        /// ��������� �����).
        u_int get_abs_quantity();

        /// @brief ����� ����������� �������� ��������.
        void  abs_reset();

        int set_cmd( const char *prop, u_int idx, double val );

        void set( u_int value, u_int abs_value, float flow );

        void eval( u_int read_value, u_int abs_read_value, float read_flow );

        //Lua.
        int save_device_ex( char *buff );

    protected:
        STATES state;

    private:
        float flow_value;

        u_int value;
        u_int last_read_value;

        u_int abs_value;       ///< ���������� �������� (�� ���������� �� �����).
        u_int abs_last_read_value;

        bool is_first_read;         ///< ���� ������ ��������� ��������.
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ���������� �������.
///
/// ��� ����� ���� ����������� ������...
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
            AO_INDEX = 0,   ///< ������ ������ ����������� ������.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ���������� ������.
///
/// ��� ����� ���� ����������� ������...
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

            P_MIN_VALUE = 1,   ///< ������ ��������� ������������ ��������.
            P_MAX_VALUE,       ///< ������ ��������� ������������� ��������.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ����������� ������.
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
            C_MIN = 0,   ///< ����������� ��������.
            C_MAX = 100, ///< ������������ ��������.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ���������� ������.
///
/// ��� ����� ���� �������� �����, ������ (����/���)...
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
        /// @brief ��������� ��������� �������.
        ///
        /// @return - ��������� �������.
        int  get_state();
#endif // DEBUG_NO_IO_MODULES


    private:
        int current_state;
        u_int_4 time;

        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< ������ ������ ����������� �����.

            ADDITIONAL_PARAMS_COUNT = 1,

            P_DT = 1,
            };
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ����� ������� ����������.
class valve_DO1 : public valve
    {
    public:
        valve_DO1( const char *dev_name ) : valve( dev_name, DT_V, DST_V_DO1 )
            {
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< ������ ������ ����������� ������.
            };

#ifndef DEBUG_NO_IO_MODULES
    public:
        void direct_on();
        void direct_off();

#endif // DEBUG_NO_IO_MODULES

    protected:
        /// @brief ��������� ��������� ������� ��� ����� �������� �����.
        VALVE_STATE get_valve_state()
            {
#ifdef DEBUG_NO_IO_MODULES
            return ( VALVE_STATE ) digital_io_device::get_state();
#else
            return ( VALVE_STATE ) get_DO( DO_INDEX );
#endif // DEBUG_NO_IO_MODULES
            };

        /// @brief ��������� ��������� �������� �����.
        bool get_fb_state()
            {
            return true;
            }
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ����� �������� ����������.
class valve_DO2 : public DO2
    {
    public:
        valve_DO2( const char *dev_name ): DO2( dev_name, DT_V, DST_V_DO2, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ���������������� (�������, �����).
class motor : public device, public io_device
    {
    public:
        motor( const char *dev_name, device::DEVICE_SUB_TYPE sub_type ):
            device( dev_name, DT_M, sub_type, ADDITIONAL_PARAM_COUNT ),
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

            P_ON_TIME = 1,    ///< ������ ��������� ������� ��������� (����).

            DO_INDEX = 0,         ///< ������ ������ ����������� ������.
            DO_INDEX_REVERSE = 1, ///< ������ ������ ����������� ������ �������.

            DI_INDEX       = 0,   ///< ������ ������ ����������� �����.
                                  //   ���
            DI_INDEX_ERROR = 0,   ///< ������ ������ ����������� ����� ������.

            AO_INDEX = 0,     ///< ������ ������ ����������� ������.
            };

        u_long start_switch_time;

#ifdef DEBUG_NO_IO_MODULES
        char  state;  ///< ��������� ����������.

        float freq;   ///< ��������� ���������� (�������).
#endif // DEBUG_NO_IO_MODULES
    };
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// @brief ����������������, ����������� ��������� ���������������� altivar �
/// ������������ ������ Ethernet.
class motor_altivar : public device, public io_device
{
public:
    motor_altivar(const char *dev_name, device::DEVICE_SUB_TYPE sub_type) :
        device(dev_name, DT_M, sub_type, ADDITIONAL_PARAM_COUNT),
        io_device(dev_name),
        start_switch_time(get_millisec()),
        atv(NULL)
#ifdef DEBUG_NO_IO_MODULES
        , state(0),
        freq(0)
#endif // DEBUG_NO_IO_MODULES
    {
    set_par_name(P_ON_TIME, 0, "P_ON_TIME");
    }

    int save_device_ex(char *buff);

    float get_value();

    void direct_set_value(float value);

    void direct_set_state(int new_state);

    int  get_state();

    void direct_on();

    void direct_off();

    void set_string_property(const char* field, const char* value) override;

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

        P_ON_TIME = 1,    ///< ������ ��������� ������� ��������� (����).

        DO_INDEX = 0,         ///< ������ ������ ����������� ������.
        DO_INDEX_REVERSE = 1, ///< ������ ������ ����������� ������ �������.

        DI_INDEX = 0,   ///< ������ ������ ����������� �����.
        //   ���
        DI_INDEX_ERROR = 0,   ///< ������ ������ ����������� ����� ������.

        AO_INDEX = 0,     ///< ������ ������ ����������� ������.
    };

    u_long start_switch_time;

    altivar_node* atv;

#ifdef DEBUG_NO_IO_MODULES
    char  state;  ///< ��������� ����������.

    float freq;   ///< ��������� ���������� (�������).
#endif // DEBUG_NO_IO_MODULES
};
//-----------------------------------------------------------------------------
/// @brief ������ ������������� ������.
class level_s : public DI1
    {
    public:
        level_s( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

        bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief ������ ������������� ������ IO-Link.
class level_s_iolink : public AI1
    {
    public:
        level_s_iolink( const char *dev_name, device::DEVICE_SUB_TYPE sub_type );

        float get_min_value();

        float get_max_value();

#ifndef DEBUG_NO_IO_MODULES
        float get_value();

        int get_state();
#endif

        bool is_active();

        void evaluate_io();

    private:
        enum class ARTICLE
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
    };
//-----------------------------------------------------------------------------
/// @brief ������ ������������� �������.
class flow_s : public DI1
    {
    public:
        flow_s( const char *dev_name ): DI1( dev_name, DT_FS, DST_NONE,
            0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ������ ���������.
class state_s : public DI1
    {
    public:
        state_s( const char *dev_name ): DI1( dev_name, DT_GS, DST_NONE,
            0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ������ ����������� ����� �����.
class DI_signal : public DI1
    {
    public:
        DI_signal( const char *dev_name ): DI1( dev_name, DT_DI, DST_NONE, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ������.
class button : public DI1
    {
    public:
        button( const char *dev_name ): DI1( dev_name, DT_SB, DST_NONE, 0 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ���������� ������ ����������.
class DO_signal : public DO1
    {
    public:
        DO_signal( const char *dev_name ): DO1( dev_name, DT_DO, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������������.
class siren : public DO1
    {
    public:
        siren( const char *dev_name ): DO1( dev_name, DT_HA, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������������.
class lamp : public DO1
    {
    public:
        lamp( const char *dev_name ): DO1( dev_name, DT_HL, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief �������.
class counter : public device,
    public i_counter,
    public io_device
    {
    public:
        counter( const char *dev_name,
            DEVICE_SUB_TYPE sub_type = device::DST_FQT,
            int extra_par_cnt = ADDITIONAL_PARAMS_COUNT );

        virtual ~counter()
            {
            }

        float get_value();
        void  direct_set_value( float new_value );
        int   get_state();
        void  direct_on();
        void  direct_off();
        void  direct_set_state( int new_state );
        virtual void  print() const;

        void  pause();
        void  start();
        void  reset();
        u_int get_quantity();
        float get_flow()
            {
            return 0;
            }

        /// @brief ��������� ����������� �������� �������� (��� �����
        /// ��������� �����).
        u_int get_abs_quantity();

        /// @brief ����� ����������� �������� ��������.
        void  abs_reset();

        int set_cmd( const char *prop, u_int idx, double val );

        //Lua.
        int save_device_ex( char *buff )
            {
            return sprintf( buff, "ABS_V=%u, ", get_abs_quantity() );
            }

    protected:
        STATES state;

    private:

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 0,

            AI_Q_INDEX    = 0,  ///< ������ ������ ����������� ����� (�����).

			MAX_OVERFLOW = 300, ///< ������������ ������������ �� ����

            MAX_VAL = 65535L,   ///< ������������ �������� ��������.
            };

        u_int value;
        u_int last_read_value;

        u_int abs_value;       ///< ���������� �������� (�� ���������� �� �����).
        u_int abs_last_read_value;

        bool is_first_read;         ///< ���� ������ ��������� ��������.
        bool is_first_read_abs;     ///< ���� ������ ��������� ��������.
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief �������.
class counter_f : public counter
    {
    public:
        counter_f( const char *dev_name );

        virtual ~counter_f()
            {
            }

        int get_state();

        float get_flow();

        void set_property( const char* field, device* dev );

        int set_cmd( const char *prop, u_int idx, double val );

        //Lua.
        int save_device_ex( char *buff );

    private:

        enum CONSTANTS
            {
            ADDITIONAL_PARAMS_COUNT = 4,

            P_MIN_FLOW = 1,
            P_MAX_FLOW,
            P_CZ,
            P_DT,

            AI_FLOW_INDEX = 1,  ///< ������ ������ ����������� ����� (�����).
            };

        float flow_value;

        std::vector < device* > motors;
    };
//-----------------------------------------------------------------------------
/// @brief ������� c ������������.
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
            DI_INDEX = 0,  ///< ������ ������ ����������� ����� (�����������).
            };
    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������.
///
/// �������� ���������� ��� ���� ����������� �������.
class device_manager: public i_Lua_save_device
    {
    public:
        device_manager();

        virtual ~device_manager();

        /// @brief ��������� ����������.
        device* get_device( int dev_type, const char *dev_name );

        /// @brief ��������� ����������.
        device* get_device( u_int serial_dev_n )
            {
            if ( serial_dev_n < project_devices.size() )
                {
                return project_devices[ serial_dev_n ];
                }

            return &stub;
            }

        /// @brief ��������� ������� �� ��� ������.
        i_DO_device* get_V( const char *dev_name );

        /// @brief ��������� ������������ ������� �� ������.
        i_AO_device* get_VC( const char *dev_name );

        /// @brief ��������� ��������� �� ������.
        i_DO_AO_device* get_M( const char *dev_name );

        /// @brief ��������� ������ �� ������.
        i_DI_device* get_LS( const char *dev_name );

        /// @brief ��������� ������� �� ������.
        i_DI_device* get_FS( const char *dev_name );

        /// @brief ��������� ������� ��������� �� ������.
        i_DI_device* get_GS( const char *dev_name );

        /// @brief ��������� ����������� ����� �� ������.
        i_AI_device* get_AI( const char *dev_name );

        /// @brief ��������� ����������� ������ �� ������.
        i_AO_device* get_AO( const char *dev_name );

        /// @brief ��������� �������� �� ������.
        i_counter* get_FQT( const char *dev_name );
        virtual_counter* get_virtual_FQT( const char *dev_name );

        /// @brief ��������� ����������� �� ������.
        i_AI_device* get_TE( const char *dev_name );

        /// @brief ��������� �������� ������ �� ������.
        i_AI_device* get_LT( const char *dev_name );

        /// @brief ��������� �������� ����� �� ������.
        i_DI_device* get_DI( const char *dev_name );

        /// @brief ��������� ������ �� ������.
        i_DI_device* get_SB( const char *dev_name );

        /// @brief ��������� ������������ ������ �� ������.
        i_DO_device* get_DO( const char *dev_name );

        /// @brief ��������� �������� ������������.
        i_DO_device* get_HA( const char *dev_name );

        /// @brief ��������� �������� ������������.
        i_DO_device* get_HL( const char *dev_name );

        /// @brief ��������� �������� �������� �� ������.
        i_AI_device* get_PT( const char *dev_name );

        /// @brief ��������� ������� ������������ �� ������.
        i_AI_device* get_QT( const char *dev_name );

        /// @brief ��������� ����� �� ������.
        wages* get_WT( const char *dev_name );

        /// @brief ��������� ��������������� ����������� �� �����.
        circuit_breaker* get_F(const char* dev_name);

        /// @brief ��������� ������������� ���������� ������.
        static device_manager* get_instance();

        /// @brief ���������� ������ ������� � �������.
        void print() const;

        const char* get_name_in_Lua() const
            {
            return "Device manager";
            }

        /// @brief ��������� ���������� ����������.
        dev_stub* get_stub()
            {
            return &stub;
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

#ifdef RM_PAC
        int rm_save_device( char *buff );

        int rm_save_device_state( char *buff );
#endif // RM_PAC

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

    protected:
        dev_stub stub;  ///< ����������-��������, ��������� ����������.

        struct range    ///< �������� ��������� ������ ����.
            {
            int start_pos;
            int end_pos;

            range(): start_pos( -1 ), end_pos( -1 )
                {
                }
            };

        /// ��������� ��������� ���� �����.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        int get_device_n( device::DEVICE_TYPE dev_type,
            const char *dev_name );

        std::vector< device* > project_devices; ///< ��� ����������.

        /// @brief ������������ ��������� ������.
        static auto_smart_ptr < device_manager > instance;

    public:
        // @brief ���������� ����������.
        //
        // ���������� �� Lua.
        io_device* add_io_device( int dev_type, int dev_sub_type,
            const char *dev_name, char * descr, char* article );
    };
//-----------------------------------------------------------------------------
/// @brief ������.
///
/// ������������� ���������������� �������.
class timer
    {
    public:
        enum STATE
            {
            S_STOP = 0, ///< �� ��������.
            S_WORK,     ///< ��������.
            S_PAUSE,    ///< �����.
            };

        /// @brief ���������� ��������� ���������� � �����.
        ///
        /// @param buff [ out ] - ����� ������, ���� ����� ������������ ������.
        ///
        /// @return >= 0 - ���������� ���������� ����.
        int save( char *buff );

        /// @brief ���������� ��������� ���������� �� ������.
        ///
        /// @param buff [ out ] - ����� ������, ������ ����� ����������� ������.
        ///
        /// @return >= 0 - ���������� ��������� ����.
        int load( char *buff );

        /// @brief ��������� ������� ������ ��� ���������� ��������� ����������.
        ///
        /// @return >= 0 - ������ ������, ����.
        int get_saved_size() const;

        timer();

        /// @brief ������ �������.
        void start();

        /// @brief ����� �������.
        void reset();

        /// @brief ����� �������.
        void pause();

        /// @brief �������� ������ ������� �������.
        ///
        /// @return 1 - ����� �����.
        /// @return 0 - ����� �� �����.
        bool is_time_up() const;

        /// @brief ��������� ������� ������ �������.
        ///
        /// @return - ����� ������ �������.
        u_long  get_work_time() const;

        /// @brief ��������� ������� �������.
        ///
        /// @param new_countdown_time - �������.
        void set_countdown_time( u_long new_countdown_time );

        /// @brief ��������� ������� �������.
        ///
        /// @return - ������� �������.
        u_long  get_countdown_time() const;

        /// @brief ��������� ��������� �������.
        ///
        /// @return - ��������� �������.
        STATE get_state() const;

    private:
        u_long  last_time;  ///< �����, ����� ������ ��� �������/����������.
        u_long  work_time;  ///< ����� ������ �������.

        STATE   state;           ///< ���������.
        u_long  countdown_time;  ///< ������� �������.
    };
//-----------------------------------------------------------------------------
/// @brief ������.
///
/// ���������� ������ ��������.
class timer_manager
    {
    public:
        /// @param timers_count - ���������� �������� � ������.
        timer_manager( u_int timers_count );

        ~timer_manager();

        /// @brief ���������� ��������� ������� �� �������.
        ///
        /// @param index - ������ �������.
        ///
        /// @return - ������ � ������ ��������, �������� - � ������ ������ ��
        /// ��������.
        timer* operator[] ( unsigned int index );

        /// @brief ���������� ��������.
        int get_count() const
            {
            return timers_cnt;
            }

    private:
        u_int   timers_cnt; ///< ���������� ��������.
        timer   *timers;    ///< �������.

        timer   stub;       ///< ��������.
    };
//-----------------------------------------------------------------------------
///@brief ��������� ��������� ���������.
///
///@return �������� ��������� �������.
device_manager* G_DEVICE_MANAGER();
//-----------------------------------------------------------------------------
/// @brief ��������� ������� �� ������.
///
/// @param number - ����� �������.
/// @return - ������ � �������� �������. ���� ��� ������ �������, ������������
/// �������� (@ref dev_stub).
i_DO_device* V( u_int dev_n );
valve* V( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������������ ������� �� ������.
///
/// @param number - ����� �������.
/// @return - ������ � �������� �������. ���� ��� ������ �������, ������������
/// �������� (@ref dev_stub).
i_AO_device* VC( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ��������� �� ������.
///
/// @param number - ����� ���������.
/// @return - ��������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_AO_device* M( u_int dev_n );
i_DO_AO_device* M( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ������ �� ������.
///
/// @param number - ����� ����������� ������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* LS( u_int dev_n );
i_DI_device* LS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ������� �� ������.
///
/// @param number - ����� ����������� �������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* FS( u_int dev_n );
i_DI_device* FS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ����� �� ������.
///
/// @param number - ����� ����������� �����.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* AI( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ������ �� ������.
///
/// @param number - ����� ����������� ������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AO_device* AO( u_int dev_n );
i_AO_device* AO( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� �� ������.
///
/// @param number - ����� ��������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_counter* FQT( u_int dev_n );
i_counter* FQT( const char *dev_name );

virtual_counter* virtual_FQT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� �� ������.
///
/// @param number - ����� �����������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* TE( u_int dev_n );
i_AI_device* TE( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������ �� ������.
///
/// @param number - ����� �������� ������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
level* LT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������� ��������� �� ������.
///
/// @param number - ����� ������� ���������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* GS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������������ �� ������.
///
/// @param number - �����.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_device* HA( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������������ �� ������.
///
/// @param number - �����.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_device* HL( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������ �� ������.
///
/// @param number - �����.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* SB( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ����� �� ������.
///
/// @param number - ����� �������� �����.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* DI( u_int dev_n );
i_DI_device* DI( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������ ���������� �� ������.
///
/// @param number - ����� ������ ����������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_device* DO( u_int dev_n);
i_DO_device* DO( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� �������� �� ������.
///
/// @param number - ����� �������� ��������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* PT( u_int dev_n );
i_AI_device* PT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������� ������������ �� ������.
///
/// @param number - ����� ������� ������������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* QT( u_int dev_n );
i_AI_device* QT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����� �� ������.
///
/// @param number - ����� �����.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
wages* WT( u_int dev_n );
wages* WT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ��������������� ����������� �� ������.
///
/// @param number - ����� ��������������� �����������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AO_device* F(u_int dev_n);
i_AO_device* F(const char* dev_name);
//-----------------------------------------------------------------------------
/// @brief ��������� ������������ ����������.
///
/// @return - ����������� ����������.
dev_stub* STUB();
//-----------------------------------------------------------------------------
/// @brief ��������� ���������� �� ������ � ����.
///
/// @param type   - ��� ����������.
/// @param s_number - ���������� ����� ����������.
/// @return - ����������.
device* DEVICE( int s_number );
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
