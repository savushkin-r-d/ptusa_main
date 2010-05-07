/// @file g_device.h
/// @brief Содержит описания классов, которые используются совместно в
/// контроллере и драйвере на сервере.
///
/// Описываются базовые интерфейсы, базовые классы.
/// Пример устройства:
///   GLOB0V512  - GLOB 0 V 512 - Клапан номер 512 среди устройств общих
///   устройств PAC;
///   TANK1PAR12 - TANK 1 PAR 12 - Параметр 1 танка номер 1.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @c DRIVER  - компиляция для драйвера в OS Windows.@n
/// @c PAC     - компиляция для PAC.@n
/// @c W750    - компиляция для PAC Wago 750.@n
/// @c I7186_E - компиляция для PAC I7186_E.@n
/// @c I7188_E - компиляция для PAC I7188_E.@n
/// @c I7188   - компиляция для PAC I7188. Данные директивы кроме необходимой 
/// функциональности определяют специфичную для платформы реализацию работы с
/// консолью (@ref Getch и @ref Printf), куда выводятся сообщения об ошибках в
/// отладочном режиме. Также они определяют разрядность базовых типов
/// данных (@ref int_2, @ref u_int_4, @ref u_int_2).
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
#ifndef GENERAL_DEVICE_H
#define GENERAL_DEVICE_H

#ifndef PAC 
#ifndef DRIVER
#define PAC
#endif // DRIVER
#endif // PAC 

#ifdef DRIVER
#include "CmnHdr.h"
typedef short           int_2;

typedef unsigned int    u_int_4;
typedef unsigned short  u_int_2;
#endif // DRIVER

#ifdef PAC
#include "sys.h"
#include "tcp_cmctr.h"

#ifdef I7188
#include "sys_7188.h"
#endif // I7188

#ifdef I7188_E
#include "sys7188e.h"
#endif // I7188

#ifdef I7186_E
#include "sys_7186.h"
#endif // I7186_E

#ifdef W750
#include "sys_w750.h"
#endif // W750

#endif // PAC

//-----------------------------------------------------------------------------
/// @brief Вывод сообщения на консоль (для PAC), или в окно драйвера (для PC).
///
/// @param err_str    - сообщение.
/// @param is_need_CR - флаг перевода каретки на новую строку.
void print_str( char *err_str, char is_need_CR = 1 );
//-----------------------------------------------------------------------------
/// @brief Итерфейс устройства, позволяющий сохранить его в потоке байтов.
class i_save_device    
    {    
    public:
        /// @brief Сохранение состояния устройства в буфер.
        ///
        /// @param buf [ out ] - адрес буфера, куда будут записываться данные.
        ///        
        /// @return >= 0 - количество записанных байт.
        virtual int save_state( char *buff ) = 0;

        /// @brief Сохранение измененного состояния устройства в буфер.
        ///
        /// @param buf [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        virtual int save_changed_state( char *buff ) = 0;

        /// @brief Сохранение самого устройства в буфер.
        ///
        /// @param buf [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        virtual int save_device( char *buff ) = 0;      
    };
//-----------------------------------------------------------------------------
#ifdef DRIVER
/// @brief Итерфейс устройства, позволяющий считать его из потока байтов.
class i_load_device    
    {    
    public:
        /// @brief Считывание состояния устройства из буфера.
        ///
        /// @param buf [ out ] - адрес буфера, откуда считываются данные.
        ///
        /// @return >= 0 - количество считанных байт.
        virtual int load_state( char *buff ) = 0;

        /// @brief Считывание измененного состояния устройства из буфера.
        ///
        /// @param buf [ out ] - адрес буфера, откуда считываются данные.
        ///
        /// @return >= 0 - количество считанных байт.
        virtual int load_changed_state( char *buff ) = 0;  

        /// @brief Считывание самого устройства из буфера.
        ///
        /// @param buf [ out ] - адрес буфера, откуда считываются данные.
        ///
        /// @return >= 0 - количество считанных байт.
        virtual int load_device( char *buff ) = 0; 
    };
#endif // DRIVER
//-----------------------------------------------------------------------------
/// @brief Итерфейс устройства, которое выполняет команду, сохраненную  в
/// потоке байтов.
class i_cmd_device    
    {    
    public:
        /// @brief Выполнение команды, хранящейся в буфере.
        ///
        /// @param buf [ out ] - адрес буфера, откуда считываются данные.
        ///
        /// @return >= 0 - количество считанных байт.
        virtual int parse_cmd( char *buff  ) = 0;    
    };
//-----------------------------------------------------------------------------
/// @brief Итерфейс устройства, которое характеризуется некоторым значением.
///
/// Устройство хранится в массиве.
class i_device    
    {    
    public:
#ifdef DRIVER
        /// @brief Получение номера устройства в массиве устройств.
        virtual u_int_4 get_idx() = 0;                  

        /// @brief Установление номера устройства в массиве устройств.
        virtual void set_idx( u_int_4 new_idx ) = 0;
#endif // DRIVER

        /// @brief Возвращает номер устройства.
        virtual u_int_4 get_n() const = 0;

        /// @brief Вывод на консоль устройства.
        virtual void print() const = 0;         
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс простого устройства.
class i_simple_device: public i_device, 
        public i_cmd_device,
#ifdef DRIVER
        public i_load_device,
#endif // DRIVER
        public i_save_device
    {     
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс сложного устройства. Оно состоит из групп простых
/// устройств.
class i_complex_device: public i_simple_device
    {
    public:

#ifdef DRIVER
        /// Устройства, которые входят данное сложное устройство.
        i_complex_device  *compl_dev;  
#endif // DRIVER

        /// Типы сложных устройств.
        enum TYPES
            {
            COMPLEX_DEV,        ///< Сложное устройство.
            GROUP_DEV_BYTE,     ///< Группа устройств с размерностью данных 1 байт, тип byte.
            GROUP_DEV_LONG,     ///< -//- 4 байта, тип unsigned long.
            GROUP_DEV_FLOAT = 4,///< -//- 4 байта, тип float.
            ARRAY_DEV_BYTE,     ///< Сложное устройство. Данные в виде массива, тип byte.
            ARRAY_DEV_FLOAT,    ///< -//- тип float.
            ARRAY_DEV_LONG,     ///< -//- тип long int.
            ARRAY_DEV_UINT,     ///< -//- тип unsigned int.

            ARRAY_DEV_INT,      ///< -//- тип int.

            ARRAY_DEV_STR,      ///< -//- тип строка.
            ARRAY_DEV_ULONG,    ///< -//- тип unsigned long int.
            };

        virtual char            get_type() const = 0;
        virtual u_int_4         get_n() const = 0;
        virtual const char*     get_name() const = 0;
        virtual u_int_4         get_subdev_quantity() const = 0;
        virtual i_save_device*  get_save_dev( u_int_4 idx ) const = 0;

        virtual int             set_type( char new_type ) = 0;
        virtual int             set_n( u_int_4 new_n ) = 0;
        virtual int             set_name( char * new_name ) = 0;
        virtual int             set_subdev_quantity( u_int_4 new_dev_cnt ) = 0;
#ifdef DRIVER
        virtual i_load_device*  get_load_dev( u_int_4 idx ) = 0;
#endif // DRIVER        
    };
//-----------------------------------------------------------------------------
/// @brief Базовое сложное устройства.
class complex_device: public i_complex_device
    {
    public:
        i_simple_device **sub_dev;  ///< Подустройства.

    protected:
        u_int_4     n;              ///< Уникальный номер.
        u_int_4     sub_dev_cnt;    ///< Количество подустройств.
        static int  MAX_NAME_LENGTH;///< Максимальная длина имени.
        char        *name;          ///< Имя.

        char        type;           ///< Тип.
        u_int_4     idx;            ///< Номер устройства массиве устройств.

    public:      
        complex_device();
        complex_device( u_int_4 n, char *new_name, u_int_2 new_subdev_cnt, 
            char type );

#ifdef DRIVER
        ~complex_device();

        complex_device( u_int_4 n, char *new_name, u_int_2 new_subdev_cnt, 
            char type, i_complex_device *owner_compl_dev );
#endif // DRIVER

        char            get_type() const;
        u_int_4         get_n() const;
        const char*     get_name() const;
        u_int_4         get_subdev_quantity() const;
        i_save_device*  get_save_dev( u_int_4 idx ) const;

        /// @brief Метод интерфейса @ref i_save_device.
        int save_device( char *buff );

        /// @brief Метод интерфейса @ref i_save_device.
        int save_changed_state( char *buff );

        /// @brief Метод интерфейса @ref i_save_device.
        int save_state( char *buff );

        int             set_type( char new_type );
        int             set_n( u_int_4 new_n );
        int             set_name( char * new_name );
        int             set_subdev_quantity( u_int_4 new_dev_cnt );
#ifdef DRIVER        
        i_load_device*  get_load_dev( u_int_4 idx );

        i_simple_device* get_sub_dev( u_int_4 id ) const;
        i_complex_device* get_sub_complex_dev( char *sub_dev_name ) const;
#endif // DRIVER        

        /// @brief Метод интерфейса @ref i_load_device.
        int load_device( char *buff );

        /// @brief Метод интерфейса @ref i_load_device.
        int load_changed_state( char *buff );

        /// @brief Метод интерфейса @ref i_load_device.
        int load_state( char *buff  );

        /// @brief Вывод на консоль устройств группы.
        void print() const;

        /// @brief Метод интерфейса @ref i_cmd_device.
        int     parse_cmd( char *buff );

        /// @brief Метод интерфейса @ref i_cmd_device.
        u_int_4 get_idx();
        
        /// @brief Метод интерфейса @ref i_cmd_device.
        void    set_idx( u_int_4 new_idx );
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Коммуникатор устройств - содержит все устройства одного PAC. Служит
/// для передачи информации о них и их состоянии на сервер (PC).
class device_communicator
#ifdef DRIVER        
: public i_load_device        
#endif // DRIVER                
    {   
    public:
        enum CMD
            {
            GET_INFO_ON_CONNECT = 10, ///< Запрос инф. о PAC перед дальнейшей работой.

            GET_DEVICES = 100,
            GET_DEVICES_STATES,
            GET_DEVICES_CHANGED_STATES,
            EXEC_DEVICE_CMD,

            GET_PAC_ERRORS,
            SET_PAC_ERROR_CMD,
            };

    private:
#ifdef PAC
        static unsigned int     dev_cnt;

    public:
        static i_complex_device **dev;
#else
        unsigned int     dev_cnt;
        i_complex_device **dev;
#endif // PAC

    public:
        device_communicator();

        /// @brief Вывод на консоль устройств группы.
        void print() const;

#ifdef DRIVER
        /// @brief Получение устройства по имени.
        ///
        /// @param dev_name - имя устройства.
        /// @param dev_n    - номер устройства.
        /// @param name     -  имя группы.
        ///
        /// @return > 0 - указатель на сложное устройство.
        /// @return   0 - не найдено сложное устройство.
        i_complex_device* get_group( char* dev_name, u_int_4 dev_n, 
            char* name ) const;

        /// @brief Метод интерфейса @ref i_load_device.
        int load_state( char *buff );

        /// @brief Метод интерфейса @ref i_load_device.
        int load_changed_state( char *buff );

        /// @brief Метод интерфейса @ref i_load_device.
        int load_device( char *buff );        
#endif // DRIVER               

        
#ifdef PAC
        /// @brief Добавление устройства.
        int add_device( i_complex_device *dev );

        /// @brief Сервис для работы с device_communicator.
        static long write_devices_states_service( long len, u_char *data,
            u_char *outdata );
#endif //PAC
    };
//-----------------------------------------------------------------------------
#endif // DEVICES_H
