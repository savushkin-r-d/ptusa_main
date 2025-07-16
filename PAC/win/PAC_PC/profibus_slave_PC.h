/// @file profibus_slave_PC.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC
/// по работе с Profibus Slave для PC.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef PROFIBUS_SLAVE_PC
#define PROFIBUS_SLAVE_PC

#include "profibus_slave.h"

//-----------------------------------------------------------------------------
/// @brief Работа с Profibus Slave.
class profibus_slave_PC : public profibus_slave
    {
    public:
        int init()
            {
            return 0;
            }

        int eval()
            {
            return 0;
            }

        static profibus_slave* get_instance()
            {
            return instance;
            }

        virtual double get_double( int offset )
            {
            return 0;
            }

        virtual bool get_bool( int byte_offset, int bit_offset )
            {
           return false;
            }

        virtual void set_bool( int byte_offset, int bit_offset, bool val )
            {
            }

        virtual int get_int( int byte_offset )
            {
            return 0;
            }

        virtual void set_int( int byte_offset, int val )
            {
            }

        virtual int get_int4( int byte_offset ) 
            {
            return 0;
            }
    private:
        static profibus_slave_PC* instance;
    };
//-----------------------------------------------------------------------------

#endif // PROFIBUS_SLAVE
