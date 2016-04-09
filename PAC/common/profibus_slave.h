/// @file profibus_slave.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC
/// по работе с Profibus Slave.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG    - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef PROFIBUS_SLAVE
#define PROFIBUS_SLAVE

//-----------------------------------------------------------------------------
/// @brief Работа с Profibus Slave.
class profibus_slave
    {
public:
    /// @brief Получение единственного экземпляра класса для работы.
    ///
    /// @return - указатель на единственный объект класса @ref
    /// profibus_slave_PFC200.
    static profibus_slave* get_instance()
        {
#ifdef USE_PROFIBUS_SLAVE_PFC200
        return profibus_slave_PFC200::get_instance();
#endif // USE_PROFIBUS_SLAVE_PFC200

        return 0;
        }

    //Доступные в Lua.
    virtual double get_double( int offset ) = 0;
    };
//-----------------------------------------------------------------------------
profibus_slave* G_PROFIBUS_SLAVE_LUA()
    {
    return profibus_slave::get_instance();
    }
#endif // PROFIBUS_SLAVE
