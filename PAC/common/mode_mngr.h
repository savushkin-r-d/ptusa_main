/// @file mode_mngr.h
/// @brief �������� �������� �������, ������� ������������ ��� ����������� �����
/// ��� ���������� �������� �����, ��������.
///
/// ����� @ref step, �������� ��� ����������� ������ � �����, �������� ���
/// ����������� ���������� - ������ �����������\����������� ���������,
/// ��������� ����, ��������� � ��� ������� ��������. ��� - ������������ ���
/// ���������� ���������������� ��������, ���������\���������� ��������
/// ���������� �������������, ��� ����������������� ������� ���������. �
/// ������� �� ��������, ������� �������� ��������\��������� ���. ��� ��������
/// ���� �������� ������� ������ ����� @ref mode_manager.
/// ����� @ref action �������� ������ ��������� � ��������� ��� ����
/// ������������ ��������.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef MODE_MNGR
#define MODE_MNGR

#include <string>

#include "dtime.h"

#include "PAC_dev.h"
#include "PAC_info.h"
#include "param_ex.h"

#include "i_tech_def.h"

class operation_manager;
class operation_state;

//-----------------------------------------------------------------------------
/// @brief �������� ��� ������������ (���������, ���������� � �.�.).
class action
    {
    public:
        action( std::string name, u_int subgropups_cnt = 1 );

        virtual ~action()
            {
            }

        /// @brief �������� �� ���������� ���������.
        ///
        /// @return true  ���� ����������, ��� �������� ���-�� ��������.
        /// @return false ��� ���������, ��� �������� ���-�� ��������.
        virtual bool is_empty() const;

        virtual void print( const char* prefix = "", bool new_line = true ) const;

        /// @brief �������� ��������.
        ///
        /// @param [out] reason ���������, ������ ������ ��������� ��������.
        virtual int check( char* reason ) const
            {
            reason[ 0 ] = 0;
            return 0;
            }

        /// @brief ������������� ��������.
        virtual void init() {}

        /// @brief ���������� ��������.
        virtual void evaluate() {}

        /// @brief ���������� ��������.
        virtual void final();

        /// @brief ���������� ���������� � ��������.
        ///
        /// @param [in] dev ����������.
        /// @param [in] group ������ ����������.
        /// @param [in] subgroup ��������� ���������.
        virtual void add_dev( device *dev, u_int group = MAIN_GROUP,
            u_int subgroup = MAIN_SUBGROUP );

        /// @brief ����� ���������� � �����-���� �������.
        ///
        /// @param [out] err_dev_name ����������.
        /// @param [in] max_to_write �������������� ��������.
        /// @return 0 - ��� ������, 1 - ���� ������.
        int check_devices( char* err_dev_name, int max_to_write ) const;

        /// @brief ��������� ���������� ��� ��������.
        ///
        /// @param [in] par ���������.
        virtual void set_params( const saved_params_float *par ) 
            {
            this->par = par;
            }

        /// @brief ������� �������� ������������ ���������� � ��������.
        ///
        /// @param [in] position ������� ���������.
        /// @param [in] idx ������ ���������.
        void set_param_idx( unsigned int position, int idx )
            {
            while ( position >= par_idx.size() )
                {
                par_idx.push_back( 0 );
                }

            par_idx[ position ] = idx;
            }

        enum CONSTANTS
            {
            MAIN_GROUP = 0,
            MAIN_SUBGROUP = 0
            };

    protected:
        u_int subgropups_cnt;

        // ����������.
        std::vector < std::vector< std::vector< device* > > > devices; 
        std::string name;                               ///< ��� ��������.

        const saved_params_float *par;      ///< ��������� ��������.
        std::vector< int >        par_idx;  ///< ������� ���������� ��������.
    };
//-----------------------------------------------------------------------------
/// <summary>
/// ��������� ���������.
/// </summary>
class on_action: public action
    {
    public:
        on_action(): action( "��������" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// ��������� ��������� ���������.
/// </summary>
class on_reverse_action : public action
    {
    public:
        on_reverse_action() : action( "�������� ������" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// ���������� ���������.
/// </summary>
class off_action: public action
    {
    public:
        off_action(): action( "���������" )
            {
            }

        void evaluate();

        void init();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// �������� �����.
/// </summary>
class open_seat_action: public action
    {
    public:
        open_seat_action( bool is_mode, operation_state *owner );

        void init();
        void evaluate();
        void final();


        void set_wait_time( int wait_time );

        /// @brief ���������� ���������� � ��������.
        ///
        /// @param [in] dev ����������.
        /// @param [in] group �������������� ��������.
        /// @param [in] seat_type �������������� ��������.
        void add_dev( device *dev, u_int group, u_int seat_type );

        virtual void print( const char* prefix = "", bool new_line = true ) const;

        bool is_empty() const;
    private:
        enum PHASES
            {
            P_WAIT = 0,
            P_OPEN_UPPER,
            P_OPEN_LOWER,
            };

        PHASES phase;      ///< ������� ����.
        PHASES next_phase; ///< ��������� ����.

        u_int     active_group_n;  ///< ����� ����������� ������ ������.

        u_int_4 wait_time;      ///< ����� �������� ����� ����������� �����.
        u_int_4 wait_seat_time; ///< ����� �������� ����� ����������� ����� ������.
        u_int_4 wash_time_upper;///< ����� �������� ������� ����� ������� ������ ��������.
        u_int_4 wash_time_lower;///< ����� �������� ������ ����� ������� ������ ��������.

        /// �����.
        std::vector< std::vector< device* > > wash_upper_seat_devices;
        std::vector< std::vector< device* > > wash_lower_seat_devices;

        u_int_4 start_cycle_time; ///< ����� ������ ����� (�������� ��� ��������).

        bool is_mode;             ///< �������� �� ����� ��������.
        operation_state* owner;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// ���� DI->DO.
/// </summary>
class DI_DO_action: public action
    {
    public:
        DI_DO_action( ):action( "������ DI->DO's" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// ���� AI->AO.
/// </summary>
class AI_AO_action : public action
    {
    public:
        AI_AO_action() :action( "������ AI->AO's" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// �������� ���������� ������� ��������.
/// </summary>
class required_DI_action: public action
    {
    public:
        required_DI_action(): action( "������� ��� ���������" )
            {
            }

        int check( char* reason ) const;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// ����� �����.
/// </summary>
class wash_action: public action
    {
    public:
        wash_action(): action( "���������� DI's DO's DEV's R_DEV's AI", G_SUBGROUPS_CNT )
            {
            }

        void evaluate();

        virtual void print( const char* prefix = "", bool new_line = true ) const;

    private:
        enum GROUPS
            {
            G_DI = 0,       //������� ������� ������� ��������� ���������.
            G_DO,           //�������� ������� "��".
            G_DEV,          //����������, ���������� �� �������.
            G_REV_DEV,      //��������� ����������, ���������� �� �������.
            G_PUMP_FREQ,    //������� ������������������.

            G_SUBGROUPS_CNT,   //���������� �����.
            };
    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������� �� �����������, ������� ������ � ��� (�����������/
/// �����������).
///
/// � �������� ����� ���� �������� (�����������) ������ ���� ���.
/// ����� ������������� ����� �������� 1 ���� ����� ����� �����������.
class step
    {
    public:
        enum ACTIONS
            {
            A_ON = 0,
            A_ON_REVERSE,
            A_OFF,
            A_UPPER_SEATS_ON,
            A_LOWER_SEATS_ON = A_UPPER_SEATS_ON,

            A_REQUIRED_FB,
            A_DI_DO,
            A_AI_AO,
            A_WASH,
            };

        step( std::string name, operation_state *owner, bool is_mode = false );

        ~step();

        /// @brief ��������� �������� ����� �������� ��������������.
        ///
        /// @param idx - ������ ��������.
        ///
        /// @return - �������� �������� � �������� ��������. ���� ������
        /// ������� �� ��������, ������������ �������� 0.
        action* operator[] ( int idx );

        /// @brief �������� ����������� ���������� ����.
        ///
        /// @return > 0 - ������ ���������.
        /// @return   0 - ��.
        int check( char* reason ) const;

        void init();

        void evaluate() const;

        void final();

        /// ��������� ������� ���������� ����.
        u_int_4 get_eval_time() const;

        /// ������������ ������� ������ ����.
        void set_start_time( u_int_4 start_time );

        /// ������� �� ������� ������.
        void print( const char* prefix = "" ) const;

        /// @brief �������� �� ���������� ���������.
        ///
        /// @return true  ���� ����������, ��� �������� ���-�� ��������.
        /// @return false ��� ���������, ��� �������� ���-�� ��������.
        bool is_empty() const;

        int check_devices( char* err_dev_name, int str_len );

        const char* get_name() const
            {
            return name.c_str();
            }

        bool is_active() const
            {
            return active;
            }
    private:
        std::vector< action* > actions; ///< ��������.
        action action_stub;             ///< ��������� ��������.
        u_int_4 start_time;             ///< ����� ������ ����.

        bool is_mode;     ///< ����������� �� ��� ����� �� ����� ��������.
        std::string name; ///< ���.

    private:
        bool active;
        u_int_4 dx_time;                ///< ����� ����, ������������ �� �����.

    public:
        /// ���������� ������� ��� ������������� ����� �����.
        void set_dx_time( u_int_4 dx_time );
    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������� �� ��������, ��������� �� �����.
///
/// � ������� (����, �����, ...) ����� ���� �������� ������ ���� ��������.
class operation_state
    {
    public:
        operation_state( const char* name, operation_manager *owner, int n );

        step* add_step( const char* name, int next_step_n,
            u_int step_duration_par_n );

        /// @brief ��������� �������� ����� �������� ��������������.
        ///
        /// @param idx - ������ ��������.
        ///
        /// @return - �������� �������� � �������� ��������. ���� ������
        /// ������� �� ��������, ������������ �������� �������� - ���� @ref
        /// mode::step_stub.
        step* operator[] ( int idx );

        int check_on( char* reason ) const;

        void init( u_int start_step = 1 );

        void evaluate();

        void final();

        void reset_eval_time()
            {
            dx_step_time = 0;
            }

        /// @brief ������� � ��������� ����.
        ///
        /// @param new_step - ����� ���� (� �������).
        /// @param cooperative_time - ����� ���������� ������ (���).
        void to_step( u_int new_step, u_long cooperative_time = 0 );

        u_long evaluation_time();

        u_long active_step_evaluation_time() const;
        u_long get_active_step_set_time() const;

        u_int active_step() const;
        u_int steps_count() const;

        /// ������� �� ������� ������.
        void print( const char* prefix = "" ) const;

        const char* get_name() const;

        int check_devices( char* err_dev_name, int str_len );

        int check_steps_params( char* err_dev_name, int str_len );

    private:
        std::string name;
        std::vector< step* > steps;

        step* mode_step;

        int active_step_n;           ///< �������� ���.
        int active_step_time;        ///< ����� ��������� ����.
        int active_step_next_step_n; ///< ��������� ���.

        /// @brief ������ ���������� ������ �����.
        std::vector< int > step_duration_par_ns;

        /// @brief ��������� ����.
        std::vector< int > next_step_ns;

        u_int_4 start_time; ///< ����� ������ ��������.
        step step_stub;     ///< ���-��������.

        operation_manager *owner;
        int n;              /// �����.

    private:
        /// ����� ���������� ��������� ����, ��� ������������� ����� �����.
        u_int_4 dx_step_time;

    public:
        /// ���������� ������� ���������� ��������� ���� ��� �������������
        /// ����� �����.
        void add_dx_step_time();

#ifndef __GNUC__
#pragma region ��������� ����� ��� ������ ��������� ����.
#endif

    public:
        void save();
        void load();

        int on_extra_step( int step_idx );

        int off_extra_step( int step_idx );

        /// @brief ���������� ��������� ��������� ���� � ��������� �������.
        ///
        /// @param off_step - ����� ������������ ���� (� �������).
        /// @param on_step - ����� ����������� ���� (� �������).
        int switch_active_extra_step( int off_step, int on_step );

        bool is_active_extra_step( int step_idx ) const;

    private:
        /// �������� ����. ����� ���� 1 ��� ����� �������������� �������� �����.
        std::vector< int > active_steps;

        std::vector< int > saved_active_steps;

#ifndef __GNUC__
#pragma endregion
#endif

    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������� �� ��������.
class operation
    {
    public:
        operation( const char* name, operation_manager *owner, int n );
#ifndef __GNUC__
#pragma region ������������� �� ������ �������.
#endif
        int check_devices_on_run_state( char* err_dev_name, int str_len );

        int check_on_run_state( char* reason ) const;

        u_long evaluation_time();

        void evaluate();

        void final();

        int check_steps_params( char* err_dev_name, int str_len );

        u_int active_step() const;
        u_int get_run_step() const;

        u_int get_run_steps_count() const;
        u_int get_run_active_step() const;

        const char* get_name() const;

        void print( const char* prefix /*= "" */ ) const;

        u_long active_step_evaluation_time() const;
        u_long get_active_step_set_time() const;

        /// @brief ������� � ���������  ����.
        ///
        /// @param new_step - ����� ���� (� �������).
        /// @param cooperative_time - ����� ���������� ������ (���).
        void to_step( unsigned int new_step, unsigned long cooperative_time = 0 );
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region ����� ���������������� �� ���������� ��������.
#endif

        enum state_idx
            {
            OFF = 0,//���������.
            RUN,    // ����������.
            PAUSE,  // �����.
            STOP,   // ����������.

			STATES_MAX,
            };

        static const char* state_str [];

        state_idx get_state() const;

        int pause();

        int stop();

        int start();
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region �������� ��������.
#endif
        /// @brief ��������� �������� ����� ��������������.
        ///
        /// @param idx - ������ ��������.
        operation_state* operator[] ( int idx )
            {
            if ( idx < STATES_MAX )
                {
                return states[ idx ];
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "Error operation_state* operation::operator[] "
                        "( int idx ) - idx %d > count %d.\n",
                        idx, STATES_MAX );
                    }
                return &stub;
                }
            }

    public:
        step* add_step( const char* name, int next_step_n,
            unsigned int step_duration_par_n, state_idx s_idx = RUN );

#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region ��������� ����� ��� ������ ��������� ����.
#endif

    public:
        int on_extra_step( int step_idx );

        int off_extra_step( int step_idx );

        /// @brief ���������� ��������� ��������� ���� � ��������� �������.
        ///
        /// @param off_step - ����� ������������ ���� (� �������).
        /// @param on_step - ����� ����������� ���� (� �������).
        int switch_active_extra_step( int off_step, int on_step );

        bool is_active_run_extra_step( int step_idx ) const;

#ifndef __GNUC__
#pragma endregion
#endif

    private:
        state_idx current_state;

        std::vector< operation_state* > states;

        std::string name;
        operation_manager *owner;
        int n;                                /// ����� �������� � ���������.

        operation_state stub;

        /// ��� ��� ��������� ����������. ����� ��� ������� ����� �����,
        /// ��������� � �.�.
        u_int run_step;

        u_int run_time;  /// ����� ���������� �������� (��������� run).
    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������� �� ��������� ������-���� ������� (����,
/// �����, ...).
///
/// � ������� (����, ...) ����� ���� �������� ���� ��������.
///
class operation_manager
    {
    public:
        /// @brief ����������� � �����������.
        ///
        /// @param modes_cnt - ���������� ��������.
        /// @param i_tech_object - ���������-��������.
        operation_manager( u_int modes_cnt, i_tech_object *owner );

        ~operation_manager();

        //NOTE ��������� ������ ������.
        operation* add_mode( const char* name )
            {
            return add_operation( name );
            }

        operation* add_operation( const char* name );

        /// @brief ��������� �������� ����� ��������������.
        ///
        /// @param idx - ������ ��������.
        ///
        /// @return - �������� �������� � �������� ��������. ���� ������
        /// ������� �� ��������, ������������ �������� ��������.
        operation* operator[] ( unsigned int idx );

        /// @brief ����� ����������� (��� ���������� ��������).
        ///
        /// @return - ����� ������� ��� �������� ��������.
        unsigned long get_idle_time();

        void reset_idle_time()
            {
            last_action_time = get_millisec();
            }

        /// @brief ���������� ����� ������� � �������.
        void print();

        void off_mode( int mode ) const
            {
            owner->set_mode( mode, 0 );
            }

        float get_step_param( u_int idx ) const
            {
            return owner->get_step_param( idx );
            }

        const saved_params_float* get_params() const
            {
            return owner->get_params();
            }

        /////TODO. ������� ����������������.
        ///// @brief ���������� ��������� ����������� ��������.
        /////
        ///// ��� ������ ��������:
        ///// 1. ���������, ���� ��� ��������, �� ����� �� �� ��������� ��
        ///// ���������������� ������� �����������. ���� ���, �� ���������
        ///// ����� ���������.
        ///// 2. ���� �������� �� ��������, �� ���������, ���� �� ���� �� ����
        ///// �������� �������� (��������).
        ///// 2.1 ���� ����, �� ��� ������ �������� �������� ���������, ����� ��
        ///// � ��� ����������� ��������� ������ ��������, ���� ���, �� ���������
        ///// ���������.
        ///// 3. ���� ��� �������� (��������) ��������, ��������� �� �����������
        ///// ��������� ����� ��������� ����������� �������� (��������), ����
        ///// ���, �� ��������� ���������.
        ////int refresh_availability( int *modes_states, int last_mode )
        ////    {
        ////    }

        i_tech_object *owner;              ///���������-��������.

    private:
        std::vector< operation* > operations; ///< ��������.
        operation *oper_stub;                 ///< ��������-��������.

        /// @brief ����� ���������� ���/���� ��������.
        u_int_4 last_action_time;

        static const char* UNKN_OPER_NAME;    ///��� ��� "�����������" ��������.
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
