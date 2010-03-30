/// @file PAC_dev.h
/// @brief Содержит описания классов, которые реализуют функции передачи 
/// состояния устройств PAC на сервер. Классы используются ТОЛЬКО в 
/// контроллере ( PAC ).
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.@n@n
/// @c WIN     - компиляция для OC Windows.@n@n
/// @c W750    - компиляция для PAC W750.@n
/// @c I7186_E - компиляция для PAC I7186_E.
/// 
/// @par Текущая версия:
/// @$Rev: $.\n
/// @$Author: $.\n
/// @$Date::                    #$.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include <string.h>
#include <stdio.h>
#include "g_device.h"

#ifdef W750
//-----------------------------------------------------------------------------
/// @brief Шаблон класса, который используется для передачи состояния устройств,
/// которые представляются массивом некоторого типа.
//
/// Например: параметры, состояния и т.д.
template < class data_type > class array_device: public i_simple_device
    {
    protected:
        u_int_4         sub_dev_cnt;    //Количество подустройств.    
        char            *name;          //Имя.
        char            type;           //Тип.
        u_int_4         n;              //Уникальный номер.

        data_type*      prev_val;       //Массив предыдущих значений.

    public:        
        array_device( u_int_4 n, char *new_name, u_int_2 new_subdev_cnt,
            char type );

        int  save_device( char *buff );
        int  save_state( char *buff );
        int  save_changed_state( char *buff );

        virtual data_type get_val( int idx ) = 0;

        void    print() const;
        u_int_4 get_n() const;

        int load_state( char *buff );
        int load_changed_state( char *buff );
        int load_device( char *buff );

        int     parse_cmd( char *buff  );
        u_int_4 get_idx();
        void    set_idx( u_int_4 new_idx );
    };
//-----------------------------------------------------------------------------
#endif // W750

#endif // PAC_DEVICES_H

