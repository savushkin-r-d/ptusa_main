/// @file bus_coupler_io.h
/// @brief ������ � ������������ �� ������ ������ � ������� �����\������, ����
/// �����\������.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef IO_H
#define IO_H

#include "smart_ptr.h"

#include "dtime.h"
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ������� �����/������.
///
/// � ����� ������ � ���������� ����� ���� ���� ��� ��������� �������
/// �����/������ (���������� ��� ����������).
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

    protected:
        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� �������
        /// ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int get_DO( u_int index );

        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� �������
        /// ����������.
        /// @param value - ����� ��������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int set_DO( u_int index, char value );

        /// @brief ��������� ��������� ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� �������
        /// ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int get_DI( u_int index );

        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� �������
        /// ����������.
        /// @param min_value - ����������� �������� ������.
        /// @param max_value - ������������ �������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        float get_AO( u_int index, float min_value, float max_value );

        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� �������
        /// ����������.
        /// @param value - ����� ��������� ������.
        /// @param min_value - ����������� �������� ������.
        /// @param max_value - ������������ �������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int set_AO( u_int index, float value, float min_value = 0,
            float max_value = 0 );

        /// @brief ��������� ��������� ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� �������
        /// ����������.
        /// @param min_value - ����������� �������� ������.
        /// @param max_value - ������������ �������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        float get_AI( u_int index, float min_value = 0, float max_value = 0 );

        /// @brief ��������� ������ ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� �������
        /// ����������.
        ///
        /// @return -  ��������� �� ������ ������.
        int_2* get_AI_data( u_int index );

        /// @brief ��������� ������������� ������ ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� �������
        /// ����������.
        ///
        /// @return -  ��������� �� ������ ������.
        int_2* get_AO_write_data( u_int index );

        /// @brief ��������� ������� ������ ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� �������
        /// ����������.
        ///
        /// @return -  ��������� �� ������ ������.
        int_2* get_AO_read_data( u_int index );

        virtual void print() const;

    private:
        /// @brief ������ ������� �����/������ ����������.
        struct IO_channels
            {
            enum CHANNEL_TYPE
                {
                CT_DI = 1,
                CT_DO,
                CT_AI,
                CT_AO,
                };

            u_int  count;   ///< ���������� �������.
            u_int* tables;  ///< ������ ������.
            u_int* offsets; ///< ������ �������� � �������� ������.

            int_2  **int_read_values;      ///< ������ �������� ��� ������.
            int_2  **int_write_values;     ///< ������ �������� ��� ������.
            u_char **char_read_values;     ///< ������ �������� ��� ������.
            u_char **char_write_values;    ///< ������ �������� ��� ������.

            CHANNEL_TYPE type;           ///< ��� ������.

            IO_channels( CHANNEL_TYPE type );

            ~IO_channels();

            void init( int ch_count );

            void init_channel( u_int ch_index, int node, int offset );

            void print() const;
            };

        IO_channels DI_channels;    ///< ������ ����������� �����.
        IO_channels DO_channels;    ///< ������ ����������� ������.
        IO_channels AI_channels;    ///< ������ ����������� �����.
        IO_channels AO_channels;    ///< ������ ����������� ������.

        VENDOR vendor;

    public:

        VENDOR get_io_vendor() const;

        // Lua.
        void init( int DO_count, int DI_count,
            int AO_count, int AI_count );

        void init_channel( int type, int ch_inex, int node, int offset );

        void set_io_vendor( VENDOR vendor );
    };
//-----------------------------------------------------------------------------
/// @brief ������ � �������� �����/������.
///
/// ���������� ������ � ������ ��������� ������� �����/������.
class io_manager
    {
    public:
        virtual ~io_manager();

        void print() const;
        void print_log() const;

        /// @brief ������ ������� �����.
        ///
        /// @return - 0 - ��.
        virtual int read_inputs() = 0;

        /// @brief ������ ������� ������.
        ///
        /// @return - 0 - ��.
        virtual int write_outputs() = 0;

        /// @brief ��������� ������������� ���������� ������.
        static io_manager* get_instance();

        /// @brief ��������� ������� ������ ��������� ������ ����������� �����.
        ///
        /// @param node_n - ����� ����.
        /// @param offset - �������� � �������� ����.
        ///
        /// @return - ��������� �� ������ ������.
        u_char* get_DI_read_data( u_int node_n, u_int offset );

        /// @brief ��������� ������� ������ ������ ��������� ������
        /// ����������� ������.
        ///
        /// @param node_n - ����� ����.
        /// @param offset - �������� � �������� ����.
        ///
        /// @return - ��������� �� ������ ������.
        u_char* get_DO_read_data( u_int node_n, u_int offset );

        /// @brief ��������� ������� ������ ������ ��������� ������
        /// ����������� ������.
        ///
        /// @param node_n - ����� ����.
        /// @param offset - �������� � �������� ����.
        ///
        /// @return - ��������� �� ������ ������.
        u_char* get_DO_write_data( u_int node_n, u_int offset );

        /// @brief ��������� ������� ������ ��������� ������ ����������� �����.
        ///
        /// @param node_n - ����� ����.
        /// @param offset - �������� � �������� ����.
        ///
        /// @return - ��������� �� ������ ������.
        int_2* get_AI_read_data( u_int node_n, u_int offset );

        /// @brief ��������� ������� ������ ������ ��������� ������
        /// ����������� ������.
        ///
        /// @param node_n - ����� ����.
        /// @param offset - �������� � �������� ����.
        ///
        /// @return - ��������� �� ������ ������.
        int_2* get_AO_read_data( u_int node_n, u_int offset );

        /// @brief ��������� ������� ������ ������ ��������� ������
        /// ����������� ������.
        ///
        /// @param node_n - ����� ����.
        /// @param offset - �������� � �������� ����.
        ///
        /// @return - ��������� �� ������ ������.
        int_2* get_AO_write_data( u_int node_n, u_int offset );

		//---------------------------------------------------------------------
		/// @brief ���� ������� �����/������.
		//
		///
		struct io_node
			{
			io_node(int type, int number, char *str_ip_addres, char *name,
				int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
				int AI_cnt, int AI_size);

			~io_node();

			void print();
			void print_log();

			enum W_CONST
				{
				C_MAX_WAIT_TIME = 6000,		///< ����� �� ��������� ������ ����� � �������, ����.
				C_ANALOG_BUF_SIZE = 256,	///< ������ ������ ���������� �������.
				C_MAX_DELAY = 60000,		///< ����. ����� �������� ���������������, ����.
				C_CNT_TIMEOUT_US = 100000,	///< ����� �������� ����������� �� ������, �����.
				C_RCV_TIMEOUT_US = 250000,	///< ����� �������� ������ �� ������, �����.
				};

			enum TYPES ///< ���� �������.
			{
				EMPTY = -1,   ///< �� �����.

				WAGO_750_86x		  = 0,  ///< Wago 750-863.
				WAGO_750_820x		  = 2,  ///< Wago PFC200.

				WAGO_750_XXX_ETHERNET = 100,///< Wago Ethernet 750-341 � �.�.
				PHOENIX_BK_ETH		  = 200,///< Phoenix 2702177
				};

			enum STATES           ///< C�������� ������ � �����.
				{
				ST_NO_CONNECT = 0,
				ST_OK,
				};

			io_node::STATES  state;          ///< C�������� ������ � �����.
			TYPES   type;            ///< ���.
			u_int   number;          ///< �����.
			char    ip_address[16];///< IP-�����.
			char    name[20];      ///< ���.

			bool is_active;          ///< ������� ����������� ����.

			u_long  last_poll_time; ///< ����� ���������� ������.
			bool    is_set_err;     ///< ����������� �� ������ �����.
			int     sock;           ///< ����� ����������.

			// Digital outputs ( DO ).
			u_int  DO_cnt;      ///< Amount of DO.
			u_char *DO;         ///< Current values.
			u_char *DO_;        ///< To write.

			// Analog outputs ( AO ).
			u_int AO_cnt;       			///< Amount of AO.
			int_2 AO[C_ANALOG_BUF_SIZE];    ///< Current values.
			int_2 AO_[C_ANALOG_BUF_SIZE];   ///< To write.
			u_int *AO_offsets;  			///< Offsets in common data.
			u_int *AO_types;    			///< Channels type.
			u_int AO_size;

			// Digital inputs ( DI ).
			u_int  DI_cnt;      ///< Amount of DI.
			u_char *DI;         ///< Current values.

			// Analog inputs ( AI ).
			u_int AI_cnt;       			///< Amount of AI.
			int_2 AI[C_ANALOG_BUF_SIZE];    ///< Current values.
			u_int *AI_offsets;  			///< Offsets in common data.
			u_int *AI_types;    			///< Channels type.
			u_int AI_size;

			u_long last_init_time; ///< ����� ��������� ������� ������������, ����.
			u_long delay_time;     ///< ����� �������� �� ������� ������������, ����.

			stat_time recv_stat;  ///< ���������� ������ � �������.
			stat_time send_stat;  ///< ���������� ������ � �������.
			};
		//---------------------------------------------------------------------

    protected:
        io_manager();
        u_int       nodes_count;        ///< ���������� �����.
        io_node **nodes;              ///< ����.

        /// ������������ ��������� ������.
        static auto_smart_ptr < io_manager > instance;

    public:
        io_node * get_node( int node_n );

		u_int get_nodes_count();

        /// @brief ��������� ����� �������.
        ///
        /// ���������� �� Lua.
        void init( int nodes_count );

        /// @brief ������������� ������.
        ///
        /// ���������� �� Lua.
        void add_node( u_int index, int ntype, int address, char* IP_address,
        char *name, int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
            int AI_cnt, int AI_size );

        /// @brief ������������� ���������� ������ ����������� ������.
        ///
        /// ���������� �� Lua.
        void init_node_AO( u_int node_index, u_int AO_index,
            u_int type, u_int offset );

        /// @brief ������������� ���������� ������ ����������� �����.
        ///
        /// ���������� �� Lua.
        void init_node_AI( u_int node_index, u_int AI_index,
            u_int type, u_int offset );

		/// @brief ��������� ���������� � �����
		virtual void disconnect(io_node *node);
    };
//-----------------------------------------------------------------------------
io_manager* G_IO_MANAGER();
//-----------------------------------------------------------------------------
#endif // IO_H

