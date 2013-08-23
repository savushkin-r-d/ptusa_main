#ifndef SAMPLE_H
#define SAMPLE_H

#include <windows.h>

#include <string>
#include <wchar.h>
//------------------------------------------------------------------------------
/// @brief Интерфейс обучающей выборки.
class i_learn_samples
    {
    public:
        /// <summary>
        /// Конструктор с заданием параметров. 
        /// </summary>
        /// <param name="samples_cnt">Количество образов.</param>
        /// <param name="inputs_cnt">Количество входных переменных выборки.</param>
        /// <param name="outputs_cnt">Количество выходных переменных выборки.</param>
        i_learn_samples( int samples_cnt, int inputs_cnt, int outputs_cnt ): 
            inputs_cnt( inputs_cnt ), outputs_cnt( outputs_cnt ), 
            samples_cnt( samples_cnt )
            {                
            }

        /// <summary>
        /// Получение количества образов в обучающей выборке. 
        /// </summary>
        /// <returns></returns>
        virtual int get_samples_cnt() const;

        /// <summary>
        /// Получение образа с заданным номером.
        /// </summary>
        /// <param name="index">Номер образа.</param>
        /// <returns> > 0 - образ, 0 - ошибка. </returns>
        virtual float* get_sample_x( int index ) const = 0;

        /// <summary>
        /// Получение эталонного значения с заданным номером.
        /// </summary>
        /// <param name="index">Номер эталонного значения.</param>
        /// <returns> > 0 - эталонные значения входов, 0 - ошибка.  </returns>
        virtual float* get_sample_y( int index ) const = 0;

        /// <summary>
        /// Получение количество входных переменных выборки.
        /// </summary>
        /// <returns></returns>
        virtual int get_inputs_cnt()  const;

        /// <summary>
        /// Получение количество выходных переменных выборки.
        /// </summary>
        /// <returns></returns>
        virtual int get_outputs_cnt() const;

    protected:
        int inputs_cnt;  /// Количество входных переменных выборки.
        int outputs_cnt; /// Количество выходных переменных выборки.

        int samples_cnt; /// Количество образов.
    };
//------------------------------------------------------------------------------
/// <summary>
///  Обучающая выборка, которая работает с данными в оперативной памяти.
/// </summary>
/// <remarks>
/// Допустим используется 3 образа. Тогда выборка выглядит так:
/// [o - 2, o - 1, o].
/// Где 
///  o     - текущий образ, 
///  o - 1 - образ в предыдущий момент времени - один такт назад,
///  o - 2 - образ два такта назад.
/// Допустим для прогнозирования используется температура t1 и t2, управляющее
/// воздействие u1. Тогда образ выглядит следующим образом (размер окна равен 3 - 
/// общий для всех входных переменных):
///  [t1 - 2, t1 - 1, t1,   t2 - 2, t2 - 1, t2,   u1 - 2, u1 - 1, u1].
/// Где 
///  t1     - текущая температура, 
///  t1 - 1 - температура в предыдущий момент времени - один такт назад,
///  t1 - 2 - температура два такта назад.
/// Аналогично рассматриваются t2 и u1.
/// В памяти хранится n таких образов, при добавлении новых данных образы
/// сдвигаются:
///  [o - 2, o - 1, o] -> [o - 1, o, o] -> [o - 1, o, o'].
/// Для каждого образа есть соответствующее эталонное значение e:
/// [e - 2, e - 1, e].
/// Эталонное значение также вектор из значений нескольких элементов:
/// e = [u + 1, u + 2, q + 1, q + 2]
/// </remarks>
class rt_sample: public i_learn_samples
    {
    public:
        /// <summary>
        /// Создание выборки.
        /// </summary>
        /// <param name="samples_cnt"></param>
        /// <param name="in_size">Количество входов.</param>
        /// <param name="in_var_count">Количество входных переменных.</param>
        /// <param name="out_size">Количество выходов.</param>
        /// <param name="out_var_count">Количество выходных переменных.</param>
        /// <param name="max_var_value">Максимальное значение переменных, для масштабирования значения в интервале [0; 1].</param>
        /// <returns></returns>
        rt_sample( int samples_cnt, int in_size, int in_var_count,
            int out_size, int out_var_count, int max_var_value ) throw (...) :
            i_learn_samples( samples_cnt, in_size, out_size ),
            max_var_value ( max_var_value )
            {     
            if ( in_size % in_var_count )
            	{
                char msg[ 200 ];
                _snprintf( msg, sizeof( msg ),
                    "rt_sample::rt_sample() - ошибка: число входов (%d) должно быть кратно числу переменных (%d).",
                    in_size, in_var_count );

                throw msg;
            	}
            if ( out_size % out_var_count )
                {
                char msg[ 200 ];
                _snprintf( msg, sizeof( msg ),
                    "rt_sample::rt_sample() - ошибка: число выходов (%d) должно быть кратно числу переменных (%d).",
                    out_size, out_var_count );

                throw msg;
                }

            x = new float*[ samples_cnt ];
            for ( int i = 0; i < samples_cnt; i++ )
                {
                x[ i ] = new float[ in_size ];
                }

            y = new float*[ samples_cnt ];
            for ( int i = 0; i < samples_cnt; i++ )
                {
                y[ i ] = new float[ out_size ];
                }

            fake_image = new float[ out_size > in_size ? out_size : in_size ];
            }

        //Возвращает максимальное значение переменных.
        int get_max_var_value() const
            {
            return max_var_value;
            }

        /// <summary>
        /// Получение входного образа с заданным номером.
        /// </summary>
        /// <param name="index">Номер образа (с нуля).</param>
        /// <returns>Образ с заданным номером или заглушку при ошибке.</returns>
        float* get_sample_x( int index ) const
            {
            if ( index < samples_cnt && index >= 0 )
            	{
                return x[ index ];
            	}
            else
                {
                char msg[ 200 ];
                _snprintf( msg, sizeof( msg ),
                    "rt_sample::get_sample_x(...) - ошибка: индекс (%d) должен"
                    " быть меньше числа образов (%d).",
                    index, samples_cnt );

                //throw msg;
                }

            return fake_image;
            }

        /// <summary>
        /// Получение выходного образа с заданным номером.
        /// </summary>
        /// <param name="index">Номер образа (с нуля).</param>
        /// <returns>Образ с заданным номером или заглушку при ошибке.</returns>
        float* get_sample_y( int index ) const
            {
            if ( index < samples_cnt && index >= 0 )
                {
                return y[ index ];
                }
            else
                {
                char msg[ 200 ];
                _snprintf( msg, sizeof( msg ),
                    "rt_sample::get_sample_x(...) - ошибка: индекс (%d) должен "
                    "быть меньше числа образов (%d).",
                    index, samples_cnt );

                //throw msg;
                }

            return fake_image;
            }

        void print()
            {
            }

    private:     

        int in_var_count;
        int out_var_count;

        int max_var_value;
                
        float **x;      ///Эталонные входы.
        float **y;      ///Эталонные выходы.  

        float *fake_image;
    };
#endif // SAMPLE_H