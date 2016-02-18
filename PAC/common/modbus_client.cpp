#include "modbus_client.h"
#include "console.h"
#include "PAC_err.h"

modbus_client::modbus_client(unsigned int id, char* ip, unsigned int port, unsigned long exchangetimeout )
	{
#ifdef DEBUG
	Print("Create modbus client with ip = %s\n\r", ip);
#endif // DEBUG
	tcpclient = tcp_client::Create( ip, port, id, PAC_critical_errors_manager::AS_MODBUS_DEVICE, 256, exchangetimeout  );
	zero_output_buff();
	modbus_async_result = 0;
	modbus_expected_length = 0;
	}

modbus_client::~modbus_client()
	{
	delete tcpclient;
	}

int modbus_client::read_discrete_inputs( unsigned int start_address, unsigned int quantity )
	{
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = 0;
	tcpclient->buff[5] = 6;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 2;
	tcpclient->buff[8] = ((int_2)start_address) >> 8;
	tcpclient->buff[9] = ((int_2)start_address) & 0xFF;
	tcpclient->buff[10] = ((int_2)quantity) >> 8;;
	tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
	unsigned int res = tcpclient->Communicate(12);
	if (res != 9 + quantity / 8 + (quantity % 8 ? 1:0))
		{
#ifdef DEBUG
		Print("modbus_client_%d: read_discrete_inputs - error in exchange. Received %u bytes\n", tcpclient->get_id(), res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

int modbus_client::read_coils( unsigned int start_address, unsigned int quantity )
	{
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = 0;
	tcpclient->buff[5] = 6;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 1;
	tcpclient->buff[8] = ((int_2)start_address) >> 8;
	tcpclient->buff[9] = ((int_2)start_address) & 0xFF;
	tcpclient->buff[10] = ((int_2)quantity) >> 8;;
	tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
	unsigned int res = tcpclient->Communicate(12);
	if (res != 9 + quantity / 8 + (quantity % 8 ? 1:0))
		{
#ifdef DEBUG
		Print("modbus_client_%d: read_coils - error in exchange. Received %u bytes\n", tcpclient->get_id(), res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

int modbus_client::read_holding_registers( unsigned int address, unsigned int quantity )
	{
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = 0;
	tcpclient->buff[5] = 6;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 3;
	tcpclient->buff[8] = ((int_2)address) >> 8;
	tcpclient->buff[9] = ((int_2)address) & 0xFF;
	tcpclient->buff[10] = ((int_2)quantity) >> 8;;
	tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
	unsigned int res = tcpclient->Communicate(12);
	if (res != 9 + quantity * 2)
		{
#ifdef DEBUG
		Print("modbus_client_%d: read_holding_registers - error in exchange. Received %u bytes\n", tcpclient->get_id(), res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

int modbus_client::read_input_registers( unsigned int address, unsigned int quantity )
	{
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = 0;
	tcpclient->buff[5] = 6;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 4;
	tcpclient->buff[8] = ((int_2)address) >> 8;
	tcpclient->buff[9] = ((int_2)address) & 0xFF;
	tcpclient->buff[10] = ((int_2)quantity) >> 8;;
	tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
	unsigned int res = tcpclient->Communicate(12);
	if (res != 9 + quantity * 2)
		{
#ifdef DEBUG
		Print("modbus_client_%d: read_input_registers at %u- error in exchange. Received %u bytes\n", tcpclient->get_id(), address, res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

int modbus_client::write_coil( unsigned int address, unsigned char value )
	{
	unsigned char bitstate = value ? 0xFF : 0;
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = 0;
	tcpclient->buff[5] = 6;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 5;
	tcpclient->buff[8] = ((int_2)address) >> 8;
	tcpclient->buff[9] = ((int_2)address) & 0xFF;
	tcpclient->buff[10] = bitstate;
	tcpclient->buff[11] = 0;
	int res = tcpclient->Communicate(12);
	if (res != 12)
		{
#ifdef DEBUG
		Print("modbus_client_%d: write coil %u - error in exchange. Received %d bytes\n", tcpclient->get_id(), address, res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

int modbus_client::force_multiply_coils( unsigned int address, unsigned int quantity )
	{
	unsigned char bytecount = (unsigned char)quantity / 8 + (((unsigned char)quantity % 8) ? 1:0);
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = 0;
	tcpclient->buff[5] = 7 + bytecount;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 0x0F;
	tcpclient->buff[8] = ((int_2)address) >> 8;
	tcpclient->buff[9] = ((int_2)address) & 0xFF;
	tcpclient->buff[10] = ((int_2)quantity) >> 8;
	tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
	tcpclient->buff[12] = bytecount;
	int res = tcpclient->Communicate(13 + bytecount);
	zero_output_buff();
	if (res != 12)
		{
#ifdef DEBUG
		Print("modbus_client_%d: force_multiply_coils at %u- error in exchange. Received %d bytes\n", tcpclient->get_id(), address, res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

int modbus_client::write_multiply_registers( unsigned int address, unsigned int quantity )
	{
	unsigned char bytecount = (unsigned char)(quantity * 2);
	tcpclient->buff[0] = 0;
	tcpclient->buff[1] = 0;
	tcpclient->buff[2] = 0;
	tcpclient->buff[3] = 0;
	tcpclient->buff[4] = ((int_2)(7 + bytecount)) >> 8;
	tcpclient->buff[5] = ((int_2)(7 + bytecount)) & 0xFF;
	tcpclient->buff[6] = 1;
	tcpclient->buff[7] = 0x10;
	tcpclient->buff[8] = ((int_2)address) >> 8;
	tcpclient->buff[9] = ((int_2)address) & 0xFF;
	tcpclient->buff[10] = ((int_2)quantity) >> 8;
	tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
	tcpclient->buff[12] = bytecount;
 	int res = tcpclient->Communicate(13 + bytecount);
	zero_output_buff();
	if (res != 12)
		{
#ifdef DEBUG
		Print("modbus_client_%d: force_multiply_registers at %u- error in exchange. Received %d bytes\n", tcpclient->get_id(), address, res);
#endif // DEBUG
		return 0;
		}
	return 1;
	}

void modbus_client::zero_output_buff()
	{
	memset(tcpclient->buff + 13, 0, tcpclient->buff_size - 13);
	}

void modbus_client::set_int2( unsigned int address, int_2 value )
	{
	if (address * 2 > tcpclient->buff_size - 15)
		{
		return;
		}
	tcpclient->buff[13+address * 2] = ((char*)&value)[1];
	tcpclient->buff[14+address * 2] = ((char*)&value)[0];
	}

int_2 modbus_client::get_int2( unsigned int address )
	{
	if (address * 2 > tcpclient->buff_size - 15)
		{
		return 0;
		}
	int_2 result;
	((char*)&result)[0] = tcpclient->buff[10 + address * 2];
	((char*)&result)[1] = tcpclient->buff[9 + address * 2];
	return result;
	}

void modbus_client::set_int4( unsigned int address, int_4 value )
	{
	if (address * 2 > tcpclient->buff_size - 17)
		{
		return;
		}
	tcpclient->buff[13+address * 2] = ((char*)&value)[1];
	tcpclient->buff[14+address * 2] = ((char*)&value)[0];
	tcpclient->buff[15+address * 2] = ((char*)&value)[3];
	tcpclient->buff[16+address * 2] = ((char*)&value)[2];
	}

int_4 modbus_client::get_int4( unsigned int address )
	{
	if (address * 2 > tcpclient->buff_size - 17)
		{
		return 0;
		}
	int_4 result;
	((char*)&result)[0] = tcpclient->buff[10 + address * 2];
	((char*)&result)[1] = tcpclient->buff[9 + address * 2];
	((char*)&result)[2] = tcpclient->buff[12 + address * 2];
	((char*)&result)[3] = tcpclient->buff[11 + address * 2];
	return result;
	}

void modbus_client::set_float( unsigned int address, float value )
	{
	if (address * 2 > tcpclient->buff_size - 17)
		{
		return;
		}
	tcpclient->buff[13+address * 2] = ((char*)&value)[1];
	tcpclient->buff[14+address * 2] = ((char*)&value)[0];
	tcpclient->buff[15+address * 2] = ((char*)&value)[3];
	tcpclient->buff[16+address * 2] = ((char*)&value)[2];
	}

float modbus_client::get_float( unsigned int address )
	{
	if (address * 2 > tcpclient->buff_size - 17)
		{
		return 0;
		}
	float result;
	((char*)&result)[0] = tcpclient->buff[10 + address * 2];
	((char*)&result)[1] = tcpclient->buff[9 + address * 2];
	((char*)&result)[2] = tcpclient->buff[12 + address * 2];
	((char*)&result)[3] = tcpclient->buff[11 + address * 2];
	return result;
	}

void modbus_client::set_bit( unsigned int address, int value )
	{
	int numbyte = address / 8;
	int numbit = address % 8;
	tcpclient->buff[13 + numbyte] = tcpclient->buff[13 + numbyte] | ((value ? 1:0) << numbit);
	}

int modbus_client::get_bit( unsigned int address )
	{
	if (tcpclient->buff[9 + address / 8] & (1 << (address % 8)))
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}

int modbus_client::get_id()
	{
	return tcpclient->get_id();
	}

int modbus_client::async_read_discrete_inputs( unsigned int start_address, unsigned int quantity )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = 0;
            tcpclient->buff[5] = 6;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 2;
            tcpclient->buff[8] = ((int_2)start_address) >> 8;
            tcpclient->buff[9] = ((int_2)start_address) & 0xFF;
            tcpclient->buff[10] = ((int_2)quantity) >> 8;;
            tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
            modbus_async_result = tcpclient->AsyncSend(12);
            modbus_expected_length = 9 + quantity / 8 + (quantity % 8 ? 1:0);
            }
        return 0;
        }
    }

int modbus_client::get_async_result()
	{
	return tcpclient->get_async_result();
	}

int modbus_client::async_read_coils( unsigned int start_address, unsigned int quantity )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = 0;
            tcpclient->buff[5] = 6;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 1;
            tcpclient->buff[8] = ((int_2)start_address) >> 8;
            tcpclient->buff[9] = ((int_2)start_address) & 0xFF;
            tcpclient->buff[10] = ((int_2)quantity) >> 8;;
            tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
            modbus_async_result = tcpclient->AsyncSend(12);
            modbus_expected_length = 9 + quantity / 8 + (quantity % 8 ? 1:0);
            }
        return 0;
        }
    }

int modbus_client::async_read_holding_registers( unsigned int address, unsigned int quantity )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = 0;
            tcpclient->buff[5] = 6;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 3;
            tcpclient->buff[8] = ((int_2)address) >> 8;
            tcpclient->buff[9] = ((int_2)address) & 0xFF;
            tcpclient->buff[10] = ((int_2)quantity) >> 8;;
            tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
            modbus_async_result = tcpclient->AsyncSend(12);
            modbus_expected_length = 9 + quantity * 2;
            }
        return 0;
        }
    }

int modbus_client::async_read_input_registers( unsigned int address, unsigned int quantity )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = 0;
            tcpclient->buff[5] = 6;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 4;
            tcpclient->buff[8] = ((int_2)address) >> 8;
            tcpclient->buff[9] = ((int_2)address) & 0xFF;
            tcpclient->buff[10] = ((int_2)quantity) >> 8;;
            tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
            modbus_async_result = tcpclient->AsyncSend(12);
            modbus_expected_length = 9 + quantity * 2;
            }
        return 0;
        }
    
    }

int modbus_client::async_write_coil( unsigned int address, unsigned char value )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            unsigned char bitstate = value ? 0xFF : 0;
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = 0;
            tcpclient->buff[5] = 6;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 5;
            tcpclient->buff[8] = ((int_2)address) >> 8;
            tcpclient->buff[9] = ((int_2)address) & 0xFF;
            tcpclient->buff[10] = bitstate;
            tcpclient->buff[11] = 0;
            modbus_async_result = tcpclient->AsyncSend(12);
            modbus_expected_length = 12;
            }
        return 0;
        }
    }

int modbus_client::async_force_multiply_coils( unsigned int address, unsigned int quantity )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            unsigned char bytecount = (unsigned char)quantity / 8 + (((unsigned char)quantity % 8) ? 1:0);
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = 0;
            tcpclient->buff[5] = 7 + bytecount;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 0x0F;
            tcpclient->buff[8] = ((int_2)address) >> 8;
            tcpclient->buff[9] = ((int_2)address) & 0xFF;
            tcpclient->buff[10] = ((int_2)quantity) >> 8;
            tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
            tcpclient->buff[12] = bytecount;
            modbus_async_result = tcpclient->AsyncSend(13 + bytecount);
            modbus_expected_length = 12;
            }
        return 0;
        }
    }

int modbus_client::async_write_multiply_registers( unsigned int address, unsigned int quantity )
    {
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
        {
        return 0;
        }
    else
        {
        if (ar > 0 && ar == modbus_expected_length)
            {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            return 1;
            }
        else
            {
            unsigned char bytecount = (unsigned char)(quantity * 2);
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = ((int_2)(7 + bytecount)) >> 8;
            tcpclient->buff[5] = ((int_2)(7 + bytecount)) & 0xFF;
            tcpclient->buff[6] = 1;
            tcpclient->buff[7] = 0x10;
            tcpclient->buff[8] = ((int_2)address) >> 8;
            tcpclient->buff[9] = ((int_2)address) & 0xFF;
            tcpclient->buff[10] = ((int_2)quantity) >> 8;
            tcpclient->buff[11] = ((int_2)quantity) & 0xFF;
            tcpclient->buff[12] = bytecount;
            modbus_async_result = tcpclient->AsyncSend(13 + bytecount);
            modbus_expected_length = 12;
            }
        return 0;
        }
    }
