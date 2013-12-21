/// @file mode_mngr.h
/// @brief Содержит описания классов, которые используются для организации шагов
/// при выполнении режимов танка, гребенки.
///
/// Класс @ref step, служащий для организации работы с шагом, содержит всю
/// необходимую информацию - список открываемых\закрываемых устройств,
/// параметры шага, связанный с ним маршрут гребенки. Шаг - представляет ход
/// протекания технологического процесса, включение\выключение которого
/// происходит автоматически, без непосредственного участия оператора. В
/// отличие от режима, который оператор включает\выключает сам. Для хранения
/// всех режимов объекта служит класс @ref mode_manager.
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

class mode_manager;
//-----------------------------------------------------------------------------
/// @brief Действие над устройствами (включение, выключение и т.д.).
class action
    {
    public:
        action( std::string name, u_int group_cnt = 1 );

        virtual ~action()
            {
            }

        /// @brief Проверка на отсутствие устройств.
        ///
        /// @return true  Есть устройства, над которыми что-то делается.
        /// @return false Нет устройств, над которыми что-то делается.
        virtual bool is_empty() const;

        virtual void print( const char* prefix = "" ) const;

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
        ///
        /// @return результат выполнения действия. Трактуется в зависимости от
        /// действия.
        virtual void evaluate() {}

        /// @brief Завершения действия.
        virtual void final();

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Группа устройства.
        virtual void add_dev( device *dev, u_int group = 0 );

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Дополнительный параметр.
        /// @param [in] subgroup Дополнительный параметр.
        virtual void add_dev( device *dev, u_int group, u_int subgroup ) {}

        /// @brief Поиск устройства с какой-либо ошибкой.
        ///
        /// @param [out] err_dev_name Устройство.
        /// @param [in] max_to_write Дополнительный параметр.
        /// @return 0 - нет ошибок, 1 - есть ошибки.
        int check_devices( char* err_dev_name, int max_to_write ) const;

    protected:
        std::vector< std::vector< device* > > devices;  ///< Устройства.
        std::string name;                               ///< Имя действия.
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
/// Выключение устройств.
/// </summary>
class off_action: public action
    {
    public:
        off_action(): action( "Выключать" )
            {
            }

        void evaluate();
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Промывка седел.
/// </summary>
class open_seat_action: public action
    {
    public:
        open_seat_action();

        void init();
        void evaluate();
        void final();

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Дополнительный параметр.
        /// @param [in] seat_type Дополнительный параметр.
        void add_dev( device *dev, u_int group, u_int seat_type );

        void print( const char* prefix = "" ) const;

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
        u_int_4 wash_time;      ///< Время промывки седел текущей группы клапанов.

        /// Седла.
        std::vector< std::vector< device* > > wash_upper_seat_devices;
        std::vector< std::vector< device* > > wash_lower_seat_devices;

        u_int_4 start_cycle_time; ///< Время старта цикла (ожидания или промывки).
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Пары DI->DO.
/// </summary>
class DI_DO_action: public action
    {
    public:
        DI_DO_action( ):action( "Группы DI->DO DO ..." )
            {
            }

        void evaluate();

        void print( const char* prefix = "" ) const;
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

        void final()
            {
            }
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Мойка линии.
/// </summary>
class wash_action: public action
    {
    public:
        wash_action(): action( "Мойка", 3 )
            {
            }

        void final();

        void evaluate();

        void print( const char* prefix /*= "" */ ) const;

    private:
        enum GROUPS
            {
            G_DI = 0, //Входные сигналы запроса включения устройств.
            G_DO,     //Выходные сигналы "Мойка ОК".
            G_DEV,    //Устройства, включаемые по запросу.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об устройствах, которые входят в шаг (открываются/
/// закрываются).
///
/// У режима может быть активным (выполняться) только один шаг.
class step
    {
    public:
        enum ACTIONS
            {
            A_ON = 0,
            A_OFF,
            A_UPPER_SEATS_ON,
            A_LOWER_SEATS_ON = A_UPPER_SEATS_ON,

            A_REQUIRED_FB,
            A_DO_DI,
            A_WASH,
            };

        step( std::string name, bool is_mode = false );

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

        void evaluate_on() const 
            {
            actions[ A_ON ]->evaluate();
            actions[ A_LOWER_SEATS_ON /*A_UPPER_SEATS_ON*/ ]->evaluate();
            };

        void evaluate_off() const 
            {
            actions[ A_OFF ]->evaluate();            
            };

        void final() const;

        /// Получение времени начала шага.
        u_int_4 get_start_time() const;

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

    private:
        std::vector< action* > actions; ///< Действия.
        action action_stub;             ///< Фиктивное действие.
        u_int_4 start_time;             ///< Время старта шага.

        bool is_mode;     ///< Выполняется ли все время во время режима.
        std::string name; ///< Имя.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию о режиме, состоящем из шагов.
///
/// У объекта (танк, ...) может быть включено параллельно несколько режимов.
class mode
    {
    public:
        mode( const char* name, mode_manager *owner, int n );

        step* add_step( const char* name, u_int next_step_n,
            u_int step_duration_par_n );

        /// @brief Установка номера параметра со временем переходного переключения шагов.
        void set_step_cooperate_time_par_n( int step_cooperate_time_par_n )
            {
            this->step_cooperate_time_par_n = step_cooperate_time_par_n;
            }

        /// @brief Получение режима через операцию индексирования.
        ///
        /// @param idx - индекс режима.
        ///
        /// @return - значение режима с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// mode::step_stub.
        step* operator[] ( int idx );

        int check_on( char* reason ) const;

        void init( u_int start_step = 1 );

        void evaluate();

        void final();

        void to_step( u_int new_step, u_long cooperative_time = 0 );

        u_long evaluation_time()
            {
            return get_delta_millisec( start_time );
            }

        u_long active_step_evaluation_time() const
            {
            if ( active_step_n >= 0 )
                {
                return get_delta_millisec( steps[ active_step_n ]->get_start_time() );
                }

            return 0;
            }

        u_int active_step() const
            {
            return active_step_n + 1;
            }

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;

        const char* get_name() const
            {
            return name.c_str();
            }

        int check_devices( char* err_dev_name, int str_len );
    private:
        std::string name;
        std::vector< step* > steps;

        step* mode_step;

        int active_step_n;           ///< Активный шаг.
        int active_step_time;        ///< Время активного шага.
        int active_step_next_step_n; ///< Следующий шаг.

        int active_step_second_n;             ///< Параллельный активный шаг.
        int active_step_second_start_time;    ///< Параллельный активный шаг.
        
        /// @brief Время переходного времени шагов.
        u_int step_cooperate_time;

        /// @brief Номер параметра со временем переходного времени шагов.
        int step_cooperate_time_par_n;

        /// @brief Номера параметров времен шагов.
        std::vector< int > step_duration_par_ns;

        /// @brief Следующие шаги.
        std::vector< int > next_step_ns;

        u_int_4 start_time; ///< Время начала режима.
        step step_stub;     ///< Шаг-заглушка.

        mode_manager *owner;
        int n;          ///< Номер.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию о всех режимах какого-либо объекта (танк,
/// гребенка).
///
/// У объекта (танк, ...) может быть включено параллельно несколько режимов.
class mode_manager
    {
    public:
        mode_manager( u_int modes_cnt );

        ~mode_manager();

        mode* add_mode( const char* name );

        void set_param( saved_params_u_int_4 *par );

        saved_params_u_int_4* get_param() const;

        /// @brief Получение режима через операцию индексирования.
        ///
        /// @param idx - индекс режима.
        ///
        /// @return - значение режима с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// mode_manager::mode_stub.
        mode* operator[] ( unsigned int idx );

        /// @brief Время бездействия (нет включенных режимов).
        ///
        /// @return - время системы без активных режимов.
        unsigned long get_idle_time();

        /// @brief Отладочный вывод объекта в консоль.
        void print();

    private:
        /// @brief Параметры, содержащие продолжительность шагов, режимов.
        saved_params_u_int_4 *par;

        std::vector< mode* > modes; ///< Режимы.

        mode *mode_stub;            ///< Режим-заглушка.

        u_int_4 last_action_time;   ///Время последнего вкл/выкл режима.

        static const char* UNKN_MODE_NAME; //Имя для "неизвестного" режима.
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
