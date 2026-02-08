/// @file bus_coupler_io.h
/// @brief Работа с устройствами на основе данных с модулей ввода\вывода, узлы
/// ввода\вывода.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef IO_H
#define IO_H

#include "smart_ptr.h"

#include "dtime.h"
//-----------------------------------------------------------------------------
/// @brief Устройство на основе модулей ввода/вывода.
///
/// В общем случае у устройства может быть один или несколько каналов
/// ввода/вывода (дискретных или аналоговых).
class io_device
    {
    public:
        const char* name;

        io_device( const char* name );

        virtual ~io_device();

		enum VENDOR
			{
			WAGO,
			PHOENIX,
			};

		enum IOLINKSTATE
			{
			OK,

			NOTCONNECTED = 100,
			DEVICEERROR,
			};

        enum class ERRORS
            {
            NO_ERR = 0,
            UNDER_RANGE = 2,
            OVER_RANGE = 3,
            OUT_OF_RANGE = 4,

            BAD_IO_DATA = 100,

            LAST_ERR_IDX
            };

#ifndef PTUSA_TEST
    protected:
#endif
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
        /// @param err - результат, 0 - Ок, >0 - ошибка.
        ///
        /// @return - аналоговое значение.
        float get_AI( u_int index, float min_value = 0, float max_value = 0,
            int& err = last_err );

        /// @brief Получение данных канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int_2* get_AI_data( u_int index ) const;


		/// @brief Получение состояния IO-Link устройства типа AI.
        ///
		/// @param index - индекс канала в таблице аналоговых входных каналов
		/// устройства.
		///
		/// @return -  состояние устройства.
		IOLINKSTATE get_AI_IOLINK_state(u_int index);

		/// @brief Получение состояния IO-Link устройства типа AI.
		///
		/// @param index - индекс канала в таблице аналоговых входных каналов
		/// устройства.
		///
		/// @return -  состояние устройства.
		IOLINKSTATE get_AO_IOLINK_state(u_int index);

        /// @brief Получение установленных данных канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int_2* get_AO_write_data( u_int index );

        /// @brief Получение текущих данных канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов
        /// устройства.
        ///
        /// @return -  указатель на данные канала.
        int_2* get_AO_read_data( u_int index );

        virtual void print() const;

#ifdef PTUSA_TEST
        public:
#else
        private:
#endif
    
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
			int* module_offsets; ///< Массив смещений начала адресного пространства модуля IO для канала
			int* logical_ports; ///< Массив логических номеров канала в пределах модуля IO

            int_2  **int_read_values;           ///< Массив значений для чтения.
			int_2  **int_module_read_values;    ///< Массив значений для чтения адресного пространства модуля.
            int_2  **int_write_values;          ///< Массив значений для записи.
            u_char **char_read_values;          ///< Массив значений для чтения.
            u_char **char_write_values;         ///< Массив значений для записи.

            CHANNEL_TYPE type;           ///< Тип канала.

            IO_channels( CHANNEL_TYPE type );

            ~IO_channels();

            void init( int ch_count );

            void init_channel( u_int ch_index, int node, int offset, int module_offset, int logical_port );

            void print() const;
            };

        IO_channels DI_channels;    ///< Каналы дискретного входа.
        IO_channels DO_channels;    ///< Каналы дискретного выхода.
        IO_channels AI_channels;    ///< Каналы аналогового входа.
        IO_channels AO_channels;    ///< Каналы аналогового выхода.

        VENDOR vendor;

    public:

        VENDOR get_io_vendor() const;

        // Lua.
        void init( int DO_count, int DI_count, int AO_count, int AI_count );

        void init_channel( int type, int ch_inex, int node, int offset,
            int module_offset = -1, int logical_port = -1 );

        void set_io_vendor( VENDOR vendor );

        static int last_err;

#ifdef PTUSA_TEST
        void init_and_alloc( int DO_count = 1, int DI_count = 0,
            int AO_count = 0, int AI_count = 0 );
#endif
    };
//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода.
///
/// Реализация чтения и записи состояний модулей ввода/вывода.
class io_manager
    {
    public:
        virtual ~io_manager();

        void print() const;
        void print_log() const;

        /// @brief Чтение модулей ввода.
        ///
        /// @return - 0 - Ок.
        virtual int read_inputs() = 0;

        /// @brief Чтение модулей вывода.
        ///
        /// @return - 0 - Ок.
        virtual int write_outputs() = 0;

        /// @brief Получение единственного экземпляра класса.
        static io_manager* get_instance();

#ifdef PTUSA_TEST
        /// @brief Получение единственного экземпляра класса.
        static io_manager* replace_instance( io_manager* );
#endif

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
        int_2* get_AI_read_data( u_int node_n, u_int offset );

        /// @brief Получение области чтения данных заданного канала
        /// аналогового выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int_2* get_AO_read_data( u_int node_n, u_int offset );

        /// @brief Получение области записи данных заданного канала
        /// аналогового выхода.
        ///
        /// @param node_n - номер узла.
        /// @param offset - смещение в пределах узла.
        ///
        /// @return - указатель на данные канала.
        int_2* get_AO_write_data( u_int node_n, u_int offset );

		//---------------------------------------------------------------------
		/// @brief Узел модулей ввода/вывода.
		//
		///
		struct io_node
			{
			io_node(int type, int number, const char *str_ip_addres, const char *name,
				int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
				int AI_cnt, int AI_size);

			~io_node();

			void print();
			void print_log();

            enum W_CONST
                {
                C_MAX_WAIT_TIME = 6'000,    ///< Время до установки ошибки связи с модулем, мсек.
                //Max 63 устройства PXC Axioline F.
                C_ANALOG_BUF_SIZE = 63 * 32,///< Размер буфера аналоговых модулей.

                C_MAX_DELAY = 60'000,       ///< Макс. время задержки переподключения, мсек.
                C_CNT_TIMEOUT_US = 100'000, ///< Время ожидания подключения от модуля, мксек.

                C_RCV_TIMEOUT_SEC = 0,      ///< Время ожидания ответа от модуля, сек.
                C_RCV_TIMEOUT_US = 250'000, ///< Время ожидания ответа от модуля, мксек.

                ///< Изначальная задержка переподключения к узлу при ошибке связи, мсек.
                C_INITIAL_RECONNECT_DELAY = 500,
                };

			enum TYPES ///< Типы модулей.
			    {
				EMPTY = -1,   ///< Не задан.

				WAGO_750_86x		  = 0,  ///< Wago 750-863.
				WAGO_750_820x		  = 2,  ///< Wago PFC200.

				WAGO_750_XXX_ETHERNET = 100,///< Wago Ethernet 750-341 и т.д.
				PHOENIX_BK_ETH		  = 200,///< Phoenix 2702177
				};

			enum STATES           ///< Cостояния работы с узлом.
				{
				ST_NO_CONNECT = 0,
				ST_OK = 1,
				ST_WARNING = 2,  ///< Node has error/warning (Status Register bits 0-5 set).
				ST_ERROR = -1,  ///< Node enabled but no connection.
				};

			/// Bits 0-5 of Status Register (7996) indicate error/PP
			/// mode conditions. When any of these bits are set, the
			/// node is in error/PP mode state.
			static constexpr u_int_2 STATUS_REG_ERROR_MASK = 0x003F;

			io_node::STATES  state;          ///< Cостояние работы с узлом.
			TYPES   type;            ///< Тип.
			u_int   number;          ///< Номер.
			char    ip_address[16];///< IP-адрес.
			char    name[20];      ///< Имя.

			bool is_active;             ///< Признак работающего узла.
            bool read_io_error_flag = false; ///< Флаг ошибки чтения узла.

			uint32_t  last_poll_time; ///< Время последнего опроса.
			bool    is_set_err;     ///< Установлена ли ошибка связи.
			int     sock;           ///< Сокет соединения.

			// Digital outputs ( DO ).
			u_int  DO_cnt;      ///< Amount of DO.
			u_char *DO;         ///< Current values.
			u_char *DO_;        ///< To write.

			// Analog outputs ( AO ).
			u_int AO_cnt;       			///< Amount of AO.
            int_2 AO[ C_ANALOG_BUF_SIZE ] = { 0 };    ///< Current values.
            int_2 AO_[ C_ANALOG_BUF_SIZE ] = { 0 };   ///< To write.
			u_int *AO_offsets;  			///< Offsets in common data.
			u_int *AO_types;    			///< Channels type.
			u_int AO_size;

			// Digital inputs ( DI ).
			u_int  DI_cnt;      ///< Amount of DI.
			u_char *DI;         ///< Current values.

			// Analog inputs ( AI ).
			u_int AI_cnt;       			///< Amount of AI.
            int_2 AI[ C_ANALOG_BUF_SIZE ] = { 0 };    ///< Current values.
			u_int *AI_offsets;  			///< Offsets in common data.
			u_int *AI_types;    			///< Channels type.
			u_int AI_size;

			uint32_t last_init_time = 0; ///< Время последней попытки подключиться, мсек.
			uint32_t delay_time;         ///< Время ожидания до попытки подключиться, мсек.

			stat_time recv_stat;  ///< Статистика работы с сокетом.
			stat_time send_stat;  ///< Статистика работы с сокетом.

            bool flag_error_read_message = false; ///< Флаг для вывода сообщений об ошибке чтения.
            bool flag_error_write_message = false; ///< Флаг для вывода сообщений об ошибке записи.

            /// Status register (7996) for Phoenix BK ETH nodes.
            /// Used to detect error/warning conditions (bits 0-5),
            /// including PP mode, startup failures, net failures,
            /// etc. Remains 0 for other node types.
            u_int_2 status_register = 0;

            /// Previous status register value for detecting changes.
            u_int_2 prev_status_register = 0;

            /// Flag indicating PP mode alarm is currently active.
            bool is_pp_mode_alarm_set = false;

            /// @brief Get the display state of the node.
            /// @return 1 - node connected and OK, -1 - node enabled
            ///         but no connection, 2 - node has error/warning
            ///         conditions (bits 0-5 set).
            int get_display_state() const;

            private:
                io_node( const io_node& io_node_copy ); // Not implemented.
            };
		//---------------------------------------------------------------------

    protected:
        io_manager();
        u_int       nodes_count;        ///< Количество узлов.
        io_node **nodes;              ///< Узлы.

        /// Единственный экземпляр класса.
        static auto_smart_ptr < io_manager > instance;

    public:
        io_node * get_node( int node_n );

		u_int get_nodes_count();

        /// @brief Установка числа модулей.
        ///
        /// Вызывается из Lua.
        void init( int nodes_count );

        /// @brief Инициализация модуля.
        ///
        /// Вызывается из Lua.
        void add_node( u_int index, int ntype, int address, const char* IP_address,
            const char *name, int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
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

		/// @brief Завершает соединение с узлом
		virtual void disconnect(io_node *node);


    };
//-----------------------------------------------------------------------------
io_manager* G_IO_MANAGER();
//-----------------------------------------------------------------------------
#endif // IO_H

