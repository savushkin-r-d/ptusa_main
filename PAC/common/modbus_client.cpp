#include "modbus_client.h"
#include "console.h"
#include "PAC_err.h"

modbus_client::modbus_client(unsigned int id, const char* ip, unsigned int port, unsigned long exchangetimeout )
    {
    if ( G_DEBUG )
        {
        printf("Create modbus client with ip = %s\n\r", ip);
        }
    tcpclient = tcp_client::Create( ip, port, id, PAC_critical_errors_manager::AS_MODBUS_DEVICE, 256, exchangetimeout  );
    zero_output_buff();
    modbus_async_result = 0;
    modbus_expected_length = 0;
    stationid = 1;
    }

modbus_client::~modbus_client()
    {
    delete tcpclient;
    }

void modbus_client::init_frame( unsigned int address, unsigned int value,
    unsigned int function_code )
    {
    tcpclient->buff[ 0 ] = 0;
    tcpclient->buff[ 1 ] = 0;
    tcpclient->buff[ 2 ] = 0;
    tcpclient->buff[ 3 ] = 0;
    tcpclient->buff[ 4 ] = 0;
    tcpclient->buff[ 5 ] = 6;
    tcpclient->buff[ 6 ] = stationid;
    tcpclient->buff[ 7 ] = function_code;
    tcpclient->buff[ 8 ] = ( (int_2)address ) >> 8;
    tcpclient->buff[ 9 ] = ( (int_2)address ) & 0xFF;
    tcpclient->buff[ 10 ] = ( (int_2)value ) >> 8;
    tcpclient->buff[ 11 ] = ( (int_2)value ) & 0xFF;
    }

int modbus_client::read_discrete_inputs( unsigned int start_address, unsigned int quantity )
    {
    this->init_frame(start_address, quantity, 2);
    if (unsigned int res = tcpclient->Communicate(12); 
        res != 9 + quantity / 8 + (quantity % 8 ? 1:0))
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: read_discrete_inputs - error in exchange. Received %u bytes\n", tcpclient->get_id(), res);
            }
        return 0;
        }
    return 1;
    }

int modbus_client::read_coils( unsigned int start_address, unsigned int quantity )
    {
    this->init_frame(start_address, quantity, 1);
    if (unsigned int res = tcpclient->Communicate(12); 
        res != 9 + quantity / 8 + (quantity % 8 ? 1:0))
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: read_coils - error in exchange. Received %u bytes\n", tcpclient->get_id(), res);
            }
        return 0;
        }
    return 1;
    }

int modbus_client::read_holding_registers( unsigned int address, unsigned int quantity )
    {
    this->init_frame(address, quantity, 3);
    if (unsigned int res = tcpclient->Communicate(12); 
        res != 9 + quantity * 2)
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: read_holding_registers - error in exchange. Received %u bytes\n", tcpclient->get_id(), res);
            }
        return 0;
        }
    return 1;
    }

int modbus_client::read_input_registers( unsigned int address, unsigned int quantity )
    {
    this->init_frame(address, quantity, 4);
    if (unsigned int res = tcpclient->Communicate(12); 
        res != 9 + quantity * 2)
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: read_input_registers at %u- error in exchange. Received %u bytes\n", tcpclient->get_id(), address, res);
            }
        return 0;
        }
    return 1;
    }

int modbus_client::write_coil( unsigned int address, unsigned char value )
    {
    unsigned char bitstate = value ? 0xFF : 0;
    this->init_frame(address, value, 5);
    tcpclient->buff[10] = bitstate;
    tcpclient->buff[11] = 0;
    if (int res = tcpclient->Communicate(12); res != 12)
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: write coil %u - error in exchange. Received %d bytes\n", tcpclient->get_id(), address, res);
            }
        return 0;
        }
    return 1;
    }

int modbus_client::force_multiply_coils( unsigned int address, unsigned int quantity )
    {
    unsigned char bytecount = (unsigned char)quantity / 8 + (((unsigned char)quantity % 8) ? 1:0);
    this->init_frame(address, quantity, 0x0F);
    tcpclient->buff[5] = 7 + bytecount;
    tcpclient->buff[12] = bytecount;
    int res = tcpclient->Communicate(13 + bytecount);
    zero_output_buff();
    if (res != 12)
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: force_multiply_coils at %u- error in exchange. Received %d bytes\n", tcpclient->get_id(), address, res);
            }
        return 0;
        }
    return 1;
    }

int modbus_client::write_multiply_registers( unsigned int address, unsigned int quantity )
    {
    unsigned char bytecount = (unsigned char)(quantity * 2);
    this->init_frame(address, quantity, 0x10);
    tcpclient->buff[4] = ((int_2)(7 + bytecount)) >> 8;
    tcpclient->buff[5] = ((int_2)(7 + bytecount)) & 0xFF;
    tcpclient->buff[12] = bytecount;
    int res = tcpclient->Communicate(13 + bytecount);
    zero_output_buff();
    if (res != 12)
        {
        if ( G_DEBUG )
            {
            printf("modbus_client_%d: force_multiply_registers at %u- error in exchange. Received %d bytes\n", tcpclient->get_id(), address, res);
            }
        return 0;
        }
    return 1;
    }

void modbus_client::zero_output_buff(int startpos /*= write_buff_start*/)
    {
    memset(tcpclient->buff + startpos, 0, tcpclient->buff_size - startpos);
    }

void modbus_client::set_byte(int address, unsigned char value)
    {
    if (address > (int)tcpclient->buff_size - write_buff_start - (int)sizeof(unsigned char) || address + write_buff_start < 0)
        {
        return;
        }
    tcpclient->buff[13 + address] = (unsigned char)value;
    }

unsigned char modbus_client::get_byte(int address)
    {
    if (address > (int)tcpclient->buff_size - read_buff_start - (int)sizeof(unsigned char) || address + read_buff_start < 0)
        {
        return 0;
        }
    return (unsigned char)(tcpclient->buff[9 + address]);
    }

void modbus_client::set_int2( unsigned int address, int_2 value )
    {
    if (address * 2 > tcpclient->buff_size - write_buff_start - sizeof(int_2))
        {
        return;
        }
    tcpclient->buff[write_buff_start + address * 2]     = ((char*)&value)[1];
    tcpclient->buff[write_buff_start + 1 + address * 2] = ((char*)&value)[0];
    }

int_2 modbus_client::get_int2( unsigned int address )
    {
    if (address * 2 > tcpclient->buff_size - read_buff_start - sizeof(int_2))
        {
        return 0;
        }
    int_2 result;
    ((char*)&result)[0] = tcpclient->buff[read_buff_start + 1 + address * 2];
    ((char*)&result)[1] = tcpclient->buff[read_buff_start + address * 2];
    return result;
    }

void modbus_client::set_int4( unsigned int address, int_4 value )
    {
    if (address * 2 > tcpclient->buff_size - write_buff_start - sizeof(int_4))
        {
        return;
        }
    tcpclient->buff[write_buff_start + address * 2]         = ((char*)&value)[1];
    tcpclient->buff[write_buff_start + 1 + address * 2]     = ((char*)&value)[0];
    tcpclient->buff[write_buff_start + 2 + address * 2]     = ((char*)&value)[3];
    tcpclient->buff[write_buff_start + 3 + address * 2]     = ((char*)&value)[2];
    }

int_4 modbus_client::get_int4( unsigned int address )
    {
    if (address * 2 > tcpclient->buff_size - read_buff_start - sizeof(int_4))
        {
        return 0;
        }
    int_4 result;
    ((char*)&result)[0] = tcpclient->buff[read_buff_start + 1 + address * 2];
    ((char*)&result)[1] = tcpclient->buff[read_buff_start + address * 2];
    ((char*)&result)[2] = tcpclient->buff[read_buff_start + 3 + address * 2];
    ((char*)&result)[3] = tcpclient->buff[read_buff_start + 2 + address * 2];
    return result;
    }

void modbus_client::set_float( unsigned int address, float value )
    {
    if (address * 2 > tcpclient->buff_size - write_buff_start - sizeof(float))
        {
        return;
        }
    tcpclient->buff[write_buff_start + address * 2]     = ((char*)&value)[1];
    tcpclient->buff[write_buff_start + 1 + address * 2] = ((char*)&value)[0];
    tcpclient->buff[write_buff_start + 2 + address * 2] = ((char*)&value)[3];
    tcpclient->buff[write_buff_start + 3 + address * 2] = ((char*)&value)[2];
    }

float modbus_client::get_float( unsigned int address )
    {
    if (address * 2 > tcpclient->buff_size - read_buff_start - sizeof(float))
        {
        return 0;
        } 
    float result;
    ((char*)&result)[0] = tcpclient->buff[read_buff_start + 1 + address * 2];
    ((char*)&result)[1] = tcpclient->buff[read_buff_start + address * 2];
    ((char*)&result)[2] = tcpclient->buff[read_buff_start + 3 + address * 2];
    ((char*)&result)[3] = tcpclient->buff[read_buff_start + 2 + address * 2];
    return result;
    }

void modbus_client::set_bit( unsigned int address, int value )
    {
    int numbyte = address / 8;
    int numbit = address % 8;
    tcpclient->buff[write_buff_start + numbyte] ^= (-(value ? 1 : 0) ^ tcpclient->buff[write_buff_start + numbyte]) & (1 << numbit);
    }

int modbus_client::reg_get_bit(unsigned int reg, unsigned int offset)
    {
    unsigned int address = reg * 2 + ((offset < 8) ? 1 : 0);
    if (tcpclient->buff[read_buff_start + address] & (1 << (offset % 8)))
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

void modbus_client::reg_set_bit(unsigned int reg, unsigned int offset, int value)
    {
    int numbyte = reg * 2 + ((offset < 8) ? 1 : 0);
    int numbit = offset % 8;
    tcpclient->buff[write_buff_start + numbyte] ^= (-(value ? 1 : 0) ^ tcpclient->buff[write_buff_start + numbyte]) & (1 << numbit);
    }

int modbus_client::get_bit( unsigned int address )
    {
    if (tcpclient->buff[read_buff_start + address / 8] & (1 << (address % 8)))
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

void modbus_client::mask_reset()
    {
    andmask = 0xFFFF;
    ormask = 0;
    }

void modbus_client::mask_set_bit(int pos, int value)
    {
    if (pos < 0 || pos > 15) return;
    andmask &= ~(1UL << pos);
    if (value)
        {
        ormask |= 1UL << pos;
        }
    else
        {
        ormask &= ~(1UL << pos);
        }
    }

unsigned char modbus_client::reverse(unsigned char b)
    {
#ifdef PAC_WAGO_PFC200
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
#else
    b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
    b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
    b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
#endif
    return b;
    }

int modbus_client::swapBits(int x, int p1, int p2, int n)
    {
    /* Move all bits of first set to rightmost side */
    unsigned int set1 = (x >> p1)& ((1U << n) - 1);
    /* Move all bits of second set to rightmost side */
    unsigned int set2 = (x >> p2)& ((1U << n) - 1);
    /* Xor the two sets */
    unsigned int Xor = (set1 ^ set2);
    /* Put the Xor bits back to their original positions */
    Xor = (Xor << p1) | (Xor << p2);
    /* Xor the 'Xor' with the original number so that the
    two sets are swapped */
    int result = x ^ Xor;
    return result;
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
            this->init_frame(start_address, quantity, 2);
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

int modbus_client::get_connected_state()
    {
    return tcpclient->get_connected_state();
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
            this->init_frame(start_address, quantity, 1);
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
            this->init_frame(address, quantity, 3);
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
            this->init_frame(address, quantity, 4);
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
            this->init_frame(address, value, 5);
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
            this->init_frame(address, quantity, 0x0F);
            tcpclient->buff[5] = 7 + bytecount;
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
            this->init_frame(address, quantity, 0x10);
            tcpclient->buff[4] = ((int_2)(7 + bytecount)) >> 8;
            tcpclient->buff[5] = ((int_2)(7 + bytecount)) & 0xFF;
            tcpclient->buff[12] = bytecount;
            modbus_async_result = tcpclient->AsyncSend(13 + bytecount);
            modbus_expected_length = 12;
            }
        return 0;
        }
    }

int modbus_client::async_read_write_multiply_registers(unsigned int readaddress, unsigned int readquantity, unsigned int writeaddress, unsigned int writequantity)
{
    int ar = get_async_result();
    if (ar == tcp_client::AR_BUSY)
    {
        return 0;
    }
    else
    {
        if (ar > 0 && (ar == modbus_expected_length || ar == 9))
        {
            tcpclient->set_async_result(tcp_client::AR_FREE);
            if (ar == modbus_expected_length)
            {
                return 1;
            } 
            else
            {
                return -1;
            }
        }
        else
        {
            unsigned char bytecount = (unsigned char)(writequantity * 2);
            tcpclient->buff[0] = 0;
            tcpclient->buff[1] = 0;
            tcpclient->buff[2] = 0;
            tcpclient->buff[3] = 0;
            tcpclient->buff[4] = ((int_2)(11 + bytecount)) >> 8;
            tcpclient->buff[5] = ((int_2)(11 + bytecount)) & 0xFF;
            tcpclient->buff[6] = stationid;
            tcpclient->buff[7] = 0x17;
            tcpclient->buff[8] = ((int_2)readaddress) >> 8;
            tcpclient->buff[9] = ((int_2)readaddress) & 0xFF;
            tcpclient->buff[10] = ((int_2)readquantity) >> 8;
            tcpclient->buff[11] = ((int_2)readquantity) & 0xFF;
            tcpclient->buff[12] = ((int_2)writeaddress) >> 8;
            tcpclient->buff[13] = ((int_2)writeaddress) & 0xFF;
            tcpclient->buff[14] = ((int_2)writequantity) >> 8;
            tcpclient->buff[15] = ((int_2)writequantity) & 0xFF;
            tcpclient->buff[16] = bytecount;
            modbus_async_result = tcpclient->AsyncSend(17 + bytecount);
            modbus_expected_length = 9 + readquantity * 2;
        }
        return 0;
    }
}

int modbus_client::async_mask_write_register(unsigned int writeaddress, unsigned int andmask, unsigned int ormask)
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
            this->init_frame(writeaddress, andmask, 0x16);
            tcpclient->buff[5] = 0x08;
            tcpclient->buff[12] = ((int_2)ormask) >> 8;
            tcpclient->buff[13] = ((int_2)ormask) & 0xFF;
            modbus_async_result = tcpclient->AsyncSend(14);
            modbus_expected_length = 14;
            }
        return 0;
        }
    }

int modbus_client::async_mask_write_register(unsigned int writeaddress)
    {
    return async_mask_write_register(writeaddress, andmask, ormask);
    }

void modbus_client::set_station( unsigned char new_station_id )
    {
    stationid = new_station_id;
    }
