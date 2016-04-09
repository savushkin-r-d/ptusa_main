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
    static profibus_slave* get_instance();

    /// <summary>
    /// Получение значения типа double.
    /// </summary>
    /// <param name="offset">Смещение, диапазон 0..239.</param>
    virtual double get_double( int offset ) = 0;

    /// <summary>
    /// Получение значения типа bool.
    /// </summary>
    /// <param name="byte_offset">Смещение, диапазон 0..243.</param>
    /// <param name="bit_offset">Смещение, диапазон 0..7.</param>
    virtual bool get_bool( int byte_offset, int bit_offset ) = 0;

    /// <summary>
    /// Установка значения типа bool.
    /// </summary>
    /// <param name="byte_offset">Смещение, диапазон 0..243.</param>
    /// <param name="bit_offset">Смещение, диапазон 0..7.</param>
    /// <param name="val">Значение.</param>
    virtual void set_bool( int byte_offset, int bit_offset, bool val ) = 0;
    };
//-----------------------------------------------------------------------------
profibus_slave* G_PROFIBUS_SLAVE_LUA();

#endif // PROFIBUS_SLAVE
