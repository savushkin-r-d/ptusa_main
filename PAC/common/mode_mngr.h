/// @file mode_mngr.h
/// @brief Содержит описания классов, которые используются для организации шагов
/// при выполнении операций танка, гребенки.
///
/// Класс @ref step, служащий для организации работы с шагом, содержит всю
/// необходимую информацию - список открываемых\закрываемых устройств,
/// параметры шага, связанный с ним маршрут гребенки. Шаг - представляет ход
/// протекания технологического процесса, включение\выключение которого
/// происходит автоматически, без непосредственного участия оператора. В
/// отличие от операции, который оператор включает\выключает сам. Для хранения
/// всех операций объекта служит класс @ref mode_manager.
/// Класс @ref action содержит списки устройств и выполняет над ними
/// определенные действия.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
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
/// @brief Действие над устройствами (включение, выключение и т.д.).
class action
    {
    public:
        action( std::string name, u_int subgropups_cnt = 1 );

        virtual ~action()
            {
            }

        /// @brief Проверка на отсутствие устройств.
        ///
        /// @return true  Есть устройства, над которыми что-то делается.
        /// @return false Нет устройств, над которыми что-то делается.
        virtual bool is_empty() const;

        virtual void print( const char* prefix = "", bool new_line = true ) const;

        /// @brief Проверка действия.
        ///
        /// @param [out] reason Пояснение, почему нельзя выполнить действие.
        virtual int check( char* reason ) const
            {
            reason[ 0 ] = 0;
            return 0;
            }

        /// @brief Инициализация действия.
        virtual void init() {}

        /// @brief Выполнение действия.
        virtual void evaluate() {}

        /// @brief Завершения действия.
        virtual void final();

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Группа устройства.
        /// @param [in] subgroup Подгруппа устройств.
        virtual void add_dev( device *dev, u_int group = MAIN_GROUP,
            u_int subgroup = MAIN_SUBGROUP );

        /// @brief Поиск устройства с какой-либо ошибкой.
        ///
        /// @param [out] err_dev_name Устройство.
        /// @param [in] max_to_write Дополнительный параметр.
        /// @return 0 - нет ошибок, 1 - есть ошибки.
        int check_devices( char* err_dev_name, int max_to_write ) const;

        /// @brief Установка параметров для действия.
        ///
        /// @param [in] par Параметры.
        virtual void set_params( const saved_params_float *par )
            {
            this->par = par;
            }

        /// @brief Задание индексов используемых параметров к действию.
        ///
        /// @param [in] position Позиция параметра.
        /// @param [in] idx Индекс параметра.
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

        // Устройства.
        std::vector < std::vector< std::vector< device* > > > devices;
        std::string name;                               ///< Имя действия.

        const saved_params_float *par;      ///< Параметры действия.
        std::vector< int >        par_idx;  ///< Индексы параметров действия.
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Включение устройств.
/// </summary>
class on_action: public action
    {
    public:
        on_action(): action( "Включать" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Включение реверсных устройств.
/// </summary>
class on_reverse_action : public action
    {
    public:
        on_reverse_action() : action( "Включать реверс" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Выключение устройств.
/// </summary>
class off_action: public action
    {
    public:
        off_action(): action( "Выключать" )
            {
            }

        void evaluate();

        void init();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Промывка седел.
/// </summary>
class open_seat_action: public action
    {
    public:
        open_seat_action( bool is_mode, operation_state *owner );

        void init();
        void evaluate();
        void final();


        void set_wait_time( int wait_time );

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Дополнительный параметр.
        /// @param [in] seat_type Дополнительный параметр.
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

        PHASES phase;      ///< Текущий этап.
        PHASES next_phase; ///< Следующий этап.

        u_int     active_group_n;  ///< Номер промываемой сейчас группы.

        u_int_4 wait_time;      ///< Время ожидания перед промыванием седел.
        u_int_4 wait_seat_time; ///< Время ожидания перед промыванием седел группы.
        u_int_4 wash_time_upper;///< Время промывки верхних седел текущей группы клапанов.
        u_int_4 wash_time_lower;///< Время промывки нижних седел текущей группы клапанов.

        /// Седла.
        std::vector< std::vector< device* > > wash_upper_seat_devices;
        std::vector< std::vector< device* > > wash_lower_seat_devices;

        u_int_4 start_cycle_time; ///< Время старта цикла (ожидания или промывки).

        bool is_mode;             ///< Является ли шагом операции.
        operation_state* owner;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Пары DI->DO.
/// </summary>
class DI_DO_action: public action
    {
    public:
        DI_DO_action( ):action( "Группы DI->DO's" )
            {
            }

        int check( char* reason ) const;

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Пары AI->AO.
/// </summary>
class AI_AO_action : public action
    {
    public:
        AI_AO_action() :action( "Группы AI->AO's" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Проверка дискретных входных сигналов.
/// </summary>
class required_DI_action: public action
    {
    public:
        required_DI_action(): action( "Сигналы для включения" )
            {
            }

        int check( char* reason ) const;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Мойка линии.
/// </summary>
class wash_action: public action
    {
    public:
        wash_action(): action( "Устройства DI's DO's DEV's R_DEV's AI", G_SUBGROUPS_CNT )
            {
            }

        void evaluate();

        virtual void print( const char* prefix = "", bool new_line = true ) const;

    private:
        enum GROUPS
            {
            G_DI = 0,       //Входные сигналы запроса включения устройств.
            G_DO,           //Выходные сигналы "ОК".
            G_DEV,          //Устройства, включаемые по запросу.
            G_REV_DEV,      //Реверсные устройства, включаемые по запросу.
            G_PUMP_FREQ,    //Задания производительности.

            G_SUBGROUPS_CNT,   //Количество групп.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об устройствах, которые входят в шаг (открываются/
/// закрываются).
///
/// У операции может быть активным (выполняться) только один шаг.
/// Также дополнительно можно включать 1 либо более шагов параллельно.
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

        /// @brief Получение действия через операцию индексирования.
        ///
        /// @param idx - индекс действия.
        ///
        /// @return - значение действия с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение 0.
        action* operator[] ( int idx );

        /// @brief Проверка возможности выполнения шага.
        ///
        /// @return > 0 - нельзя выполнить.
        /// @return   0 - ок.
        int check( char* reason ) const;

        void init();

        void evaluate() const;

        void final();

        /// Получение времени выполнения шага.
        u_int_4 get_eval_time() const;

        /// Установление времени начала шага.
        void set_start_time( u_int_4 start_time );

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;

        /// @brief Проверка на отсутствие устройств.
        ///
        /// @return true  Есть устройства, над которыми что-то делается.
        /// @return false Нет устройств, над которыми что-то делается.
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
        std::vector< action* > actions; ///< Действия.
        action action_stub;             ///< Фиктивное действие.
        u_int_4 start_time;             ///< Время старта шага.

        bool is_mode;     ///< Выполняется ли все время во время операции.
        std::string name; ///< Имя.

    private:
        bool active;
        u_int_4 dx_time;                ///< Время шага, отработанное до паузы.

    public:
        /// Добавление времени для возобновления после паузы.
        void set_dx_time( u_int_4 dx_time );
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об операции, состоящей из шагов.
///
/// У объекта (танк, линия, ...) может быть включена только одна операция.
class operation_state
    {
    public:
        operation_state( const char* name, operation_manager *owner, int n );

        step* add_step( const char* name, int next_step_n,
            u_int step_duration_par_n );

        /// @brief Получение операции через операцию индексирования.
        ///
        /// @param idx - индекс операции.
        ///
        /// @return - значение операции с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
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

        /// @brief Переход к заданному шагу.
        ///
        /// @param new_step - номер шага (с единицы).
        /// @param cooperative_time - время совместной работы (сек).
        void to_step( u_int new_step, u_long cooperative_time = 0 );

        /// @brief Переход к следующему шагу.
        void to_next_step();

        u_long evaluation_time();

        u_long active_step_evaluation_time() const;
        u_long get_active_step_set_time() const;

        u_int active_step() const;
        u_int steps_count() const;

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;

        const char* get_name() const;

        int check_devices( char* err_dev_name, int str_len );

        int check_steps_params( char* err_dev_name, int str_len );

    private:
        std::string name;
        std::vector< step* > steps;

        step* mode_step;

        int active_step_n;           ///< Активный шаг.
        int active_step_time;        ///< Время активного шага.
        int active_step_next_step_n; ///< Следующий шаг.

        /// @brief Номера параметров времен шагов.
        std::vector< int > step_duration_par_ns;

        /// @brief Следующие шаги.
        std::vector< int > next_step_ns;

        u_int_4 start_time; ///< Время начала операции.
        step step_stub;     ///< Шаг-заглушка.

        operation_manager *owner;
        int n;              /// Номер.

    private:
        /// Время выполнения активного шага, для возобновления после паузы.
        u_int_4 dx_step_time;

    public:
        /// Добавление времени выполнения активного шага при возобновлении
        /// после паузы.
        void add_dx_step_time();

#ifndef __GNUC__
#pragma region Поддержка более чем одного активного шага.
#endif

    public:
        void save();
        void load();

        int on_extra_step( int step_idx );

        int off_extra_step( int step_idx );

        /// @brief Выключение заданного активного шага и включение другого.
        ///
        /// @param off_step - номер выключаемого шага (с единицы).
        /// @param on_step - номер включаемого шага (с единицы).
        int switch_active_extra_step( int off_step, int on_step );

        /// @brief Определение активности заданного шага.
        ///
        /// @param step_idx - номер выключаемого шага (с единицы).
        bool is_active_extra_step( int step_idx ) const;

    private:
        /// Активные шаги. Может быть 1 или более дополнительных активных шагов.
        std::vector< int > active_steps;

        std::vector< int > saved_active_steps;

#ifndef __GNUC__
#pragma endregion
#endif

    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об операции.
class operation
    {
    public:
        operation( const char* name, operation_manager *owner, int n );
#ifndef __GNUC__
#pragma region Совместимость со старой версией.
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

        /// @brief Переход к заданному  шагу.
        ///
        /// @param new_step - номер шага (с единицы).
        /// @param cooperative_time - время совместной работы (сек).
        void to_step( unsigned int new_step, unsigned long cooperative_time = 0 );

        /// @brief Переход к следующему шагу.
        ///
        /// Данный метод упрощает реализацию, когда необходимо использовать
        /// функцию to_step, передавая в качестве параметра номер текущего шага,
        /// увеличенного на единицу.
        void to_next_step();
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region Новая функциональность по состояниям операции.
#endif

        enum state_idx
            {
            OFF = 0,//Отключено.
            RUN,    // Выполнение.
            PAUSE,  // Пауза.
            STOP,   // Остановлен.

			STATES_MAX,
            };

        static const char* state_str [];
        static const char* en_state_str[];

        state_idx get_state() const;

        int pause();

        int stop();

        int start();
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region Загрузка описания.
#endif
        /// @brief Получение операции через индексирование.
        ///
        /// @param idx - индекс операции.
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
#pragma region Поддержка более чем одного активного шага.
#endif

    public:
        int on_extra_step( int step_idx );

        int off_extra_step( int step_idx );

        /// @brief Выключение заданного активного шага и включение другого.
        ///
        /// @param off_step - номер выключаемого шага (с единицы).
        /// @param on_step - номер включаемого шага (с единицы).
        int switch_active_extra_step( int off_step, int on_step );

        /// @brief Определение активности заданного шага для выполнения
        /// операции.
        ///
        /// @param step_idx - номер выключаемого шага (с единицы).
        bool is_active_run_extra_step( int step_idx ) const;

        /// @brief Определение активности заданного шага.
        ///
        /// @param step_idx - номер выключаемого шага (с единицы).
        bool is_active_extra_step( int step_idx ) const;

#ifndef __GNUC__
#pragma endregion
#endif

    private:
        state_idx current_state;

        std::vector< operation_state* > states;

        std::string name;
        operation_manager *owner;
        int n;                                /// Номер операции у владельца.

        operation_state stub;

        /// Шаг для состояния Выполнение. Нужен для запуска после паузы,
        /// остановки и т.д.
        u_int run_step;

        u_int run_time;  /// Время выполнения операции (состояние run).
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об операциях какого-либо объекта (танк,
/// линия, ...).
///
/// У объекта (танк, ...) может быть включена одна операция.
///
class operation_manager
    {
    public:
        /// @brief Конструктор с параметрами.
        ///
        /// @param modes_cnt - количество операций.
        /// @param i_tech_object - техобъект-владелец.
        operation_manager( u_int modes_cnt, i_tech_object *owner );

        ~operation_manager();

        //NOTE Поддержка старой версии.
        operation* add_mode( const char* name )
            {
            return add_operation( name );
            }

        operation* add_operation( const char* name );

        /// @brief Получение операции через индексирование.
        ///
        /// @param idx - индекс операции.
        ///
        /// @return - значение операции с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки.
        operation* operator[] ( unsigned int idx );

        /// @brief Время бездействия (нет включенных операций).
        ///
        /// @return - время системы без активных операций.
        unsigned long get_idle_time();

        void reset_idle_time()
            {
            last_action_time = get_millisec();
            }

        /// @brief Отладочный вывод объекта в консоль.
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

        /////TODO. Будущая функциональность.
        ///// @brief Обновление состояния доступности операций.
        /////
        ///// Для каждой операции:
        ///// 1. Проверяем, если она включена, то можно ли ее отключить из
        ///// соответствующего массива доступности. Если нет, то формируем
        ///// также пояснение.
        ///// 2. Если операция не включена, то проверяем, есть ли хотя бы одна
        ///// активная операция (основная).
        ///// 2.1 Если есть, то для каждой активной операции проверяем, можно ли
        ///// с ней параллельно запустить данную операцию, если нет, то формируем
        ///// пояснение.
        ///// 3. Если нет активных (основных) операций, проверяем на возможность
        ///// включения после последней выключенной операции (основной), если
        ///// нет, то формируем пояснение.
        ////int refresh_availability( int *modes_states, int last_mode )
        ////    {
        ////    }

        i_tech_object *owner;              ///Техобъект-владелец.

    private:
        std::vector< operation* > operations; ///< Операции.
        operation *oper_stub;                 ///< Операция-заглушка.

        /// @brief Время последнего вкл/выкл операции.
        u_int_4 last_action_time;

        static const char* UNKN_OPER_NAME;    ///Имя для "неизвестной" операции.
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
