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
///  Обучающая выборка, которая работает с данными из файла.
/// </summary>
/// <remarks>
/// Файл имеет следующую структуру:
///   число переменных (столбцов данных);
///   число данных (строк данных);
///   макс. значение, на него будут делиться числа, чтобы получить значения < 1;
///   далее идут данные в столбцах.
/// Пример, sample.txt:
/// 3
/// 5
/// 200
/// 122 134 234 
/// 102 134  34 
/// 122 134   9 
/// 102 234  34 
/// </remarks>
class stored_sample: public i_learn_samples
    {
    public:

        //Возвращает коэффициент масштабирования значений выборки.
        int get_factor_k() const;

        //Возвращает размер окна для отдельного столбца.
        int get_column_window_size() const;

        //Конструктор.
        stored_sample();

        //Возвращает обучающую выборку. Далее на ее основе можно прогнозировать
        //значения.
        float** get_sample_x();

        //Возвращает выборку с заданным номером.
        //ПАРАМЕТРЫ.
        //   index   - номер обучающей выборки;
        //   samle_x - эталонные значения входов.
        //   samle_y - эталонные значения выходов.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        // > 0 - указатель на данные выборки;
        //   0 - ошибка.        
        float* get_sample_x( int index ) const;
        float* get_sample_y( int index ) const;

        /// <summary>
        /// Создание обучающей выборки.
        /// </summary>
        /// <param name="window_size">Размер окна.</param>
        /// <param name="learn_samples_count">Количество образов.</param>
        /// <param name="exclude_every_i">Использовать каждую i-ю строку для формирования обучающих данных. Если равен 1, использовать все строки.</param>
        /// <param name="etalon_column_number"></param>
        /// <returns></returns>
        int create_learn_sample( int window_size, int learn_samples_count = -1, int exclude_every_i = 1, int etalon_column_number = 0, int etalon_columns_cnt = 1, bool exclude_etalons = false );

        void print();

        int load_from_file( char *file_name );
        int load_from_file( char *file_name, char is_ignore_first_column );
        int save_to_file( char *file_name );

    private:        
        int column_cnt;
        int row_cnt;
        int factor_k;

        float **data;   //Считанные значения
        float **x;      //Эталонные входы.
        float **y;      //Эталонные выходы.  
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
        rt_sample( int samples_cnt, int in_size, int in_var_count,
            int out_size, int out_var_count, int max_var_value ) throw (...) :
        i_learn_samples( samples_cnt, in_size, out_size ),
            max_var_value ( max_var_value ),
            in_var_count( in_var_count ),
            out_var_count( out_var_count )
            {     
            if ( in_size % in_var_count )
                {
                char msg[ 200 ];
                _snprintf( msg, sizeof( msg ),
                    "rt_sample::rt_sample() - ошибка: число входов (%d) должно "
                    "быть кратно числу переменных (%d).",
                    in_size, in_var_count );

                throw msg;
                }
            if ( out_size % out_var_count )
                {
                char msg[ 200 ];
                _snprintf( msg, sizeof( msg ),
                    "rt_sample::rt_sample() - ошибка: число выходов (%d) должно "
                    "быть кратно числу переменных (%d).",
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

            for ( int i = 0; i < samples_cnt; i++ )
                {
                for ( int j = 0; j < inputs_cnt; j++ )
                    {
                    x[ i ][ j ] = 0;
                    }                
                }

            for ( int i = 0; i < samples_cnt; i++ )
                {
                for ( int j = 0; j < outputs_cnt; j++ )
                    {
                    y[ i ][ j ] = 0;
                    }                
                }

            fake_image = new float[ out_size > in_size ? out_size : in_size ];
            }

        /// <summary>
        /// Получение максимального значения переменных.
        /// </summary>
        int get_max_var_value() const
            {
            return max_var_value;
            }

        /// <summary>
        /// Сдвиг перед добавлением нового образа.
        /// </summary>
        void shift_images()
            {
            for ( int i = 0; i < samples_cnt - 1; i++ )
                {
                for ( int j = 0; j < inputs_cnt; j++ )
                	{
                    x[ i ][ j ] = x[ i + 1 ][ j ];
                	}                
                }

            for ( int i = 0; i < samples_cnt - 1; i++ )
                {
                for ( int j = 0; j < outputs_cnt; j++ )
                    {
                    y[ i ][ j ] = y[ i + 1 ][ j ];
                    }                
                }
            }

        /// <summary>
        /// Добавление нового значения входной переменной.
        /// </summary>
        void add_new_val_to_in_image( int var_n, float val )
            {
            static int var_total_size = inputs_cnt / in_var_count;

            int start_idx = var_n * var_total_size;
            int finish_idx = start_idx + var_total_size - 1;

            if ( start_idx >= inputs_cnt )
            	{
#ifdef _DEBUG
                printf( "rt_sample::add_new_val_to_in_image(...) - "
                    "ошибка: выход за диапазон (%d).",
                    var_n );
#endif // _DEBUG
            	}

            for ( int i = start_idx; i < finish_idx; i++ )
            	{
                x[ samples_cnt - 1 ][ i ] = x[ samples_cnt - 1 ][ i + 1 ];
            	}
            
            x[ samples_cnt - 1 ][ finish_idx ] = val /*/ max_var_value*/;
            }

        /// <summary>
        /// Добавление нового значения выходной переменной.
        /// </summary>
        void add_new_val_to_out_image( int var_n, float val )
            {
            static int var_total_size = outputs_cnt / out_var_count;

            int start_idx = var_n * var_total_size;
            int finish_idx = start_idx + var_total_size - 1;

            if ( start_idx >= inputs_cnt )
                {
#ifdef _DEBUG
                printf( "rt_sample::add_new_val_to_in_image(...) - "
                    "ошибка: выход за диапазон (%d).",
                    var_n );
#endif // _DEBUG
                }
            
            for ( int i = start_idx; i < finish_idx; i++ )
                {
                y[ samples_cnt - 1 ][ i ] = y[ samples_cnt - 1 ][ i + 1 ];
                }

            y[ samples_cnt - 1 ][ finish_idx ] = val/* / max_var_value*/;
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
        /// Получение последнего входного образа.
        /// </summary>
        /// <returns>Последний образ.</returns>
        float* get_last_sample_x() const
            {
            return x[ samples_cnt - 1 ];
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
            for ( int i = 0; i < samples_cnt; i++ )
                {
                printf( "%3d: [", i );
                for ( int j = 0; j < inputs_cnt; j++ )
                    {
                    printf( "%.2f ", x[ i ][ j ] );
                    }
                printf( "] -> [" );

                for ( int j = 0; j < outputs_cnt; j++ )
                    {
                    printf( "%.2f ", y[ i ][ j ] );
                    }     
                printf( "]\n" );
                }   
            printf( "\n");
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