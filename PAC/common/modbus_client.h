#ifndef modbus_client_h__
#define modbus_client_h__
#include "tcp_client.h"

class modbus_client 
    {
    protected:
        tcp_client* tcpclient;
        int modbus_expected_length;
        int modbus_async_result;
        unsigned char stationid;
        unsigned int ormask;
        unsigned int andmask;
        static const int write_buff_start = 13; //Начало буфера данных для операций записи
        static const int read_buff_start = 9;  //Начало буфера данных для операций чтения

        void init_frame(unsigned int address, unsigned int value, unsigned int seventh_byte);

    public:
        modbus_client(unsigned int id, const char* ip, unsigned int port = 502, unsigned long exchangetimeout = 50);
        int get_id();
        //реализация функций протокола modbus
        int read_discrete_inputs(unsigned int start_address, unsigned int quantity);
        int read_coils(unsigned int start_address, unsigned int quantity);
        int read_holding_registers(unsigned int address, unsigned int quantity);
        int read_input_registers(unsigned int address, unsigned int quantity);
        int write_coil(unsigned int address, unsigned char value);
        int force_multiply_coils(unsigned int address, unsigned int quantity);
        int write_multiply_registers(unsigned int address, unsigned int quantity);
        int async_read_discrete_inputs(unsigned int start_address, unsigned int quantity);
        int async_read_coils(unsigned int start_address, unsigned int quantity);
        int async_read_holding_registers(unsigned int address, unsigned int quantity);
        int async_read_input_registers(unsigned int address, unsigned int quantity);
        int async_write_coil(unsigned int address, unsigned char value);
        int async_force_multiply_coils(unsigned int address, unsigned int quantity);
        int async_write_multiply_registers(unsigned int address, unsigned int quantity);
        int async_read_write_multiply_registers(unsigned int readaddress, unsigned int readquantity, unsigned int wrireaddress, unsigned int writequantity);
        int async_mask_write_register(unsigned int writeaddress, unsigned int andmask, unsigned int ormask);
        int async_mask_write_register(unsigned int writeaddress);
        int get_async_result();
        int get_connected_state();
        void set_station(unsigned char new_station_id);
        //функции для работы с буфером из lua
        void zero_output_buff(int startpos = write_buff_start);
        void set_byte(int address, unsigned char value);
        unsigned char get_byte(int address);
        void set_int2(unsigned int address, int_2 value);
        int_2 get_int2(unsigned int address);
        void set_int4(unsigned int address, int_4 value);
        int_4 get_int4(unsigned int address);
        void set_float(unsigned int address, float value);
        float get_float(unsigned int address);
        void set_bit(unsigned int address, int value);
        int reg_get_bit(unsigned int reg, unsigned int offset);
        void reg_set_bit(unsigned int reg, unsigned int offset, int value);
        int get_bit(unsigned int address);
        void mask_reset();
        void mask_set_bit(int pos, int value);
        unsigned char reverse(unsigned char b);
        int swapBits(int x, int p1, int p2, int n);
        ~modbus_client();
    };
#endif // modbus_client_h__
