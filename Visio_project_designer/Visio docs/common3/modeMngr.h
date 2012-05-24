/// @file modeMngr.h
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
/// @c PAC - компиляция для контроллера.@n
/// @c WIN - компиляция для Windows.
/// 
/// @par Текущая версия:
/// @$Rev: 1631 $.\n
/// @$Author: id $.\n
/// @$Date:: 2012-01-12 13:16:57#$.

#ifndef MODEMNGR 
#define MODEMNGR 

#include "plc.h"

//-----------------------------------------------------------------------------
/// @brief Содержит информацию об устройствах, которые входят в шаг (открываются/
/// закрываются).
///
/// У режима может быть активным (выполняться) только один шаг.
class step_path
    {
    public:
        step_path( u_int_2 new_close_dev_cnt = 0, 
            u_int_2 new_open_device_cnt = 0 );

        /// Задание координат маршрута для шага.
        void set_path( char x1, char y1, char x2, char y2 );

        /// @brief Установка количества открываемых\закрываемых устройств.
        ///        
        /// @param new_close_dev_cnt - число закрываемых устройств.
        /// @param new_open_dev_cnt  - число открываемых устройств.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int set_dev_cnt( u_int_2 new_close_dev_cnt, u_int_2 new_open_dev_cnt );

        /// @brief Добавление устройства, которое выключается\закрывается во
        ///  время выполнения шага.
        ///
        /// @param [in] dev - указатель на добавляемое устройство.        
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_closed_dev( TVDO *dev );

        /// @brief Добавление устройства, которое включается\открывается во 
        /// время выполнения шага.
        ///
        /// @param [in] dev - указатель на добавляемое устройство.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_opened_dev( TVDO *dev );

        int init( void* object = 0, int mode = 0 );
        int evaluate() const;
        int final( );

#ifdef USE_SIMPLE_DEV_ERRORS
        /// @brief Проверка на наличие ошибки обратной связи устройств, которые
        /// включаются\открываются (выключаются\закрываются) во время выполнения
        /// шага.
        ///
        /// @param [out] err_dev_name - имена найденных устройств.
        ///
        /// @return > 0 - есть такие устройства.
        /// @return   0 - ок, таких устройств нет.
        int check_devices( char* err_dev_name, int max_to_write ) const;
#endif // USE_SIMPLE_DEV_ERRORS

        /// Возвращает время начала шага.
        inline u_int_4 get_start_time() const;

        /// Устанавливает время начала шага.
        inline void set_start_time( u_int_4 start_time );

        /// Выводит на консоль объект.
        void print() const;

    private:
        //  [ 1 ]
        /// @var close_devices_cnt
        /// @brief Количество закрываемых устройств.         
        /// @details Это свойство определяет размер буфера, в котором  
        /// хранятся указатели на закрываемые устройства. 
        //  [ 2 ]
        /// @var open_devices_cnt
        /// @brief Количество открываемых устройств.
        /// @details Это свойство определяет размер буфера, в котором  
        /// хранятся указатели на открываемые устройства.
        //  [ 3 ]
        /// @var last_close_dev
        /// @brief Номер последнего добавленного устройства. 
        //  [ 4 ]
        /// @var last_open_dev
        /// @brief Номер последнего добавленного устройства.
        //  [ 5 ]
        /// @var closeDevices
        /// @brief Закрываемые устройства. 
        //  [ 6 ]
        /// @var openDevices
        /// @brief Открываемые устройства.
        u_int_2 close_devices_cnt;  // 1   
        u_int_2 open_devices_cnt;   // 2
        u_int_2 last_close_dev;     // 3        
        u_int_2 last_open_dev;      // 4 
        TVDO    **closeDevices;     // 5    
        TVDO    **openDevices;      // 6       
        
        u_int_4 start_time;         ///< Время старта шага.

        char    in_x;               ///< Координата x начала маршрута.
        char    in_y;               ///< Координата y начала маршрута.
        char    out_x;              ///< Координата x конца маршрута.
        char    out_y;              ///< Координата y конца маршрута.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию о всех шагах какого-либо объекта (танк, 
/// гребенка).
/// 
/// У объекта (танк,...) может быть включено параллельно несколько режимов.
class mode_manager
    {
    public:
        TParams *par;       ///< Параметры, содержащие продолжительность шагов.

        mode_manager( u_int_2 new_modes_cnt );

        /// Добавляет параметры с временами шагов.
        int set_param( TParams *par );

        /// @brief Устанавливает количество шагов для режима.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int set_mode_config( u_int_2 mode, u_char new_steps_cnt );

        int init( u_int_2 mode, u_char start_step = 0, void *object = 0 );
        int evaluate( u_int_2 mode );
        int final( u_int_2 mode );

#ifdef USE_SIMPLE_DEV_ERRORS
        /// @brief Проверка на наличие ошибки обратной связи устройств, которые
        /// включаются\открываются (выключаются\закрываются) во время выполнения
        /// всех шагов режима.
        ///
        /// @param [in]  mode - номер режима.
        /// @param [out] err_dev_name - имена найденных устройство.
        ///
        /// @return > 0 - есть такие устройства.
        /// @return   0 - ок, таких устройств нет.
        int check_mode_devices( u_int_2 mode, char* err_dev_name, int str_len );
#endif // USE_SIMPLE_DEV_ERRORS

        /// @brief Переход к шагу.
        ///
        /// @param [in] mode     - режим.
        /// @param [in] new_step - шаг, диапазон 0..255.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int to_step( u_int_2 mode, u_char new_step );

        /// @brief Устанавливает параметры шага.
        ///
        /// @param [in] mode            - режим.
        /// @param [in] step            - шаг, диапазон 0..255.
        /// @param [in] close_dev_cnt   - количество закрываемых устройств.
        /// @param [in] open_device_cnt - количество открываемых устройств.
        /// @param [in] next_step       - номер шага, к выполнению которого  
        ///                               перейти при завершении времени  
        ///                               текущего шага.
        /// @param [in] par_n           - номер параметра, содержащего время 
        ///                               шага в мин.
        /// @param [in] x1              - координата x начала маршрута гребенки.
        /// @param [in] y1              - координата y начала маршрута гребенки.
        /// @param [in] x2              - координата x конца маршрута гребенки.
        /// @param [in] y2              - координата x конца маршрута гребенки.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int set_step_config( u_int_2 mode, u_char step, u_int_2 close_dev_cnt, 
            u_int_2 open_device_cnt, u_char next_step = 0,
            u_char par_n = 0, u_char x1 = 0, u_char y1 = 0,
            u_char x2 = 0, u_char y2 = 0 );

        /// @brief Добавляет закрываемое устройство для шага.
        ///
        /// @param [in] mode - режим;
        /// @param [in] step - шаг, диапазон 0..255.
        /// @param [in] dev  - указатель на закрываемое устройство.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_closed_dev( u_int_2 mode, u_char step, TVDO *dev );

        /// @brief Добавляет открываемое устройство для шага.
        ///
        /// @param [in] mode - режим.
        /// @param [in] step - шаг, диапазон 0..255.
        /// @param [in] dev  - указатель на открываемое устройство.       
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_opened_dev( u_int_2 mode, u_char step, TVDO *dev );

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

        int save( char *buff );
        int load( char *buff );

        int get_saved_size();
        int get_active_step( u_int_2 mode );


        void print();
			
		u_char  *steps_cnt;             ///< Количество шагов.	
				
    private:
        u_char  **step_duration_par_n;  ///< Номера параметров времени шага.    
        u_char  **next_step_n;          ///< Номера шагов, к которым перейти при  
                                        ///< завершении времени шагов.

        u_int_2 modes_cnt;              ///< Количество режимов.
        //u_char  *steps_cnt;             ///< Количество шагов.
		step_path **steps;              ///< Шаги для каждого режима.
		        
        u_char  *active_step;           ///< Шаги, выполняемые в текущий момент.
        u_char  *is_active_mode;        ///< Активен ли режим.

        /// @brief Проверяет, является ли номер режима и шага допустимым.
        ///
        /// @return -2 - номер режима выходит за пределы.
        /// @return -1 - номер шага выходит за пределы.
        /// @return  0 - оk.
        int check_correct_step_n( u_int_2 mode, u_char step );
    };
//-----------------------------------------------------------------------------
#endif // MODEMNGR