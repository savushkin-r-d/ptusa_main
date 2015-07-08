/// @file wago.h
/// @brief Работа с Wago - устройства на основе данных с модулей Wago, узлы
/// Wago.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef WAGO_H
#define WAGO_H

#include "smart_ptr.h"
//-----------------------------------------------------------------------------
/// @brief Устройство на основе модулей ввода/вывода WAGO.
///
/// В общем случае у устройства может быть один или несколько каналов
/// ввода/вывода (дискретных или аналоговых).
class wago_device
    {
    public:
        const char* name;

        wago_device( const char* name );

        virtual ~wago_device();

    protected:
        /// @brief Получение состояния канала дискретного выхода.
        ///
        /// @param index - индекс канала в таблице дискретных выходных каналов
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int get_DO( u_int index );

        /// @brief Установка состояния канала дискретного выхода.
        ///
        /// @param index - индекс канала в таблице дискретных выходных каналов
        /// устройства.
        /// @param value - новое состояние канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_DO( u_int index, char value );

        /// @brief Получение состояния канала дискретного входа.
        ///
        /// @param index - индекс канала в таблице дискретных входных каналов
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int get_DI( u_int index );

        /// @brief Получение состояния канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        /// @param min_value - минимальное значение канала.
        /// @param max_value - максимальное значение канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        float get_AO( u_int index, float min_value, float max_value );

        /// @brief Установка состояния канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        /// @param value - новое состояние канала.
        /// @param min_value - минимальное значение канала.
        /// @param max_value - максимальное значение канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_AO( u_int index, float value, float min_value = 0,
            float max_value = 0 );

        /// @brief Получение состояния канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов
        /// устройства.
        /// @param min_value - минимальное значение канала.
        /// @param max_value - максимальное значение канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        float get_AI( u_int index, float min_value = 0, float max_value = 0 );

        /// @brief Получение данных канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int* get_AI_data( u_int index );

        /// @brief Получение установленных данных канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int* get_AO_write_data( u_int index );

        /// @brief Получение текущих данных канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int* get_AO_read_data( u_int index );

        virtual void print() const;

    private:
        /// @brief Группа каналов ввода/вывода устройства.
        struct IO_channels
            {
            enum CHANNEL_TYPE
                {
                CT_DI = 1,
                CT_DO,
                CT_AI,
                CT_AO,
                };

            u_int  count;   ///< Количество каналов.
            u_int* tables;  ///< Массив таблиц.
            u_int* offsets; ///< Массив смещений в пределах таблиц.

            int  **int_read_values;      ///< Массив значений для чтения.
            int  **int_write_values;     ///< Массив значений для записи.
            u_char **char_read_values;   ///< Массив значений для чтения.
            u_char **char_write_values;  ///< Массив значений для записи.

            CHANNEL_TYPE type;           ///< Тип канала.

            IO_channels( CHANNEL_TYPE type );

            ~IO_channels();

            void init( int ch_count );

            void init_channel( u_int ch_index, int node, int offset );

            void print() const;
            };

        IO_channels DI_channels;    ///< Каналы дискретного входа.
        IO_channels DO_channels;    ///< Каналы дискретного выхода.
        IO_channels AI_channels;    ///< Каналы аналогового входа.
        IO_channels AO_channels;    ///< Каналы аналогового выхода.

    public:
        // Lua.
        void init( int DO_count, int DI_count,
            int AO_count, int AI_count );

        void init_channel( int type, int ch_inex, int node, int offset );
    };
//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода Wago.
///
/// Реализация чтения и записи состояний модулей ввода/вывода Wago.
class wago_manager
    {
    public:
        virtual ~wago_manager();

        void print() const;

        /// @brief Чтение модулей ввода.
        ///
        /// @return - 0 - Ок.
        virtual int read_inputs() = 0;

        /// @brief Чтение модулей вывода.
        ///
        /// @return - 0 - Ок.
        virtual int write_outputs() = 0;

        /// @brief Получение единственного экземпляра класса.
        static wago_manager* get_instance();

        /// @brief Получение области данных заданного канала дискретного входа.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        u_char* get_DI_read_data( u_int node_n, u_int offset );

        /// @brief Получение области чтения данных заданного канала
        /// дискретного выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        u_char* get_DO_read_data( u_int node_n, u_int offset );

        /// @brief Получение области записи данных заданного канала
        /// дискретного выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        u_char* get_DO_write_data( u_int node_n, u_int offset );

        /// @brief Получение области данных заданного канала аналогового входа.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int* get_AI_read_data( u_int node_n, u_int offset );

        /// @brief Получение области чтения данных заданного канала
        /// аналогового выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int* get_AO_read_data( u_int node_n, u_int offset );

        /// @brief Получение области записи данных заданного канала
        /// аналогового выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int* get_AO_write_data( u_int node_n, u_int offset );

    protected:
        wago_manager();

        //---------------------------------------------------------------------
        /// @brief Узел модулей ввода/вывода Wago.
        //
        ///
        struct wago_node
            {
            wago_node( int type, int number, char *str_ip_addres, char *name,
                int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
                int AI_cnt, int AI_size );

            ~wago_node();

            void print();

            enum W_CONST
                {
                C_MAX_WAIT_TIME   = 10000L,///< Время до установки ошибки связи с модулем, мсек.
                C_ANALOG_BUF_SIZE = 256,   ///< Размер буфера аналоговых модулей.
                C_MAX_DELAY       = 60000, ///< Макс. время задержки переподключения, мсек.
                C_CNT_TIMEOUT_US = 100000, ///< Время ожидания подключения от модуля, мксек.
                C_RCV_TIMEOUT_US = 200000, ///< Время ожидания ответа от модуля, мксек.
                };

            enum TYPES ///< Типы модулей.
                {
                T_EMPTY = -1,   ///< Не задан.

                T_750_86x = 0,  ///< Wago 750-863.

                T_750_820x = 2, ///< Wago PFC200.

                T_750_XXX_ETHERNET = 100,///< Ethernet 750-341 и т.д.
                };

            enum STATES           ///< Cостояния работы с узлом.
                {
                ST_NO_CONNECT = 0,
                ST_OK,
                };

            wago_node::STATES  state;          ///< Cостояние работы с узлом.
            TYPES   type;            ///< Тип.
            u_int   number;          ///< Номер.
            char    ip_address[ 16 ];///< IP-адрес.
            char    name[ 20 ];      ///< Имя.

            bool is_active;          ///< Признак работающего узла.

            u_long  last_poll_time; ///< Время последнего опроса.
            bool    is_set_err;     ///< Установлена ли ошибка связи.
            int     sock;           ///< Сокет соединения.

            // Digital outputs ( DO ).
            u_int  DO_cnt;      ///< Amount of DO.
            u_char *DO;         ///< Current values.
            u_char *DO_;        ///< To write.

            // Analog outputs ( AO ).
            u_int AO_cnt;       ///< Amount of AO.
            int AO[ C_ANALOG_BUF_SIZE ];          ///< Current values.
            int AO_[ C_ANALOG_BUF_SIZE ];         ///< To write.
            u_int *AO_offsets;  ///< Offsets in common data.
            u_int *AO_types;    ///< Channels type.
            u_int AO_size;

            // Digital inputs ( DI ).
            u_int  DI_cnt;      ///< Amount of DI.
            u_char *DI;         ///< Current values.

            // Analog inputs ( AI ).
            u_int AI_cnt;       ///< Amount of AI.
            int AI[ C_ANALOG_BUF_SIZE ];          ///< Current values.
            u_int *AI_offsets;  ///< Offsets in common data.
            u_int *AI_types;    ///< Channels type.
            u_int AI_size;

            u_long last_init_time; ///< Время последней попытки подключиться, мсек.
            u_long delay_time;     ///< Время ожидания до попытки подключиться, мсек.
            };
        //---------------------------------------------------------------------

        u_int       nodes_count;        ///< Количество узлов.
        wago_node **nodes;              ///< Узлы.

        /// Единственный экземпляр класса.
        static auto_smart_ptr < wago_manager > instance;

    public:
        wago_node * get_node( int node_n );

        /// @brief Установка числа модулей.
        ///
        /// Вызывается из Lua.
        void init( int nodes_count );

        /// @brief Инициализация модуля Wago.
        ///
        /// Вызывается из Lua.
        void add_node( u_int index, int ntype, int address, char* IP_address,
        char *name, int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
            int AI_cnt, int AI_size );

        /// @brief Инициализация параметров канала аналогового вывода.
        ///
        /// Вызывается из Lua.
        void init_node_AO( u_int node_index, u_int AO_index,
            u_int type, u_int offset );

        /// @brief Инициализация параметров канала аналогового ввода.
        ///
        /// Вызывается из Lua.
        void init_node_AI( u_int node_index, u_int AI_index,
            u_int type, u_int offset );

    };
//-----------------------------------------------------------------------------
wago_manager* G_WAGO_MANAGER();
//-----------------------------------------------------------------------------
#endif // WAGO_H

