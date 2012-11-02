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
        action( std::string name );
        
        virtual ~action()
            {            
            }

        virtual void print( const char* prefix = "" ) const;

        /// @brief Проверка действия.
        virtual int check() const 
            {
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
        virtual void add_dev( device *dev )
            {
            devices.push_back( dev );
            }

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Дополнительный параметр.
        /// @param [in] subgroup Дополнительный параметр.
        virtual void add_dev( device *dev, u_int group, u_int subgroup ) {}
        

    protected: 
        std::vector< device* > devices; ///< Устройства.   
        std::string name;               ///< Имя действия.
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

    private:
        enum PHASES
            {
            P_WAIT = 0,            
            P_OPEN_UPPER,            
            P_OPEN_LOWER,
            };

        PHASES phase;      ///< Текущий этап.
        PHASES next_phase; ///< Следуюший этап.

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
        DI_DO_action( ):action( "Пары DI->DO" )
            {
            }

        /// @brief Проверка действия.
        int check() const;

        void evaluate();

        void final();
                
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

        int check() const;
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
            A_PAIR_DO_DI,
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
        int check() const;

        void init();

        void evaluate() const;

        void final() const;

        /// Получение времени начала шага.
        u_int_4 get_start_time() const;

        /// Установление времени начала шага.
        void set_start_time( u_int_4 start_time );

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;
            
    private: 
        std::vector< action* > actions; ///< Действия.           
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
        mode( const char* name, mode_manager *owner );

        step* add_step( const char* name, u_int next_step_n,
            u_int step_duration_par_n );

        /// @brief Получение режима через операцию индексирования.
        ///
        /// @param idx - индекс режима.
        ///
        /// @return - значение режима с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// mode::step_stub.
        step* operator[] ( int idx );

        int check_on() const;

        void init( u_int start_step = 0 );

        void evaluate();

        void final();

        void to_step( u_int new_step );

        u_long evaluation_time()
            {
            return get_delta_millisec( start_time );
            }

        u_int active_step() const
            {
            return active_step_n + 1;
            }

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;

        const std::string* get_name() const
            {
            return &name;
            }

    private:
        std::string name;
        std::vector< step* > steps;

        step* mode_step;
        u_int active_step_n;
        
        /// @brief Номера параметров времени шага.
        std::vector< u_int > step_duration_par_ns; 

        /// @brief Следующий шаг.
        std::vector< u_int > next_step_ns;

        u_int_4 start_time; ///< Время начала режима.


        step step_stub; ///< Шаг-заглушка.

        mode_manager *owner;
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
        
        const char* get_mode_name( int n ) const
            {
            return modes[ n ]->get_name()->c_str();
            }

        mode* add_mode( const char* name );

        void set_param( saved_params_u_int_4 *par );

        saved_params_u_int_4 * get_param() const;

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
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
