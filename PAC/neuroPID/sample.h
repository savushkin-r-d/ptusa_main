#ifndef SAMPLE_H
#define SAMPLE_H

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
        virtual float* get_sample_x( int index ) = 0;

        /// <summary>
        /// Получение эталонного значения с заданным номером.
        /// </summary>
        /// <param name="index">Номер эталонного значения.</param>
        /// <returns> > 0 - эталонные значения входов, 0 - ошибка.  </returns>
        virtual float* get_sample_y( int index ) = 0;

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
        /// <param name="in_sample_size">Размер окна (общий для всех переменных)</param>
        /// <param name="in_variables_count">Количество переменных.</param>
        /// <param name="outputs_cnt"></param>
        /// <param name="max_var_value">Максимальное значение переменных, для масштабирования значения в интервале [0; 1].</param>
        /// <returns></returns>
        rt_sample( int samples_cnt, int in_sample_size, int in_variables_count,
            int outputs_cnt, int max_var_value ):
            i_learn_samples( samples_cnt, inputs_cnt, outputs_cnt )
            {            
            }

        //Возвращает максимальное значение переменных.
        int get_max_var_value() const
            {
            return max_var_value;
            }

        //Возвращает выборку с заданным номером.
        //ПАРАМЕТРЫ.
        //   index   - номер обучающей выборки;
        //   samle_x - эталонные значения входов.
        //   samle_y - эталонные значения выходов.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        // > 0 - указатель на данные выборки;
        //   0 - ошибка.        
        float* get_sample_x( int index ) const
            {
            }

        float* get_sample_y( int index ) const
            {
            }

        void print()
            {
            }

    private:     

        int column_cnt;
        int row_cnt;
        int max_var_value;

        float **data;   ///Считанные значения
        float **x;      ///Эталонные входы.
        float **y;      ///Эталонные выходы.  
    };
#endif // SAMPLE_H