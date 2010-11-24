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
/// @$Rev: 1631 $.\n
/// @$Author: id $.\n
/// @$Date:: 2010-03-06 14:43:44#$.

#ifndef MODE_MNGR 
#define MODE_MNGR 

#include "PAC_dev.h"
#include "param_ex.h"

//-----------------------------------------------------------------------------
/// @brief Содержит информацию об устройствах, которые входят в шаг (открываются/
/// закрываются).
///
/// У режима может быть активным (выполняться) только один шаг.
class step_path
    {
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
        std::vector< device* > close_devices;  ///< Закрываемые устройства.   
        std::vector< device* > open_devices;   ///< Открываемые устройства.    
        
        u_int_4 start_time;                   ///< Время старта шага.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию о всех шагах какого-либо объекта (танк, 
/// гребенка).
/// 
/// У объекта (танк, ...) может быть включено параллельно несколько режимов.
class mode_manager
    {
    public:
        saved_params_u_int_4 *par;       ///< Параметры, содержащие продолжительность шагов.

        mode_manager( u_int_2 new_modes_cnt );

        /// Добавление параметра с временами шагов.
        int set_param( saved_params_u_int_4 *par );

        /// @brief Устанавливает количество шагов для режима.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int set_mode_config( u_int_2 mode, u_char new_steps_cnt );

        int init( u_int_2 mode, u_char start_step = 0, void *object = 0 );
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

        int get_active_step( u_int_2 mode );

        void print();

    private:
        u_char  **step_duration_par_n;  ///< Номера параметров времени шага.    
        u_char  **next_step_n;          ///< Номера шагов, к которым перейти при  
                                        ///< завершении времени шагов.

        u_int_2 modes_cnt;              ///< Количество режимов.
        u_char  *steps_cnt;             ///< Количество шагов.

        step_path **steps;              ///< Шаги для каждого режима.

        u_char  *active_step;           ///< Шаги, выполняемые в текущий момент.
        u_char  *is_active_mode;        ///< Активен ли режим.

        /// @brief Проверка, является ли номер режима и шага допустимым.
        ///
        /// @return -2 - номер режима выходит за пределы.
        /// @return -1 - номер шага выходит за пределы.
        /// @return  0 - оk.
        int check_correct_step_n( u_int_2 mode, u_char step );
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
