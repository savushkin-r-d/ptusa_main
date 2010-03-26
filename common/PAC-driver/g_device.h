//ОБЩЕЕ ОПИСАНИЕ.
//  Содержит описания классов, которые используются совместно в контроллере и
//  драйвере на сервере. Описываются базовые интерфейсы, базовые классы.
//  Пример устройства:
//    GLOB0V512 - GLOB 0 V 512 - Клапан номер 512 среди устройств общих устройств PAC;
//    TANK1PAR12 - TANK 1 PAR 12 - Параметр 1 танка номер 1.
//
//ОПИСАНИЕ DEFINE.
//  PAC   - определяет компиляцию для контроллера.
//  WIN32 - определяет компиляцию для драйвера сервера (PC).

#ifndef GENERAL_DEVICE_H
#define GENERAL_DEVICE_H

#ifndef PAC 
#ifndef WIN32
#define PAC
#endif
#endif


#ifdef PAC
#if !defined I7186_E && !defined I7188_E
#include "cmmctr.h"
#else
#include "tcpcmctr.h"
#endif
#endif //!defined I7186_E && !defined I7188_E

#ifdef WIN32
#include "CmnHdr.h"
typedef short           int_2;

typedef unsigned int    u_int_4;
typedef unsigned short  u_int_2;
typedef unsigned char   uchar;
typedef unsigned int    uint;


#define Print           printf
#endif //WIN32

#ifdef PAC
typedef int             int_2;

typedef unsigned long   u_int_4;
typedef unsigned int    u_int_2;
typedef unsigned char   u_char;
#endif //PAC

//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Вывод сообщения на консоль (для PAC), или в окно драйвера (для PC).
void print_str( char *err_str, char is_need_CR = 1 );
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Итерфейс устройства, позволяющий сохранить его в потоке байтов.
class i_save_device    
    {    
    public:        
        //Сохранение состояния устройства в буфер.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        //  > 0 - количество записанных байт.
        //ПАРАМЕТРЫ.
        //  buff - битовый поток, куда записываются данные.
        virtual int save_state( char *buff  ) = 0;

		//Сохранение измененного состояния устройства в буфер.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        //  > 0 - количество записанных байт.
        //ПАРАМЕТРЫ.
        //  buff - битовый поток, куда записываются данные.
        virtual int save_changed_state( char *buff ) = 0;

        //Сохранение самого устройства в буфер.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        //  > 0 - количество записанных байт.
        //ПАРАМЕТРЫ.
        //  buff - битовый поток, куда записываются данные.
        virtual int save_device( char *buff  ) = 0;      
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Итерфейс устройства, позволяющий считать его из потока байтов.
class i_load_device    
    {    
    public:
        //Считывание состояния устройства из буфера.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        //  > 0 - количество считанных байт.
        //ПАРАМЕТРЫ.
        //  buff - битовый поток, откуда считываются данные.
        virtual int load_state( char *buff  ) = 0;
        
		//Считывание измененного состояния устройства из буфера.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        //  > 0 - количество считанных байт.
        //ПАРАМЕТРЫ.
        //  buff - битовый поток, откуда считываются данные.
        virtual int load_changed_state( char *buff ) = 0;  

        //Считывание самого устройства из буфера.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        //  > 0 - количество считанных байт.
        //ПАРАМЕТРЫ.
        //  buff - битовый поток, откуда считываются данные.
        virtual int load_device( char *buff  ) = 0; 
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Итерфейс устройства, позволяющий сохранить его в потоке байтов.
class i_cmd_device    
    {    
    public:        
        //Выполнение команды, хранящейся в буфере.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //  < 0 - ошибка.
        // > 0 - количество обработанных байт.
        //ПАРАМЕТРЫ.
        // buff - битовый поток, откуда считываются данные.
        virtual int parse_cmd( char *buff  ) = 0;    
    };
//-----------------------------------------------------------------------------
class i_device    
    {    
    public:        
        //Получение устройства в массиве устройств.
        virtual u_int_4 get_idx() = 0;                  

        //Установление устройства в массиве устройств.
        virtual void set_idx( u_int_4 new_idx ) = 0; 

        //Возвращает номер устройства.
        virtual u_int_4 get_n() const = 0;

        //Вывод на консоль устройства.
        virtual void print() const = 0;         
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Интерфейс простого устройства.
class i_simple_device: public i_device, public i_cmd_device, 
    public i_load_device, public i_save_device
    {     
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Интерфейс сложного устройства. Оно состоит из групп простых устройств.
class i_complex_device: public i_simple_device
    {
    public:

#ifdef WIN32
        i_complex_device  *compl_dev;  //Сложное устройство, к которой принадлежит 
                                       //группа.
#endif //WIN32
        enum TYPES
            {
            COMPLEX_DEV,        //Сложное устройство.
            GROUP_DEV_BYTE,     //Группа устройств с размерностью данных 1 байт, тип byte.
            GROUP_DEV_LONG,     // -//- 4 байта, тип unsigned long.
            GROUP_DEV_FLOAT = 4,// -//- 4 байта, тип float.
            ARRAY_DEV_BYTE,     //Сложное устройство. Данные в виде массива, тип byte.
            ARRAY_DEV_FLOAT,    // -//- тип float.
            ARRAY_DEV_LONG,     // -//- тип long int.
            ARRAY_DEV_UINT,     // -//- тип unsigned int.

            ARRAY_DEV_INT,      // -//- тип int.

            ARRAY_DEV_STR,      // -//- тип строка.
            ARRAY_DEV_ULONG,    // -//- тип unsigned long int.
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
        virtual i_load_device*  get_load_dev( u_int_4 idx ) = 0;        
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Базовое сложное устройства.
class complex_device: public i_complex_device
    {
    public:
        i_simple_device **sub_dev;  //Подустройства.

    protected:
        u_int_4     sub_dev_cnt;    //Количество подустройств.
        static int  MAX_NAME_LENGTH;//Максимальная длина имени.
        char        *name;          //Имя.

        char        type;           //Тип.
        u_int_4     n;              //Уникальный номер.
        u_int_4     idx;            //Номер устройства массиве устройств.

    public:      
        complex_device();
        complex_device( u_int_4 n, char *new_name, u_int_2 new_subdev_cnt, 
            char type );

#ifdef WIN32
        ~complex_device();

        complex_device( u_int_4 n, char *new_name, u_int_2 new_subdev_cnt, 
            char type, i_complex_device *owner_compl_dev );
#endif //WIN32

        char            get_type() const;
        u_int_4         get_n() const;
        const char*     get_name() const;
        u_int_4         get_subdev_quantity() const;
        i_save_device*  get_save_dev( u_int_4 idx ) const;
                
        int save_device( char *buff );              //Интерфейс i_save_device.
        int save_changed_state( char *buff );
        int save_state( char *buff );


        int set_type( char new_type );
        int set_n( u_int_4 new_n );
        int set_name( char * new_name );
        int set_subdev_quantity( u_int_4 new_dev_cnt );
        i_load_device* get_load_dev( u_int_4 idx );

       
        int load_device( char *buff );              //Интерфейс i_load_device.
        int load_changed_state( char *buff );       
        int load_state( char *buff  );

        //Вывод на консоль устройств группы.
        void print() const;

        i_simple_device* get_sub_dev( u_int_4 id ) const;

        i_complex_device* get_sub_complex_dev( char *sub_dev_name ) const;
                                                                             
        int     parse_cmd( char *buff  );           //Интерфейс i_cmd_device.
        u_int_4 get_idx();
        void    set_idx( u_int_4 new_idx );
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Коммуникатор устройств - содержит все устройства одного PAC. Служит для передачи 
//  информации о них и их состоянии на сервер (PC).
class device_communicator: public i_load_device
    {   
    public:
        enum CMD
            {
            GET_INFO_ON_CONNECT = 10,   //Запрос инф. о PAC перед дальнейшей работой.

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

        //Возвращает устройство по имени.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - не найдено сложное устройство.
        // > 0 - указатель на сложное устройство.
        //ПАРАМЕТРЫ.
        //  dev_name - имя сложного составного устройства (1), 
        //  dev_n    - номер устройства, 
        //  name     - имя устройства, являющегося частью (1).  
        i_complex_device* get_group( char* dev_name, u_int_4 dev_n, 
            char* name ) const;

        //Вывод на консоль устройств группы.
        void print() const;
        
        int load_state( char *buff );               //Интерфейс i_load_device.
        int load_changed_state( char *buff );       
        int load_device( char *buff );      
        
#ifdef PAC
        int add_device( i_complex_device *dev );

        //Сервис для работы с device_communicator.
        static long write_devices_states_service( DESTDATA dest, long len, 
            unsigned char *data, unsigned  char *outdata );
#endif //PAC
    };
//-----------------------------------------------------------------------------
#endif  //DEVICES_H