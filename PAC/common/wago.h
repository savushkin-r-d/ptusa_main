/// @file wago.h
/// @brief ������ � Wago - ���������� �� ������ ������ � ������� Wago, ����
/// Wago.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef WAGO_H
#define WAGO_H

#include "smart_ptr.h"

#include "dtime.h"
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ������� �����/������ WAGO.
///
/// � ����� ������ � ���������� ����� ���� ���� ��� ��������� �������
/// �����/������ (���������� ��� ����������).
class wago_device
    {
    public:
        const char* name;

        wago_device( const char* name );

        virtual ~wago_device();

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
            u_char **char_read_values;   ///< ������ �������� ��� ������.
            u_char **char_write_values;  ///< ������ �������� ��� ������.

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

    public:
        // Lua.
        void init( int DO_count, int DI_count,
            int AO_count, int AI_count );

        void init_channel( int type, int ch_inex, int node, int offset );
    };
//-----------------------------------------------------------------------------
/// @brief ������ � �������� �����/������ Wago.
///
/// ���������� ������ � ������ ��������� ������� �����/������ Wago.
class wago_manager
    {
    public:
        virtual ~wago_manager();

        void print() const;

        /// @brief ������ ������� �����.
        ///
        /// @return - 0 - ��.
        virtual int read_inputs() = 0;

        /// @brief ������ ������� ������.
        ///
        /// @return - 0 - ��.
        virtual int write_outputs() = 0;

        /// @brief ��������� ������������� ���������� ������.
        static wago_manager* get_instance();

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
		/// @brief ���� ������� �����/������ Wago.
		//
		///
		struct wago_node
			{
			wago_node(int type, int number, char *str_ip_addres, char *name,
				int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
				int AI_cnt, int AI_size);

			~wago_node();

			void print();

			enum W_CONST
				{
				C_MAX_WAIT_TIME = 6000,  ///< ����� �� ��������� ������ ����� � �������, ����.
				C_ANALOG_BUF_SIZE = 256,   ///< ������ ������ ���������� �������.
				C_MAX_DELAY = 60000, ///< ����. ����� �������� ���������������, ����.
				C_CNT_TIMEOUT_US = 100000, ///< ����� �������� ����������� �� ������, �����.
				C_RCV_TIMEOUT_US = 200000, ///< ����� �������� ������ �� ������, �����.
				};

			enum TYPES ///< ���� �������.
				{
				T_EMPTY = -1,   ///< �� �����.

				T_750_86x = 0,  ///< Wago 750-863.

				T_750_820x = 2, ///< Wago PFC200.

				T_750_XXX_ETHERNET = 100,///< Ethernet 750-341 � �.�.
				};

			enum STATES           ///< C�������� ������ � �����.
				{
				ST_NO_CONNECT = 0,
				ST_OK,
				};

			wago_node::STATES  state;          ///< C�������� ������ � �����.
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
			int_2 AO[C_ANALOG_BUF_SIZE];  ///< Current values.
			int_2 AO_[C_ANALOG_BUF_SIZE]; ///< To write.
			u_int *AO_offsets;  			///< Offsets in common data.
			u_int *AO_types;    			///< Channels type.
			u_int AO_size;

			// Digital inputs ( DI ).
			u_int  DI_cnt;      ///< Amount of DI.
			u_char *DI;         ///< Current values.

			// Analog inputs ( AI ).
			u_int AI_cnt;       			///< Amount of AI.
			int_2 AI[C_ANALOG_BUF_SIZE];  ///< Current values.
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
        wago_manager();
        u_int       nodes_count;        ///< ���������� �����.
        wago_node **nodes;              ///< ����.

        /// ������������ ��������� ������.
        static auto_smart_ptr < wago_manager > instance;

    public:
        wago_node * get_node( int node_n );

		u_int get_nodes_count();

        /// @brief ��������� ����� �������.
        ///
        /// ���������� �� Lua.
        void init( int nodes_count );

        /// @brief ������������� ������ Wago.
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
		virtual void disconnect(wago_node *node);



    };
//-----------------------------------------------------------------------------
wago_manager* G_WAGO_MANAGER();
//-----------------------------------------------------------------------------
#endif // WAGO_H

