/// @file profibus_slave.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC
/// по работе с Profibus Slave.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
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
    //Конфигурирование клиента.
    public:
        /// <summary>
        /// Включение модуля обмена.
        /// </summary>
        void activate()
            {
            active = true;
            }

        /// <summary>
        /// Установка адреса станции.
        /// </summary>
        void set_station_address( int address )
            {
            station_address = address;
            }

        /// <summary>
        /// Установка размера массива области записи.
        /// </summary>
        void set_output_byte_size( int size )
            {
            output_byte_size = size;
            }

        /// <summary>
        /// Установка размера массива области чтения.
        /// </summary>
        void set_input_byte_size( int size )
            {
            input_byte_size = size;
            }

    public:
        profibus_slave()
            {
            active = false;
            station_address = 2;
            output_byte_size = 1;
            input_byte_size = 1;
            }

        inline bool is_active() const
            {
            return active;
            };

        virtual int init() = 0;
        virtual int eval() = 0;

    public:
        /// @brief Получение единственного экземпляра класса для работы.
        ///
        /// @return - указатель на единственный объект класса.
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

        /// <summary>
        /// Получение значения типа int.
        /// </summary>
        /// <param name="byte_offset">Смещение, диапазон 0..242.</param>
        virtual int get_int( int byte_offset ) = 0;

        /// <summary>
        /// Получение значения типа int (4 байта).
        /// </summary>
        /// <param name="byte_offset">Смещение, диапазон 0..240.</param>
        virtual int get_int4( int byte_offset ) = 0;

        /// <summary>
        /// Установка значения типа int.
        /// </summary>
        /// <param name="byte_offset">Смещение, диапазон 0..242.</param>
        /// <param name="val">Значение.</param>
        virtual void set_int( int byte_offset, int val ) = 0;

    protected:
        bool active;
        int station_address;
        int output_byte_size;
        int input_byte_size;
    };
//-----------------------------------------------------------------------------
profibus_slave* G_PROFIBUS_SLAVE_LUA();
profibus_slave* G_PROFIBUS_SLAVE();

#endif // PROFIBUS_SLAVE
