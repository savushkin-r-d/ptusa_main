/// @file mode_mngr.h
/// @brief Содержит описания классов, которые используются для организации шагов
/// при выполнении режимов танка, гребенки.
/// 
/// Класс @ref step_path, служащий для организации работы с шагом, содержит всю  
/// необходимую информацию - список открываемых\закрываемых устройств,  
/// параметры шага, связанный с ним маршрут гребенки. Шаг - представляет ход  
/// протекания технологического процесса, включение\выключение которого  
/// происходит автоматически, без непосредственного участия оператора. В 
/// отличие от режима, который оператор включает\выключает сам. Для хранения 
/// всех режимов объекта служит класс @ref mode_manager. 
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

#include "dtime.h"

#include "PAC_dev.h"
#include "param_ex.h"

class tech_object;
//-----------------------------------------------------------------------------
class mode_manager;
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об устройствах, которые входят в шаг (открываются/
/// закрываются).
///
/// У режима может быть активным (выполняться) только один шаг.
class step_path
    {
    friend class mode_manager;
    public:
        step_path();

        /// @brief Добавление устройства, которое выключается\закрывается во
        ///  время выполнения шага.
        ///
        /// @param [in] dev - указатель на добавляемое устройство.        
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_closed_dev( device *dev );

        /// @brief Добавление устройства, которое включается\открывается во 
        /// время выполнения шага.
        ///
        /// @param [in] dev - указатель на добавляемое устройство.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_opened_dev( device *dev );

        int init();
        int evaluate() const;
        int final( );

        /// Получение времени начала шага.
        inline u_int_4 get_start_time() const;

        /// Установление времени начала шага.
        inline void set_start_time( u_int_4 start_time );

        /// Выводит на консоль объект.
        void print() const;

    private: 
        std::vector< device* > close_devices;   ///< Закрываемые устройства.   
        std::vector< device* > open_devices;    ///< Открываемые устройства.    
        
        u_int_4 start_time;                     ///< Время старта шага.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию о всех шагах какого-либо объекта (танк, 
/// гребенка).
/// 
/// У объекта (танк, ...) может быть включено параллельно несколько режимов.
class mode_manager
    {
    public:
        mode_manager( u_int_2 new_modes_cnt );

        ~mode_manager();
        
        /// Добавление параметра с временами шагов.
        int set_param( saved_params_u_int_4 *par );

        /// @brief Устанавливает количество шагов для режима.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int set_mode_config( u_int_2 mode, u_char new_steps_cnt );

        int init( u_int_2 mode, u_char start_step = 0 );
        int evaluate( u_int_2 mode );
        int final( u_int_2 mode );

        /// @brief Переход к шагу.
        ///
        /// @param [in] mode     - режим.
        /// @param [in] new_step - шаг, диапазон 0..255.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int to_step( u_int_2 mode, u_char new_step );

        /// @brief Добавление закрываемое устройство для шага.
        ///
        /// @param [in] mode - режим;
        /// @param [in] step - шаг, диапазон 0..255.
        /// @param [in] dev  - указатель на закрываемое устройство.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_closed_dev( u_int_2 mode, u_char step, device *dev );

        /// @brief Добавление открываемое устройство для шага.
        ///
        /// @param [in] mode - режим.
        /// @param [in] step - шаг, диапазон 0..255.
        /// @param [in] dev  - указатель на открываемое устройство.       
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_opened_dev( u_int_2 mode, u_char step, device *dev );

        /// @brief Добавление закрываемое устройство для режима.
        ///
        /// @param [in] mode - режим;
        /// @param [in] dev  - указатель на закрываемое устройство.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_mode_closed_dev( u_int_2 mode, device *dev );

        /// @brief Добавление открываемое устройство для режима.
        ///
        /// @param [in] mode - режим.
        /// @param [in] dev  - указатель на открываемое устройство.       
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_mode_opened_dev( u_int_2 mode, device *dev );

        /// @brief Проверка времени выполнения шага.
        ///
        /// @return  < 0 - ошибка.
        /// @return    0 - время не вышло.
        /// @return    1 - время вышло.
        int is_current_step_evaluation_time_left( u_int_2 mode );

        /// @brief Время выполнения шага.
        ///
        /// @return    1 - время выполнения шага.
        unsigned long get_current_step_evaluation_time( u_int_2 mode );

        /// @brief Время выполнения режима.
        ///
        /// @param [in] mode - режим.
        ///
        /// @return - время выполнения режима.
        unsigned long get_mode_evaluation_time( unsigned int mode );

        /// @brief Время бездействия (нет включенных режимов).
        ///
        /// @return - время системы без активных режимов.
        unsigned long get_idle_time();

        /// @brief Получение активного шага заданного режима.
        ///
        /// @return - активный шаг режима.
        u_int get_active_step( u_int mode );

        /// @brief Отладочный вывод объекта в консоль.
        void print();


        /// @brief Добавление проверяемой обратной связи при включении режима.
        ///
        /// @param [in] mode - режим;
        /// @param [in] dev  - указатель на проверяемую обратную связь.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_mode_on_FB( u_int_2 mode, device *dev );

        /// @brief Проверка на возможность включения режима.
        ///
        /// @param [in] mode - режим;
        ///
        /// @return false - ошибка.
        /// @return true  - ок.
        bool check_on_mode( u_int_2 mode );

    private:
        /// @brief Технологический объект.
        tech_object *owner;

        /// @brief Параметры, содержащие продолжительность шагов, режимов.
        saved_params_u_int_4 *par;      

        u_char  **step_duration_par_n;  ///< Номера параметров времени шага.    
        u_char  **next_step_n;          ///< Номера шагов, к которым перейти при  
                                        ///< завершении времени шагов.

        u_int_2 modes_cnt;              ///< Количество режимов.
        u_char  *steps_cnt;             ///< Количество шагов.

        std::vector < u_int_4 > modes_start_time; ///< Время начала режима.


        step_path **steps;              ///< Шаги для каждого режима.

        u_char  *active_step;           ///< Шаги, выполняемые в текущий момент.
        u_char  *is_active_mode;        ///< Активен ли режим.

        /// @brief Устройства режимов.
        std::vector < step_path* > modes_devices; 

        /// @brief Обратная связь для включения режима.
        std::vector < std::vector < device* > > modes_on_FB; 

        /// @brief Проверка, является ли номер режима и шага допустимым.
        ///
        /// @return -2 - номер режима выходит за пределы.
        /// @return -1 - номер шага выходит за пределы.
        /// @return  0 - оk.
        int check_correct_step_n( u_int_2 mode, u_char step );
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
